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
#include <memory>
#include <karrot/driver.hpp>

#define KARROT_VER_MAJOR 1
#define KARROT_VER_MINOR 0
#define KARROT_VER_PATCH 0
#define KARROT_VERSION "1.0.0"

typedef struct KImplementation KImplementation;
typedef struct KEngine KEngine;

typedef void (*KPrint) (void *target, int level, char const *string);
typedef void (*KVisit) (void *target, char const *key, char const *val);


char const *
k_version (int *major, int *minor, int *patch);


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


/**
 * @defgroup Engine class
 * @ingroup Karrot
 * @{
 */

/**
 * Create a new Engine.
 *
 * @return a `KEngine` instance
 */
KEngine *
k_engine_new (void);

void
k_engine_set_logger (KEngine *self, KPrint print, void *target);

/**
 * Add a Driver to an Engine.
 *
 * @param self a `KEngine` instance
 * @param driver a `KDriver` instance
 */
void
k_engine_add_driver (KEngine *self, std::unique_ptr<Karrot::Driver> driver);

/**
 * Add a Request to an Engine.
 *
 * @param self a `KEngine` instance
 * @param url the url of the requested feed
 * @param source limit the request to source implementations
 */
void
k_engine_add_request (KEngine *self, char const *url, int source);

/**
 * Run the Engine.
 *
 * @param self a `KEngine` instance
 * @return zero indicates success
 */
int
k_engine_run (KEngine *self);

/**
 * Engine destructor
 *
 * @param self a `KEngine` instance
 */
void
k_engine_free (KEngine *self);

/** @} */

#endif /* KARROT_H_INCLUDED */
