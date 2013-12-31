/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_ENGINE_HPP
#define KARROT_ENGINE_HPP

#include <memory>
#include <vector>

namespace Karrot
{
class Module;

class Engine
{
public:
	Engine();
	~Engine();

	Engine(Engine const&) = delete;
	Engine& operator=(Engine const&) = delete;

  using AddModule = std::function<void(Module, bool)>;
  using Filter = std::function<void(Module, AddModule)>;
  void add_filter(std::string name, std::string xmlns, Filter filter);

	void add_request(std::string const& url, bool source);

	void load(std::string const& cache, bool force);
	bool solve();

	std::size_t num_modules() const;

	Module const& get_module(std::size_t index) const;

	std::vector<int> get_depends(std::size_t index) const;

	bool is_requested(std::size_t index) const;

private:
	struct Private;
	Private* self;
};

} // namespace Karrot

#endif /* KARROT_ENGINE_HPP */
