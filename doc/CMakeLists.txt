#=============================================================================
# Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
#
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#   http://www.boost.org/LICENSE_1_0.txt
#=============================================================================

find_package(Doxygen  REQUIRED)
find_package(XSLTPROC REQUIRED)

find_program(Quickbook_EXECUTABLE
  NAMES
    quickbook
  DOC
    "the quickbook tool"
  )

# Find the Boostbook DTD
find_path(BOOSTBOOK_DTD_DIR boostbook.dtd PATHS
  "/usr/share/boostbook/dtd/1.1"
  "/usr/share/boostbook/dtd"
  )
if(NOT BOOSTBOOK_DTD_DIR)
  message(WARNING "could not find Boostbook DTD!")
endif()

# Find the Boostbook XSL stylesheets
find_path(BOOSTBOOK_XSL_DIR html.xsl PATHS
  "/usr/share/boostbook/xsl"
  )
if(NOT BOOSTBOOK_XSL_DIR)
  message(WARNING "could not find Boostbook XSL stylesheets!")
endif()

# Find the DocBook DTD (version 4.2)
find_path(DOCBOOK_DTD_DIR docbookx.dtd PATHS
  "/opt/local/share/xml/docbook/4.2"
  "/usr/share/sgml/docbook/xml-dtd-4.2"
  "/usr/share/xml/docbook/schema/dtd/4.2"
  "/usr/share/xml/docbook/xml-dtd-4.2"
  )
if(NOT DOCBOOK_DTD_DIR)
  message(WARNING "could not find DocBook DTD!")
endif()

# Find the DocBook XSL stylesheets
find_path(DOCBOOK_XSL_DIR html/html.xsl PATHS
  "/opt/local/share/xsl/docbook-xsl"
  "/usr/share/sgml/docbook/xsl-stylesheets"
  "/usr/share/xml/docbook/*"
  "/usr/share/xml/docbook/stylesheet/nwalsh"
  )
if(NOT DOCBOOK_XSL_DIR)
  message(WARNING "could not find DocBook XSL stylesheets!")
endif()

set(catalog "${CMAKE_CURRENT_BINARY_DIR}/catalog.xml")
configure_file(catalog.xml.in "${catalog}")

set(doxyfile "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")
set(xml_dir  "${CMAKE_CURRENT_BINARY_DIR}/xml")

file(WRITE "${doxyfile}"
  "QUIET = YES\n"
  "WARN_IF_UNDOCUMENTED = NO\n"
  "GENERATE_LATEX = NO\n"
  "GENERATE_HTML = NO\n"
  "GENERATE_XML = YES\n"
  "XML_OUTPUT = \"${xml_dir}\"\n"
  "INPUT = \"${Karrot_SOURCE_DIR}/include/karrot.hpp\"\n"
  )

add_custom_command(
  OUTPUT
    "${xml_dir}/index.xml"
    "${xml_dir}/combine.xslt"
  COMMAND
    ${DOXYGEN_EXECUTABLE} ${doxyfile}
  DEPENDS
    "${Karrot_SOURCE_DIR}/include/karrot.hpp"
  )

# Collect Doxygen XML into a single XML file
xsltproc(
  INPUT      "${xml_dir}/index.xml"
  OUTPUT     "${xml_dir}/all.xml"
  STYLESHEET "${xml_dir}/combine.xslt"
  )

# Transform single Doxygen XML file into BoostBook XML
xsltproc(
  INPUT      "${xml_dir}/all.xml"
  OUTPUT     "${CMAKE_CURRENT_BINARY_DIR}/reference.xml"
  STYLESHEET "${BOOSTBOOK_XSL_DIR}/doxygen/doxygen2boostbook.xsl"
  PARAMETERS "boost.doxygen.header.prefix=karrot"
  )

add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/main.qbk"
  COMMAND
    ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/main.qbk"
    "${CMAKE_CURRENT_BINARY_DIR}/main.qbk"
  DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/main.qbk"
  )

add_custom_command(
  OUTPUT
    "${CMAKE_CURRENT_BINARY_DIR}/karrot.xml"
  COMMAND
    ${Quickbook_EXECUTABLE}
    --input-file   "${CMAKE_CURRENT_BINARY_DIR}/main.qbk"
    --include-path "${CMAKE_CURRENT_BINARY_DIR}"
    --include-path "${CMAKE_CURRENT_SOURCE_DIR}"
    --output-file  "${CMAKE_CURRENT_BINARY_DIR}/karrot.xml"
  DEPENDS
    "${CMAKE_CURRENT_BINARY_DIR}/main.qbk"
    "${CMAKE_CURRENT_BINARY_DIR}/reference.xml"
  )

xsltproc(
  INPUT      "${CMAKE_CURRENT_BINARY_DIR}/karrot.xml"
  OUTPUT     "${CMAKE_CURRENT_BINARY_DIR}/karrot.dbk"
  CATALOG    "${catalog}"
  STYLESHEET "${BOOSTBOOK_XSL_DIR}/docbook.xsl"
  DEPENDS    "${CMAKE_CURRENT_BINARY_DIR}/karrot.xml"
  )

xsltproc(
  INPUT      "${CMAKE_CURRENT_BINARY_DIR}/karrot.dbk"
  OUTPUT     "${CMAKE_CURRENT_BINARY_DIR}/html/index.html"
  CATALOG    "${catalog}"
  STYLESHEET "${BOOSTBOOK_XSL_DIR}/xhtml.xsl"
  )

add_custom_target(documentation
  DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/html/index.html"
  )
