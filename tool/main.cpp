/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <karrot.hpp>
#include <boost/program_options.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <iostream>
#include <fstream>

#ifdef USE_ARCHIVE
#  include "archive.hpp"
#endif

#ifdef USE_GIT
#  include "git.hpp"
#endif

#ifdef USE_PACKAGEKIT
#  include "packagekit.hpp"
#endif

#ifdef USE_SUBVERSION
#  include "subversion.hpp"
#endif

#include "cmake.hpp"

using namespace Karrot;

template<typename Type, typename... Args>
std::unique_ptr<Driver> make_driver(Args&&... args)
  {
  return std::unique_ptr<Driver>(new Type(std::forward<Args>(args)...));
  }

int main(int argc, char* argv[])
  {
  std::string machine;
  std::string sysname;
  std::vector<std::string> request_urls;
  try
    {
    namespace po = boost::program_options;
    po::options_description allowed_options("Allowed options");
    allowed_options.add_options()
      ("help,h", "produce help message")
      ("version,v", "print version string")
      ("sysname,s", po::value(&sysname), "the system name")
      ("machine,m", po::value(&machine), "the hardware name")
      ;
    po::options_description hidden_options("Hidden options");
    hidden_options.add_options()
      ("request-url", po::value(&request_urls), "request url")
      ;
    po::options_description all_options("All options");
    all_options
      .add(allowed_options)
      .add(hidden_options)
      ;
    po::positional_options_description positional_options;
    positional_options
      .add("request-url", -1)
      ;
    po::variables_map variables;
    store(po::command_line_parser(argc, argv)
      .options(all_options)
      .positional(positional_options)
      .run(), variables);
    if (variables.count("help"))
      {
      std::cout << allowed_options << std::endl;
      return 0;
      }
    if (variables.count("version"))
      {
      std::cout << "Karrot 0.1" << std::endl;
      return 0;
      }
    notify(variables);
    }
  catch (std::exception& error)
    {
    std::cout << error.what() << std::endl;
    return -1;
    }
  try
    {
    Engine engine;
    CMake::Listsfile listsfile;

#ifdef USE_ARCHIVE
    engine.add_driver("archive",
        make_driver<CMake::Injector>(listsfile,
            make_driver<Archive>()));
#endif

#ifdef USE_PACKAGEKIT
    PackageKit package_kit;
    engine.add_driver("packagekit",
        make_driver<PKDriver>(package_kit));
#endif

#ifdef USE_GIT
    engine.add_driver("git",
        make_driver<CMake::Injector>(listsfile,
            make_driver<Git>()));
#endif

#ifdef USE_SUBVERSION
    engine.add_driver("subversion",
        make_driver<CMake::Injector>(listsfile,
            make_driver<Subversion>()));
#endif

    for (const std::string& url : request_urls)
      {
      engine.add_request(url.c_str(), true);
      }
    if (!engine.run())
      {
      std::cout << "The request is not satisfiable!" << std::endl;
      return -1;
      }

#ifdef USE_PACKAGEKIT
    package_kit.install_queued();
#endif

    listsfile.write();
    }
  catch (...)
    {
    std::cerr
      << "\nUnhandled exception caught!\n"
      << boost::current_exception_diagnostic_information()
      ;
    return -1;
    }
  return 0;
  }
