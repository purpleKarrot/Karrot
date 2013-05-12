/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <karrot.h>

char const *
k_version(int *major, int *minor, int *patch)
  {
  if (major)
    {
    *major = KARROT_VER_MAJOR;
    }
  if (minor)
    {
    *minor = KARROT_VER_MINOR;
    }
  if (patch)
    {
    *patch = KARROT_VER_PATCH;
    }
  return KARROT_VERSION;
  }
