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

#define KARROT_VER_MAJOR 0
#define KARROT_VER_MINOR 1
#define KARROT_VER_PATCH 0
#define KARROT_VERSION "0.1.0"

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

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _KError KError;
typedef struct _KDictionary KDictionary;
typedef struct _KImplementation KImplementation;
typedef struct _KDriver KDriver;
typedef struct _KEngine KEngine;

typedef void (*KAddFun) (char const **val, int size, int native, void *self);
typedef void (*KDownload) (KImplementation const *impl, int requested, KError *error, void *self);
typedef void (*KFilter) (KDictionary const *fields, KAddFun fun, void *target, void *self);
typedef void (*KMapping) (char const *key, char const *val, void *self);

KARROT_API void
k_error_set (KError *self, char const *what);

KARROT_API void
k_dictionary_foreach (KDictionary const *self, KMapping mapping, void *target);

KARROT_API char const *
k_dictionary_lookup (KDictionary const *self, char const *str);

KARROT_API char const *
k_implementation_get_name (KImplementation const *self);

KARROT_API char const *
k_implementation_get_component (KImplementation const *self);

KARROT_API char const *
k_implementation_get_version (KImplementation const *self);

KARROT_API KDictionary const *
k_implementation_get_variant (KImplementation const *self);

KARROT_API KDictionary const *
k_implementation_get_values (KImplementation const *self);

struct _KDriver
  {
  char const *name;
  char const *const *fields;
  size_t fields_length1;
  KDownload download;
  void *download_target;
  void (*download_target_destroy_notify) (void*);
  KFilter filter;
  void *filter_target;
  void (*filter_target_destroy_notify) (void*);
  };

KARROT_API KEngine *
k_engine_new (char const *namespace_uri);

KARROT_API void
k_engine_add_driver (KEngine *self, KDriver *driver);

KARROT_API void
k_engine_add_request (KEngine *self, char const *url, int source);

KARROT_API void
k_engine_dot_filename (KEngine *self, char const *filename);

KARROT_API int
k_engine_run (KEngine *self);

KARROT_API char const *
k_engine_error_message (KEngine *self);

KARROT_API void
k_engine_free (KEngine *self);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* KARROT_H_INCLUDED */
