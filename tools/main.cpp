/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <karrot.h>

#include "archive.hpp"
#include "git.hpp"
#include "subversion.hpp"

#include <boost/program_options.hpp>
#include <iostream>

#ifndef _WIN32
#  include <sys/utsname.h>
#endif

static void set_uname(std::string& sysname, std::string& machine)
{
#ifdef _WIN32
	sysname = "Windows";
	machine = getenv("PROCESSOR_ARCHITECTURE");
#else
	struct utsname uts;
	uname(&uts);
	sysname = uts.sysname;
	machine = uts.machine;
#endif
}

static void run(std::string& sysname, std::string& machine,
		std::vector<std::string> const& request_urls)
{
	using namespace Karrot;

	KPrint print = [](void *target, int level, char const *string)
	{
		std::cout << string << std::endl;
	};

	KEngine *engine = k_engine_new();

	k_engine_set_logger(engine, print, nullptr);

	k_engine_add_driver(engine,
			std::unique_ptr<Driver>(new Archive(machine, sysname)));
	k_engine_add_driver(engine,
			std::unique_ptr<Driver>(new Git));
	k_engine_add_driver(engine,
			std::unique_ptr<Driver>(new Subversion));

	for (const auto& url : request_urls)
	{
		k_engine_add_request(engine, url.c_str(), true);
	}

	int result = k_engine_run(engine);
	if (result != 0)
	{
		std::cerr << "Not solvable!" << std::endl;
	}

	k_engine_free(engine);
}

int main(int argc, char *argv[])
{
	std::string machine;
	std::string sysname;
	std::vector<std::string> request_urls;

	set_uname(sysname, machine);

	try
	{
		namespace po = boost::program_options;
		po::options_description program_options("Allowed options");
		program_options.add_options()
			("help", "produce help message")
			("version", "print version string")
			("sysname", po::value(&sysname)->value_name("SYSNAME"), "the system name")
			("machine", po::value(&machine)->value_name("MACHINE"), "the hardware name")
			;

		po::options_description hidden_options;
		hidden_options.add_options()
			("request-url", po::value(&request_urls))
			;

		po::options_description cmdline_options;
		cmdline_options
			.add(program_options)
			.add(hidden_options)
			;

		po::positional_options_description positional_options;
		positional_options
			.add("request-url", -1)
			;

		po::variables_map variables;
		store(po::command_line_parser(argc, argv)
			.options(cmdline_options)
			.positional(positional_options)
			.run(), variables);

		if (variables.count("help"))
		{
			std::cout << program_options << std::endl;
			return 0;
		}

		if (variables.count("version"))
		{
			std::cout << "Lemonade 0.1" << std::endl;
			return 0;
		}

		notify(variables);
	}
	catch (std::exception const& error)
	{
		std::cerr << error.what() << std::endl;
		return -1;
	}

	if (request_urls.empty())
	{
		std::cout << "No project URL given." << std::endl;
		return -1;
	}

	try
	{
		run(sysname, machine, request_urls);
	}
	catch (std::exception const& error)
	{
		std::cerr << error.what() << std::endl;
		return -1;
	}

	return 0;
}
