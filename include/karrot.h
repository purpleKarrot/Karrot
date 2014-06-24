/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_H_INCLUDED
#define KARROT_H_INCLUDED

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct KImplementation KImplementation;
typedef struct KDriver KDriver;

typedef void (*KAdd) (KImplementation *impl, int native, void *self);
typedef void (*KVisit) (void *target, char const *key, char const *val);


char const *
k_implementation_get_id (KImplementation const *self);

char const *
k_implementation_get_name (KImplementation const *self);

char const *
k_implementation_get_version (KImplementation const *self);

void
k_implementation_set_version (KImplementation *self, char const *value);

char const *
k_implementation_get_component (KImplementation const *self);

void
k_implementation_set_component (KImplementation *self, char const *value);

char const *
k_implementation_get_driver (KImplementation const *self);

char const *
k_implementation_get_meta (KImplementation const *self, char const *key);

char const *
k_implementation_get_variant (KImplementation const *self, char const *key);

void
k_implementation_foreach_variant (KImplementation const *self, KVisit visit, void *target);

char const *
k_implementation_get_value (KImplementation const *self, char const *key);

void
k_implementation_set_value (KImplementation *self, char const *key, char const *value);

char const *
k_implementation_get_global (KImplementation const *self, char const *key);


struct KDriver
  {
  int (*filter) (KDriver const *self, KImplementation *impl, KAdd add, void *target);
  int (*depend) (KDriver const *self, KImplementation const *impl, KImplementation const *other);
  int (*handle) (KDriver const *self, KImplementation const *impl, int requested);
  int (*commit) (KDriver const *self);
  char const* (*get_error) (KDriver const *self);
  };


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* KARROT_H_INCLUDED */
