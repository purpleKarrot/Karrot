/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_DRIVER_HPP
#define KARROT_DRIVER_HPP

#include <string>
#include <functional>

struct KImplementation;

namespace Karrot
{

class Driver
{
protected:
	Driver(std::string name, std::string xmlns) :
			name_(std::move(name)), xmlns_(std::move(xmlns))
	{
	}

	Driver(Driver const&) = delete;
	Driver& operator=(Driver const&) = delete;

public:
	virtual ~Driver()
	{
	}

	std::string const& name() const
	{
		return name_;
	}

	std::string const& xmlns() const
	{
		return xmlns_;
	}

	using Add = std::function<void(KImplementation&, bool)>;
	void filter(KImplementation& impl, Add add) const
	{
		do_filter(impl, add);
	}

	void handle(KImplementation const& impl, bool requested) const
	{
		do_handle(impl, requested);
	}

	void depend(KImplementation const& impl, KImplementation const& other) const
	{
		do_depend(impl, other);
	}

private:
	virtual void do_filter(KImplementation& impl, Add add) const
	{
	}

	virtual void do_handle(KImplementation const& impl, bool requested) const = 0;

	virtual void do_depend(KImplementation const& impl, KImplementation const& other) const
	{
	}

private:
	std::string name_;
	std::string xmlns_;
};

} // namespace Karrot

#endif /* KARROT_DRIVER_HPP */
