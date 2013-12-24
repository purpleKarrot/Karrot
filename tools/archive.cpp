/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "archive.hpp"

#include <archive.h>
#include <archive_entry.h>
#include <boost/filesystem.hpp>
#include <curl/curl.h>
#include <karrot.h>
#include <openssl/evp.h>
#include <proxy.h>

#include <string>
#include <cstring>
#include <stdexcept>

namespace Karrot
{

namespace fs = boost::filesystem;

class TempDir
{
public:
	TempDir() :
			saved{fs::current_path()}
	{
		auto tmp = fs::temp_directory_path() / fs::unique_path();
		fs::create_directories(tmp);
		fs::current_path(tmp);
	}

	~TempDir()
	{
		auto tmp = fs::current_path();
		fs::current_path(saved);
		fs::remove_all(tmp);
	}

private:
	fs::path saved;
};

struct ArchiveError: std::runtime_error
{
	ArchiveError(archive* ar) :
			std::runtime_error(archive_error_string(ar))
	{
	}
};

static fs::path analyze_extracted()
{
	int found = false;
	auto path = fs::current_path();
	for (fs::directory_iterator it(path), end; it != end; ++it)
	{
		if (found || !is_directory(*it))
		{
			return fs::current_path();
		}

		path = *it;
		found = true;
	}

	return path;
}

static void copy_folder(const fs::path& src, const fs::path& dst)
{
	create_directory(dst);
	for (fs::directory_iterator it(src), end; it != end; ++it)
	{
		fs::path target = dst / it->path().filename();
		if (!exists(target) || !is_directory(*it))
		{
			remove(target);
			rename(*it, target);
		}
		else
		{
			copy_folder(*it, target);
		}
	}
}

static int copy_data(struct archive* ar, struct archive* aw)
{
	for (;;)
	{
		void const* buff;
		size_t size;
		off_t offset;

		int ret = archive_read_data_block(ar, &buff, &size, &offset);
		if (ret == ARCHIVE_EOF)
		{
			return ARCHIVE_OK;
		}

		if (ret != ARCHIVE_OK)
		{
			return ret;
		}

		archive_write_data_block(aw, buff, size, offset);
	}
}

static int extract_local(fs::path const& archive_path)
{
	int ret;

	/* Select which attributes we want to restore. */
	int flags
		= ARCHIVE_EXTRACT_TIME
		| ARCHIVE_EXTRACT_PERM
		| ARCHIVE_EXTRACT_ACL
		| ARCHIVE_EXTRACT_FFLAGS
		;

	using archive_ptr = std::unique_ptr<archive, int (*)(archive*)>;

	archive_ptr read{archive_read_new(), archive_read_free};
	archive_read_support_filter_all(read.get());
	archive_read_support_format_all(read.get());

	archive_ptr write{archive_write_disk_new(), archive_write_free};
	archive_write_disk_set_options(write.get(), flags);
	archive_write_disk_set_standard_lookup(write.get());

#ifdef BOOST_WINDOWS
	if (ret = archive_read_open_filename_w(read.get(), archive_path.c_str(), 10240))
#else
	if (ret = archive_read_open_filename(read.get(), archive_path.c_str(), 10240))
#endif
	{
		return ret;
	}

	for (;;)
	{
		struct archive_entry* entry;
		ret = archive_read_next_header(read.get(), &entry);
		if (ret == ARCHIVE_EOF)
		{
			return ARCHIVE_OK;
		}
		if (ret != ARCHIVE_OK)
		{
			archive_error_string(read.get());
		}
		if (ret < ARCHIVE_WARN)
		{
			return ret;
		}

		ret = archive_write_header(write.get(), entry);
		if (ret != ARCHIVE_OK)
		{
			archive_error_string(write.get());
		}
		else if (archive_entry_size(entry) > 0)
		{
			ret = copy_data(read.get(), write.get());
			if (ret != ARCHIVE_OK)
			{
				archive_error_string(write.get());
			}
			if (ret < ARCHIVE_WARN)
			{
				return ret;
			}
		}

		ret = archive_write_finish_entry(write.get());
		if (ret != ARCHIVE_OK)
		{
			archive_error_string(write.get());
		}
		if (ret < ARCHIVE_WARN)
		{
			return ret;
		}
	}

	return ret;
}

static void extract(fs::path const& archive, fs::path const& directory)
{
	TempDir tempdir;
	extract_local(archive);
	copy_folder(analyze_extracted(), directory);
}

static inline int hex_char(unsigned int c)
{
	return "0123456789abcdef"[c & 0xF];
}

static std::string urlencode(std::string const& input)
{
	std::string result;
	for (char c : input)
	{
		if (('0' <= c && c <= '9')
		 || ('A' <= c && c <= 'Z')
		 || ('a' <= c && c <= 'z')
		 || (c == '~' || c == '-' || c == '_' || c == '.'))
		{
			result.push_back(c);
		}
		else
		{
			result.push_back('%');
			result.push_back(hex_char(c >> 4));
			result.push_back(hex_char(c));
		}
	}

	return result;
}

static bool check_checksum(FILE *file, const char *sum)
{
	const char *hash = std::strchr(sum, '=');
	if (!hash)
	{
		return false;
	}

	const EVP_MD *md = EVP_get_digestbyname(std::string(sum, hash).c_str());
	if (!md || strlen(++hash) != EVP_MD_size(md) * 2)
	{
		return false;
	}

	using context = std::unique_ptr<EVP_MD_CTX, void (*)(EVP_MD_CTX*)>;
	context ctx{EVP_MD_CTX_create(), EVP_MD_CTX_destroy};

	EVP_DigestInit_ex(ctx.get(), md, NULL);

	size_t size;
	char buffer[1024];
	while ((size = fread(buffer, sizeof(buffer), 1, file)) > 0)
	{
		EVP_DigestUpdate(ctx.get(), buffer, size);
	}

	unsigned int md_len;
	unsigned char digest[EVP_MAX_MD_SIZE];
	EVP_DigestFinal_ex(ctx.get(), digest, &md_len);

	for (int i = 0; i < md_len; ++i)
	{
		if (hex_char(digest[i] >> 4) != std::tolower(*hash++))
		{
			return false;
		}

		if (hex_char(digest[i]) != std::tolower(*hash++))
		{
			return false;
		}
	}

	return true;
}

Archive::Archive() :
		Driver("archive", "http://purplekarrot.net/2013/archive")
{
	this->curl = curl_easy_init();
	this->proxy_factory = px_proxy_factory_new();

	curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, std::fwrite);
	curl_easy_setopt(this->curl, CURLOPT_USERAGENT, "karrpt/0.1");
	curl_easy_setopt(this->curl, CURLOPT_FOLLOWLOCATION, 1);
}

Archive::~Archive()
{
	curl_easy_cleanup(curl);
	px_proxy_factory_free(proxy_factory);
}

void Archive::do_filter(KImplementation& impl, Add add) const
{
	char const* pack;
	char const* glob;

	pack = k_implementation_get_value(&impl, "sysname");
	glob = k_implementation_get_global(&impl, "sysname");
	if (std::strcmp(pack, "*") != 0 && std::strcmp(pack, glob) != 0)
	{
		return;
	}

	pack = k_implementation_get_value(&impl, "machine");
	glob = k_implementation_get_global(&impl, "machine");
	if (std::strcmp(pack, "*") != 0 && std::strcmp(pack, glob) != 0)
	{
		return;
	}

	add(impl, false);
}

void Archive::do_handle(KImplementation const& impl, bool requested) const
{
	char const *name = k_implementation_get_name(&impl);
	char const *href = k_implementation_get_value(&impl, "href");
	char const *checksum = k_implementation_get_value(&impl, "checksum");
	char const *cache = k_implementation_get_global(&impl, "pkgcache");

	fs::path archive = fs::path(cache) / urlencode(href);
	this->download(archive.c_str(), href, checksum);
	extract(archive, fs::path(name));
}

void Archive::download(const char* path, const char* url, const char* checksum) const
{
	FILE *file = std::fopen(path, "rb");
	if (file && check_checksum(file, checksum))
	{
		fclose(file);
		return;
	}

	file = std::fopen(path, "w+b");
	if (!file)
	{
		throw std::runtime_error("failed to open file");
	}

	CURLcode result = CURLE_UNSUPPORTED_PROTOCOL;

	curl_easy_setopt(this->curl, CURLOPT_URL, url);
	curl_easy_setopt(this->curl, CURLOPT_FILE, file);

	char **proxies = px_proxy_factory_get_proxies(this->proxy_factory, url);
	for (int i = 0; proxies[i]; ++i)
	{
		if (result != CURLE_OK)
		{
			curl_easy_setopt(this->curl, CURLOPT_PROXY, proxies[i]);
			result = curl_easy_perform(this->curl);
		}

		free(proxies[i]);
	}

	free(proxies);

	if (result != CURLE_OK)
	{
		throw std::runtime_error("failed to download file.");
	}

	rewind(file);

	if (check_checksum(file, checksum))
	{
		throw std::runtime_error("file checksum mismatch!");
	}
}

} // namespace Karrot
