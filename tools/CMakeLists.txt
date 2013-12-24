#=============================================================================
# Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
#
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#   http://www.boost.org/LICENSE_1_0.txt
#=============================================================================

find_package(Boost "1.46" REQUIRED filesystem program_options system)
find_package(CURL REQUIRED)
find_package(LibArchive REQUIRED)
find_package(libproxy REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(LibGit2 REQUIRED)
find_package(APR       REQUIRED)
find_package(SVN "1.6" REQUIRED COMPONENTS client delta diff fs subr wc)

include_directories(
  ${Boost_INCLUDE_DIRS}
  ${CURL_INCLUDE_DIRS}
  ${OPENSSL_INCLUDE_DIR}
  ${LibGit2_INCLUDE_DIRS}
  ${APR_INCLUDE_DIRS}
  ${SVN_INCLUDE_DIRS}
  )

add_executable(karrot_tool
  archive.hpp
  archive.cpp
  git.hpp
  git.cpp
  subversion.hpp
  subversion.cpp
  karrot.rc
  main.cpp
  )

target_link_libraries(karrot_tool
  ${Boost_LIBRARIES}
  ${CURL_LIBRARIES}
  ${OPENSSL_LIBRARIES}
  ${LibArchive_LIBRARIES}
  ${LIBPROXY_LIBRARIES}
  ${LibGit2_LIBRARY}
  ${APR_LIBRARIES}
  ${SVN_LIBRARIES}
  karrot
  )
