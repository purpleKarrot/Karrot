/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "graph.hpp"
#include <karrot/solve.hpp>
#include <karrot/spec.hpp>
#include <karrot/implementation.hpp>

#include <fstream>
#include <boost/graph/vector_as_graph.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/function_output_iterator.hpp>

namespace Karrot
{

std::vector<int>
topological_sort(std::vector<int> const& model, Database const& database, StringPool const& pool)
  {
  std::vector<int> topo_order;
  std::size_t size = model.size();
  std::vector<std::vector<int>> graph(size);
  for (std::size_t i = 0; i < size; i++)
    {
    auto& impl = database[model[i]];
    for (std::size_t k = 0; k < size; k++)
      {
      for (auto& dep : database[model[k]].depends)
        {
        if (dep.satisfies(impl, pool))
          {
          graph[i].push_back(k);
          }
        }
      }
    }
  auto insert = [&topo_order, &model](int idx)
    {
    topo_order.push_back(model[idx]);
    };
  boost::topological_sort(graph,
    boost::make_function_output_iterator(insert),
    boost::vertex_index_map(boost::identity_property_map()));
  std::reverse(std::begin(topo_order), std::end(topo_order));
  return topo_order;
  }

void write_graphviz(
    std::string const& filename,
    std::vector<int> const& model,
    Karrot::Database const& database, StringPool const& pool)
  {
  std::ofstream dot_file(filename);
  dot_file << "digraph G {\n";
  for (std::size_t i = 0; i < model.size(); ++i)
    {
    auto& entry = database[model[i]];
    dot_file << "  " << i << " ["
      << "label=\"" << entry.id << ' ' << entry.version << "\", "
      << "URL=\"" << entry.id << "\""
      << "];" << std::endl;
      ;
    for (std::size_t k = 0; k < model.size(); ++k)
      {
      for (auto& spec : database[model[k]].depends)
        {
        if (spec.satisfies(entry, pool))
          {
          dot_file << "  " << k << " -> " << i << ";\n";
          }
        }
      }
    }
  dot_file << "}\n";
  }

} // namespace Karrot
