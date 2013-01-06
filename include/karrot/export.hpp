/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_EXPORT_HPP
#define KARROT_EXPORT_HPP

#include <boost/config.hpp>

#if defined(KARROT_DYN_LINK)
# if defined(KARROT_SOURCE)
#   define KARROT_DECL BOOST_SYMBOL_EXPORT
# else
#   define KARROT_DECL BOOST_SYMBOL_IMPORT
# endif
#else
# define KARROT_DECL
#endif

#endif /* KARROT_EXPORT_HPP */
