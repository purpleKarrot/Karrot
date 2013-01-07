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
#include <boost/filesystem/fstream.hpp>

#include <karrot/deliverable.hpp>
#include <karrot/quark.hpp>
#include <karrot/url.hpp>

#include <iostream>
#include <string>
#include <cctype>

extern "C"
{
#include "md5.h"
#include <curl/curl.h>
}

#ifndef _WIN32
#  include <sys/utsname.h>
#endif

namespace karrot
{
namespace fs = boost::filesystem;

static const int ASTERISK = string_to_quark("*");
static const int CHECKSUM = string_to_quark("checksum");
static const int HREF     = string_to_quark("href");
static const int MACHINE  = string_to_quark("machine");
static const int SYSNAME  = string_to_quark("sysname");

Archive::Archive()
  {
#ifdef _WIN32
  char* env = getenv("PROCESSOR_ARCHITECTURE");
  machine = string_to_quark(env, std::strlen(env));
  sysname = string_to_quark("Windows");
#else
  struct utsname uts;
  uname(&uts);
  machine = string_to_quark(uts.machine, std::strlen(uts.machine));
  sysname = string_to_quark(uts.sysname, std::strlen(uts.sysname));
#endif
  }

int Archive::namespace_uri() const
  {
  static const int instance = string_to_quark("http://karrot.org/2012/archive");
  return instance;
  }

Driver::Fields Archive::fields() const
  {
  Fields fields;
  fields.insert(std::make_pair(SYSNAME, ASTERISK));
  fields.insert(std::make_pair(MACHINE, ASTERISK));
  fields.insert(std::make_pair(HREF, 0));
  fields.insert(std::make_pair(CHECKSUM, 0));
  return fields;
  }

void r_url_to_string (const Url& url, char *str)
  {
  strcpy(str, quark_to_string(url.scheme));
  strcat(str, "://");
  if (url.user_info)
    {
    strcat(str, quark_to_string(url.user_info));
    strcat(str, "@");
    }
  strcat(str, quark_to_string(url.host));
  if (url.port)
    {
    strcat(str, ":");
    strcat(str, quark_to_string(url.port));
    }
  strcat(str, quark_to_string(url.path));
  }

static int progress(void *clientp, double total, double now, double t, double n)
  {
  int x;
  int w = 60;
  double ratio = (now == 0) ? 0 : now / total;
  int c = (int)(ratio * w);
  printf("                                                                 \r");
  printf("%3d%% [", (int) (ratio * 100));
  for (x = 0; x < c; x++)
    {
    printf("=");
    }
  for (x = c; x < w; x++)
    {
    printf(" ");
    }
  printf("]\r");
  fflush(stdout);
  return 0;
  }

int r_download_(FILE *file, const char *url)
  {
  CURL *curl_handle= curl_easy_init();
  curl_easy_setopt(curl_handle, CURLOPT_URL, url);
  curl_easy_setopt(curl_handle, CURLOPT_FILE, file);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, fwrite); // NEEDED when linking debug against release
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "karrot/0.1");
  curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1);
  curl_easy_setopt(curl_handle, CURLOPT_PROGRESSFUNCTION, progress);
  curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 0);
  CURLcode res = curl_easy_perform(curl_handle);
  if (res != CURLE_OK)
    {
    fprintf(stderr, "curl failed: %s\n", curl_easy_strerror(res));
    }
  curl_easy_cleanup(curl_handle);
  return res;
  }

static std::string char2hex(char dec)
  {
  char dig1 = (dec & 0xF0) >> 4;
  char dig2 = (dec & 0x0F);
  if (0 <= dig1 && dig1 <= 9)
    {
    dig1 += 48;
    }
  if (10 <= dig1 && dig1 <= 15)
    {
    dig1 += 65 - 10;
    }
  if (0 <= dig2 && dig2 <= 9)
    {
    dig2 += 48;
    }
  if (10 <= dig2 && dig2 <= 15)
    {
    dig2 += 65 - 10;
    }
  std::string r;
  r.append(&dig1, 1);
  r.append(&dig2, 1);
  return r;
  }

static std::string urlencode(const std::string& c)
  {
  std::string escaped;
  int max = c.length();
  for (int i = 0; i < max; i++)
    {
    if ((48 <= c[i] && c[i] <= 57)
    || (65 <= c[i] && c[i] <= 90)
    || (97 <= c[i] && c[i] <= 122)
    || (c[i] == '~' || c[i] == '-' || c[i] == '_' || c[i] == '.'))
      {
      escaped.append(&c[i], 1);
      }
    else
      {
      escaped.append("%");
      escaped.append(char2hex(c[i]));
      }
    }
  return escaped;
  }

struct ArchiveClose
  {
  public:
    ArchiveClose(archive* self, int (*func)(archive*)) :
        self(self), func(func)
      {
      }
    ~ArchiveClose()
      {
      func(self);
      }
  private:
    archive* self;
    int (*func)(archive*);
  };

struct ArchiveError: std::runtime_error
  {
  ArchiveError(archive* ar) :
      std::runtime_error(archive_error_string(ar))
    {
    }
  };

static int copy_data(struct archive *ar, struct archive *aw)
  {
  int ret;
  const void *buff;
  size_t size;
  __LA_INT64_T offset;
  for (;;)
    {
    ret = archive_read_data_block(ar, &buff, &size, &offset);
    if (ret == ARCHIVE_EOF)
      {
      return ARCHIVE_OK;
      }
    if (ret != ARCHIVE_OK)
      {
      return ret;
      }
    ret = archive_write_data_block(aw, buff, size, offset);
    if (ret != ARCHIVE_OK)
      {
      fprintf(stderr, "%s\n", archive_error_string(aw));
      return ret;
      }
    }
  }

static int extract_local(const fs::path& archive_path)
  {
  int ret;
  int flags;
  struct archive *a, *ext;
  struct archive_entry *entry;

  /* Select which attributes we want to restore. */
  flags = ARCHIVE_EXTRACT_TIME;
  flags |= ARCHIVE_EXTRACT_PERM;
  flags |= ARCHIVE_EXTRACT_ACL;
  flags |= ARCHIVE_EXTRACT_FFLAGS;

  a = archive_read_new();
  ArchiveClose close_read(a, archive_read_free);
  archive_read_support_format_all(a);
  archive_read_support_compression_all(a);

  ext = archive_write_disk_new();
  ArchiveClose close_write(ext, archive_write_free);
  archive_write_disk_set_options(ext, flags);
  archive_write_disk_set_standard_lookup(ext);

#ifdef BOOST_WINDOWS
  if (ret = archive_read_open_filename_w(a, archive_path.c_str(), 10240))
#else
  if (ret = archive_read_open_filename(a, archive_path.c_str(), 10240))
#endif
    {
    return ret;
    }
  for (;;)
    {
    ret = archive_read_next_header(a, &entry);
    if (ret == ARCHIVE_EOF)
      {
      ret = ARCHIVE_OK;
      return ret;
      }
    if (ret != ARCHIVE_OK)
      {
      fprintf(stderr, "%s\n", archive_error_string(a));
      }
    if (ret < ARCHIVE_WARN)
      {
      return ret;
      }
    ret = archive_write_header(ext, entry);
    if (ret != ARCHIVE_OK)
      {
      fprintf(stderr, "%s\n", archive_error_string(ext));
      }
    else if (archive_entry_size(entry) > 0)
      {
      copy_data(a, ext);
      if (ret != ARCHIVE_OK)
        {
        fprintf(stderr, "%s\n", archive_error_string(ext));
        }
      if (ret < ARCHIVE_WARN)
        {
        return ret;
        }
      }
    ret = archive_write_finish_entry(ext);
    if (ret != ARCHIVE_OK)
      {
      fprintf(stderr, "%s\n", archive_error_string(ext));
      }
    if (ret < ARCHIVE_WARN)
      {
      return ret;
      }
    }
  return ret;
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

static fs::path analyze_extracted(fs::path root)
  {
  int found = false;
  fs::path nested(root);
  for (fs::directory_iterator it(root), end; it != end; ++it)
    {
    if (found || !is_directory(*it))
      {
      return root;
      }
    nested = *it;
    found = true;
    }
  return nested;
  }

int Archive::filter(const Fields& fields, Identification& id, int& href, int& hash)
  {
  int p_sysname = fields.find(SYSNAME)->second;
  if (p_sysname != ASTERISK && p_sysname != sysname)
    {
    return INCOMPATIBLE;
    }
  int p_machine = fields.find(MACHINE)->second;
  if (p_machine != ASTERISK && p_machine != machine)
    {
    return INCOMPATIBLE;
    }
  href = fields.find(HREF)->second;
  hash = fields.find(CHECKSUM)->second;
  return NORMAL;
  }

static bool check_md5(FILE *file, const char *md5)
  {
  MD5_CTX md5_ctx;
  static const char hex[] = "0123456789abcdef";
  unsigned char buffer[1024];
  std::size_t bytes;
  MD5_Init(&md5_ctx);
  while ((bytes = std::fread(buffer, 1, sizeof(buffer), file)) != 0)
    {
    MD5_Update(&md5_ctx, buffer, bytes);
    }
  MD5_Final(buffer, &md5_ctx);
  for (int i = 0; i < 16; ++i)
    {
    if (hex[(buffer[i] >> 4) & 0xF] != std::tolower(*md5++))
      {
      return false;
      }
    if (hex[buffer[i] & 0xF] != std::tolower(*md5++))
      {
      return false;
      }
    }
  return true;
  }

void Archive::download(const Deliverable& artefact)
  {
  const char* url = quark_to_string(artefact.href);
  const char* md5 = quark_to_string(artefact.hash);
  fs::path filepath = fs::path(".archives") / urlencode(url);
  FILE* file = std::fopen(filepath.string().c_str(), "rb");
  if (file)
    {
    if (check_md5(file, md5))
      {
      std::fclose(file);
      return;
      }
    std::fclose(file);
    }
  file = std::fopen(filepath.string().c_str(), "wb");
  r_download_(file, quark_to_string(artefact.href));
  std::fclose(file);

  std::string dir_name = quark_to_string(artefact.folder);

  fs::path current_path = fs::current_path();
  fs::path output_path = current_path / dir_name;
  fs::path temp_path = current_path / (dir_name + "-tmp");
  output_path.replace_extension();

  create_directory(temp_path);
  fs::current_path(temp_path);

  extract_local(filepath);
  fs::path wanted = analyze_extracted(temp_path);
  copy_folder(wanted, output_path);

  fs::current_path(current_path);
  remove_all(temp_path);
  }

} // namespace karrot