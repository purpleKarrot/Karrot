/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "karrot.hpp"
#include <boost/program_options.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <iostream>

template<typename Type, typename... Args>
std::unique_ptr<Driver> make_driver(Args&&... args)
  {
  return std::unique_ptr<Driver>(new Type(std::forward<Args>(args)...));
  }

class Archive: public Driver
  {
  public:
    Archive(std::string machine, std::string sysname)
        : machine(machine), sysname(sysname)
      {
      }
  private:
    const char* name() const //override
      {
      return "archive";
      }
    void fields(Fields& out) const //override
      {
      static const char* const fields_instance[] =
        {
        "sysname", "*",
        "machine", "*",
        "href", nullptr
        };
      out = fields_instance;
      }
    void filter(Dictionary const& fields, AddFun const& add) //override
      {
      std::string p_sysname = fields["sysname"];
      if (p_sysname != "*" && p_sysname != sysname)
        {
        return;
        }
      std::string p_machine = fields["machine"];
      if (p_machine != "*" && p_machine != machine)
        {
        return;
        }
      std::string href = fields["href"];
      std::string checksum = fields["checksum"];
      const char *values[] =
        {
        "href", href.c_str()
        };
      add(values, 4, false);
      }
    void download(const Implementation& impl, bool requested) //override
      {
      std::cout << "download " << impl.name() << std::endl;
      }
  private:
    std::string machine;
    std::string sysname;
  };

class Source: public Driver
  {
  public:
    Source(const char* name) : name_(name)
      {
      }
  private:
    char const* name() const
      {
      return name_;
      }
    void download(const Implementation& impl, bool requested) //override
      {
      std::cout << name_ << " download " << impl.name() << std::endl;
      }
  private:
    const char* name_;
  };

int main(int argc, char* argv[])
  {
  std::string dotfile;
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
      ("dotfile,d", po::value(&dotfile), "output graphviz dot file")
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
    Engine engine("http://purplekarrot.net/2013/");
    engine.add_driver(make_driver<Archive>(machine, sysname));
    engine.add_driver(make_driver<Source>("git"));
    engine.add_driver(make_driver<Source>("subversion"));
    for (const std::string& url : request_urls)
      {
      engine.add_request(url.c_str(), true);
      }
    if (!dotfile.empty())
      {
      engine.dot_filename(dotfile.c_str());
      }
    if (!engine.run())
      {
      std::cout << "The request is not satisfiable!" << std::endl;
      return -1;
      }
    }
  catch (std::exception const& error)
    {
    std::cerr << error.what() << std::endl;
    return -1;
    }
  return 0;
  }
