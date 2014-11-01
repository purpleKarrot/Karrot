/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "engine.hpp"
#include "url.hpp"
#include "cache.hpp"
#include "graph.hpp"
#include <karrot.h>
#include "feed_queue.hpp"
#include "feed_parser.hpp"
#include "xml_reader.hpp"
#include "version.h"

#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <iostream>

#ifndef _WIN32
#  include <sys/utsname.h>
#endif

#define S(X) engine.string_pool.from_static_string(#X)
#define s(X) engine.string_pool.from_string(X)

static void set_uname(std::string& sysname, std::string& machine)
  {
#ifdef _WIN32
  sysname = "Windows";
  machine = getenv("PROCESSOR_ARCHITECTURE");
#else
  utsname uts;
  uname(&uts);
  sysname = uts.sysname;
  machine = uts.machine;
#endif
  }

static void run(std::string& sysname, std::string& machine,
    std::vector<std::string> const& request_urls)
{
  using namespace Karrot;
  Karrot::Engine engine;

  engine.globals.emplace(S(sysname), s(sysname.c_str()));
  engine.globals.emplace(S(machine), s(machine.c_str()));

  // k_engine_add_driver(engine,
  //     std::unique_ptr<Karrot::Driver>(new Karrot::Archive));
  // k_engine_add_driver(engine,
  //     std::unique_ptr<Karrot::Driver>(new Karrot::Git));
  // k_engine_add_driver(engine,
  //     std::unique_ptr<Karrot::Driver>(new Karrot::Subversion));

  for (const auto& url : request_urls)
    {
    Karrot::Spec spec(url.c_str(), engine.string_pool);
    spec.component = STR_SOURCE;
    engine.feed_queue.push(spec.id);
    engine.requests.push_back(spec);
    }

  while (auto id = engine.feed_queue.get_next())
    {
    std::string url = engine.string_pool.to_string(*id);
    std::clog << boost::format("Reading feed '%1%'\n") % url;
    XmlReader xml(download(url));
    if (!xml.start_element())
      {
      throw std::runtime_error("failed to read feed" + url);
      }
    FeedParser parser{*id, engine};
    try
      {
      parser.parse(xml);
      }
    catch (XmlParseError& error)
      {
      error.filename = url;
      throw;
      }
    }

  std::clog << boost::format("Solving SAT with %1% variables\n") % engine.database.size();
  bool solvable = engine.solve(engine.requests, engine.string_pool);
  if (!solvable)
    {
    std::cerr << "Not solvable!\n";
    return;
    }

  if (!engine.no_topological_order)
    {
    engine.model = topological_sort(engine.model, engine.database, engine.string_pool);
    }

  for (int i : engine.model)
    {
    const Implementation& impl = engine.database[i];
    std::clog << boost::format("Handling '%1% %2%'\n") % impl.id % impl.version;
    bool requested = std::any_of(engine.requests.begin(), engine.requests.end(),
      [&impl, &engine](const Spec& spec)
      {
      return spec.satisfies(impl, engine.string_pool);
      });
    int STR_DRIVER = engine.string_pool.from_static_string("driver");
    auto driver = engine.package_handler.get(engine.string_pool.to_string(impl.get(STR_DRIVER)));
    driver->handle(impl, requested);
    }

  write_cache("cache.yaml", engine.model, engine.database, engine.string_pool);
  write_graphviz("dependencies.dot", engine.model, engine.database, engine.string_pool);
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
      std::cout << "Karrot " KARROT_VERSION << std::endl;
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
