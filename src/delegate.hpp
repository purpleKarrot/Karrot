/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_DELEGATE_HPP
#define KARROT_DELEGATE_HPP

#include <cassert>

namespace Karrot
{

template<typename Ret, typename... Args>
class Delegate
  {
  public:
    typedef Ret (*Function)(Args... args, void *self);
    typedef void (*Destroy)(void*);
    typedef void *Target;
  public:
    Delegate(Function function, Target target, Destroy destroy)
        : function(function), target(target), destroy(destroy)
      {
      }
    ~Delegate()
      {
      if (destroy)
        {
        destroy(target);
        }
      }
    Ret operator()(Args... args) const
      {
      assert(function);
      /*return*/ function(args..., target);
      }
  private:
    Function function;
    Target target;
    Destroy destroy;
  };

} // namespace Karrot

#endif /* KARROT_DELEGATE_HPP */
