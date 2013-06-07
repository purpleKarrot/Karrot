/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_CLOSURES_HPP
#define KARROT_CLOSURES_HPP

#include <karrot.h>
#include <memory>

namespace Karrot
{
namespace detail
{

template<typename Ret, typename... Args>
class ClosureBase
  {
  private:
    using Destroy = void (*)(void*);
    using Context = std::unique_ptr<void, Destroy>;
    using Function = Ret(*)(void*, Args...);
  public:
    ClosureBase() = default;
    ClosureBase(Function function, void* context, Destroy destroy)
      : function{function}, context{context, destroy}
      {
      }
  protected:
    Function function{nullptr};
    Context context{nullptr, [](void*){}};
  };

template<typename Function>
class Closure;

template<typename... Args>
class Closure<void(*)(void*, Args...)>: ClosureBase<void, Args...>
  {
  public:
    using ClosureBase<void, Args...>::ClosureBase;
  public:
    explicit operator bool() const
      {
      return this->function;
      }
  protected:
    void call(Args... args) const
      {
      this->function(this->context.get(), args...);
      }
  };

template<typename Ret, typename... Args>
class Closure<Ret(*)(void*, Args...)>: ClosureBase<Ret, Args...>
  {
  public:
    using ClosureBase<Ret, Args...>::ClosureBase;
  public:
    explicit operator bool() const
      {
      return this->function;
      }
  protected:
    Ret call(Args... args) const
      {
      return this->function(this->context.get(), args...);
      }
  };

} // namespace detail

class Fields: public detail::Closure<KFields>
  {
  public:
    using detail::Closure<KFields>::Closure;
  public:
    bool operator()(std::string const& driver, KDictionary& fields) const
      {
      return this->call(driver.c_str(), &fields) == 0;
      }
  };

class Filter: public detail::Closure<KFilter>
  {
  public:
    using detail::Closure<KFilter>::Closure;
  public:
    template<typename Add>
    bool operator()(std::string const& driver, KDictionary const& fields, Add add) const
      {
      KAdd add_fun = [](void *target, KDictionary const* values, int native) -> int
        {
        try
          {
          Add const& add = *reinterpret_cast<Add*>(target);
          add(*values, native != 0);
          return 0;
          }
        catch (...)
          {
          return -1;
          }
        };
      return this->call(driver.c_str(), &fields, add_fun, &add) == 0;
      }
  };

class Handle: public detail::Closure<KHandle>
  {
  public:
    using detail::Closure<KHandle>::Closure;
  public:
    bool operator()(KImplementation const& impl, bool requested) const
      {
      return this->call(&impl, requested ? 1 : 0) == 0;
      }
  };

class Depend: public detail::Closure<KDepend>
  {
  public:
    using detail::Closure<KDepend>::Closure;
  public:
    bool operator()(KImplementation const& impl, KImplementation const& other) const
      {
      return this->call(&impl, &other) == 0;
      }
  };

} // namespace Karrot

#endif /* KARROT_CLOSURES_HPP */
