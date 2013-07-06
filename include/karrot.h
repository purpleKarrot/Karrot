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

#define KARROT_VER_MAJOR 1
#define KARROT_VER_MINOR 0
#define KARROT_VER_PATCH 0
#define KARROT_VERSION "1.0.0"

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

typedef struct KDictionary KDictionary;
typedef struct KImplementation KImplementation;
typedef struct KDriver KDriver;
typedef struct KEngine KEngine;

typedef void (*KAdd) (KDictionary const *dict, int native, void *self);
typedef void (*KPrintFun) (char const *string);
typedef void (*KVisit) (void *target, char const *key, char const *val);


KARROT_API char const *
k_version (int *major, int *minor, int *patch);


KARROT_API void
k_dictionary_set (KDictionary *self, char const *key, char const *value);

KARROT_API char const *
k_dictionary_get (KDictionary const *self, char const *key);

KARROT_API void
k_dictionary_foreach (KDictionary const *self, KVisit visit, void *target);


KARROT_API char const *
k_implementation_get_id (KImplementation const *self);

KARROT_API char const *
k_implementation_get_name (KImplementation const *self);

KARROT_API char const *
k_implementation_get_version (KImplementation const *self);

KARROT_API char const *
k_implementation_get_component (KImplementation const *self);

KARROT_API char const *
k_implementation_get_driver (KImplementation const *self);

KARROT_API KDictionary const *
k_implementation_get_meta (KImplementation const *self);

KARROT_API KDictionary const *
k_implementation_get_variant (KImplementation const *self);

KARROT_API KDictionary const *
k_implementation_get_values (KImplementation const *self);

KARROT_API KDictionary const *
k_implementation_get_globals (KImplementation const *self);


struct KDriver
  {
  int (*fields) (KDriver const *self, KDictionary *dict);
  int (*filter) (KDriver const *self, KDictionary const *fields, KAdd add, void *target);
  int (*depend) (KDriver const *self, KImplementation const *impl, KImplementation const *other);
  int (*handle) (KDriver const *self, KImplementation const *impl, int requested);
  int (*commit) (KDriver const *self);
  char const* (*get_error) (KDriver const *self);
  };


enum _KOption
  {
  K_OPT_LOG_FUNCTION            = (1u << 0),
  K_OPT_FEED_CACHE              = (1u << 2),
  K_OPT_RELOAD_FEEDS            = (1u << 3),
  K_OPT_IGNORE_SOURCE_CONFLICTS = (1u << 4),
  K_OPT_NO_TOPOLOGICAL_ORDER    = (1u << 5),
  };

typedef enum _KOption KOption;

/**
 * @defgroup Engine class
 * @ingroup Karrot
 * @{
 */

/**
 * Create a new Engine.
 *
 * @param namespace_uri The base url for the feed namespace.
 * @return a `KEngine` instance
 */
KARROT_API KEngine *
k_engine_new (char const *namespace_uri);

KARROT_API void
k_engine_set_global (KEngine *self, char const *key, char const *value);

/**
 * Add a Driver to an Engine.
 *
 * @param self a `KEngine` instance
 * @param driver a `KDriver` instance
 */
KARROT_API void
k_engine_add_driver (KEngine *self, char const *name, KDriver const *driver);

/**
 * Add a Request to an Engine.
 *
 * @param self a `KEngine` instance
 * @param url the url of the requested feed
 * @param source limit the request to source implementations
 */
KARROT_API void
k_engine_add_request (KEngine *self, char const *url, int source);

/**
 * Configure options of the Engine.
 *
 * @param self a `KEngine` instance
 * @param option the `KOption` to set
 * @param ... the value to be set
 */
KARROT_API void
k_engine_setopt (KEngine *self, KOption option, ...);

/**
 * Run the Engine.
 *
 * @param self a `KEngine` instance
 * @return zero indicates success
 */
KARROT_API int
k_engine_run (KEngine *self);

/**
 * Get the error message of the Engine.
 *
 * @param self a `KEngine` instance
 * @return the error message string
 */
KARROT_API char const *
k_engine_get_error (KEngine *self);

/**
 * Engine destructor
 *
 * @param self a `KEngine` instance
 */
KARROT_API void
k_engine_free (KEngine *self);

/** @} */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* KARROT_H_INCLUDED */
