/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_H_INCLUDED
#define KARROT_H_INCLUDED

#if defined(_WIN32)
#  define KARROT_IMPORT __declspec(dllimport)
#  define KARROT_EXPORT __declspec(dllexport)
#else
#  define KARROT_IMPORT __attribute__ ((visibility("default")))
#  define KARROT_EXPORT __attribute__ ((visibility("default")))
#endif

#if defined(KARROT_STATIC)
#  define KARROT_API
#elif defined(KARROT_BUILD)
#  define KARROT_API KARROT_EXPORT
#else
#  define KARROT_API KARROT_IMPORT
#endif

typedef struct _KDictionary KDictionary;
typedef struct _KImplementation KImplementation;
typedef struct _KDriver KDriver;
typedef struct _KEngine KEngine;

typedef void (*KAddFun) (char const **val, int size, int native, void *self);
typedef void (*KDownload) (KImplementation *impl, int requested, void *self);
typedef void (*KFilter) (KDictionary *fields, KAddFun fun, void *target, void *self);
typedef void (*KMapping) (char const *key, char const *val, void *self);

KARROT_API void
k_dict_foreach (KDictionary *self, KMapping mapping, void *target);

KARROT_API char const *
k_dict_lookup (KDictionary *self, char const *str);

KARROT_API char const *
k_impl_get_name (KImplementation *self);

KARROT_API char const *
k_impl_get_component (KImplementation *self);

KARROT_API char const *
k_impl_get_version (KImplementation *self);

KARROT_API KDictionary *
k_impl_get_variant (KImplementation *self);

KARROT_API KDictionary *
k_impl_get_values (KImplementation *self);

struct _KDriver
  {
  char const *namespace_uri;
  char const *const *fields;
  int fields_size;
  KDownload download;
  void *download_target;
  void (*download_target_destroy_notify) (void*);
  KFilter filter;
  void *filter_target;
  void (*filter_target_destroy_notify) (void*);
  };

KARROT_API KEngine *
k_engine_new (void);

KARROT_API void
k_engine_add_driver (KEngine *self, char const *name, KDriver *driver);

KARROT_API void
k_engine_add_request (KEngine *self, char const *url, int source);

KARROT_API int
k_engine_run (KEngine *self);

KARROT_API void
k_engine_free (KEngine *self);

#endif /* KARROT_H_INCLUDED */
