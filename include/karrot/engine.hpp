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

typedef void (*KPrint) (void *target, int level, char const *string);

namespace Karrot
{
class Driver;

class Engine
{
public:
	Engine();
	~Engine();

	Engine(Engine const&) = delete;
	Engine& operator=(Engine const&) = delete;

	void set_logger(KPrint print, void *target);

	void add_driver(std::unique_ptr<Karrot::Driver> driver);

	void add_request(char const *url, int source);

	bool run();

private:
	struct Implementation;
	Implementation* self;
};

} // namespace Karrot

#endif /* KARROT_ENGINE_HPP */
