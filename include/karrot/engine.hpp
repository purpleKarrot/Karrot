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
class Driver;
class Implementation;

class Engine
{
public:
	Engine();
	~Engine();

	Engine(Engine const&) = delete;
	Engine& operator=(Engine const&) = delete;

	void add_driver(std::unique_ptr<Driver> driver);
	Driver* get_driver(std::string const& name) const;

	void add_request(char const *url, int source);

	void load(std::string const& cache, bool force);
	bool solve();

	std::size_t num_modules() const;

	Implementation const& get_module(std::size_t index) const;

	std::vector<int> get_depends(Implementation const& module) const;

	bool is_requested(Implementation const& module) const;

private:
	struct Private;
	Private* self;
};

} // namespace Karrot

#endif /* KARROT_ENGINE_HPP */
