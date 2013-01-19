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

using namespace karrot;

int main(int argc, char* argv[])
  {
  try
    {
    Engine engine;

#ifdef USE_ARCHIVE
    Archive archive;
    engine.add_driver("archive", &archive);
#endif

#ifdef USE_PACKAGEKIT
    PackageKit package_kit;
    engine.add_driver("packagekit", &package_kit);
#endif

#ifdef USE_GIT
    Git git;
    engine.add_driver("git", &git);
#endif

#ifdef USE_SUBVERSION
    Subversion subversion;
    engine.add_driver("svn", &subversion);
#endif

    for (int i = 1; i < argc; ++i)
      {
      engine.add_request(argv[i]);
      }
    engine.run();

#ifdef USE_PACKAGEKIT
    package_kit.install();
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
