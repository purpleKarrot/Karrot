/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <karrot.hpp>

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

#include <boost/exception/diagnostic_information.hpp>
#include <iostream>
#include <fstream>

using namespace Karrot;

template<typename Type, typename... Args>
std::unique_ptr<Driver> make_driver(Args&&... args)
  {
  return std::unique_ptr<Driver>(new Type(std::forward<Args>(args)...));
  }

int main(int argc, char* argv[])
  {
  try
    {
    Engine engine;

#ifdef USE_ARCHIVE
    engine.add_driver("archive", make_driver<Archive>());
#endif

#ifdef USE_PACKAGEKIT
    PackageKit package_kit;
    engine.add_driver("packagekit", make_driver<PKDriver>(package_kit));
#endif

#ifdef USE_GIT
    engine.add_driver("git", make_driver<Git>());
#endif

#ifdef USE_SUBVERSION
    engine.add_driver("svn", make_driver<Subversion>());
#endif

    for (int i = 1; i < argc; ++i)
      {
      engine.add_request(argv[i], true);
      }
    engine.run();

#ifdef USE_PACKAGEKIT
    package_kit.install_queued();
#endif
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
