/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "query.hpp"
#include "string.hpp"

static const int LPAREN        = - 1;
static const int RPAREN        = - 2;
static const int LESS          = - 3;
static const int LESS_EQUAL    = - 4;
static const int GREATER       = - 5;
static const int GREATER_EQUAL = - 6;
static const int NOT_EQUAL     = - 7;
static const int EQUAL         = - 8;
static const int AND           = - 9;
static const int OR            = -10;

#include "query_re2c.hpp"
#include "quark.hpp"
#include "vercmp.hpp"
#include "variants.hpp"
#include <iostream>
#include <sstream>

namespace Karrot
{

static const int ASTERISK      = string_to_quark("*");
static const int VERSION       = string_to_quark("version");

static int op_preced(int op)
  {
  switch (op)
    {
    case LESS:
    case LESS_EQUAL:
    case GREATER:
    case GREATER_EQUAL:
      {
      return 4;
      }
    case EQUAL:
    case NOT_EQUAL:
      {
      return 3;
      }
    case AND:
      {
      return 2;
      }
    case OR:
      {
      return 1;
      }
    }
  return 0;
  }

static inline bool is_operator(int op)
  {
  return op <= LESS && op >= OR;
  }

static inline bool is_relation(int op)
  {
  return op <= LESS && op >= GREATER_EQUAL;
  }

static inline bool is_ident(int id)
  {
  return id >= 0;
  }

static inline int get_variant(const KDictionary& variants, int key)
  {
  auto it = variants.find(quark_to_string(key));
  if (it != variants.end())
    {
    return to_quark(it->second);
    }
  std::stringstream message;
  message
    << "Unknown variable '"
    << quark_to_string(key)
    << "' used in test. Known variables are: "
    ;
  for (auto& entry : variants)
    {
    message << "'" << entry.first << "', ";
    }
  message << "and 'version'.";
  throw std::runtime_error(message.str());
  }

Query::Query(const std::string& string)
  : string{string}
  {
  if (string.empty())
    {
    return;
    }
  std::string::const_iterator strpos = string.begin();
  std::string::const_iterator strend = string.end();

  // TODO: change to use std::vector/std::array
  int stack[32]; // operator stack
  unsigned int sl = 0; // stack length

  while (strpos != strend)
    {
    // read one token from the input stream
    int c = query_tokenize(strpos, strend);
    // If the token is a number (identifier), then add it to the output queue.
    if (is_ident(c))
      {
      queryspace.push_back(c);
      }
    // If the token is an operator, op1, then:
    else if (is_operator(c))
      {
      while (sl > 0)
        {
        int sc = stack[sl - 1];
        // While there is an operator token, op2, at the top of the stack
        // op1 is left-associative and its precedence is less than or equal to that of op2,
        // or op1 has precedence less than that of op2,
        // Let + and ^ be right associative.
        // Correct transformation from 1^2+3 is 12^3+
        // The differing operator priority decides pop / push
        // If 2 operators have equal priority then associativity decides.
        if (is_operator(sc) && op_preced(c) <= op_preced(sc))
          {
          // Pop op2 off the stack, onto the output queue;
          queryspace.push_back(sc);
          sl--;
          }
        else
          {
          break;
          }
        }
      // push op1 onto the stack.
      stack[sl++] = c;
      }
    // If the token is a left parenthesis, then push it onto the stack.
    else if (c == LPAREN)
      {
      stack[sl++] = c;
      }
    // If the token is a right parenthesis:
    else if (c == RPAREN)
      {
      int pe = false;
      // Until the token at the top of the stack is a left parenthesis,
      // pop operators off the stack onto the output queue
      while (sl > 0)
        {
        int sc = stack[sl - 1];
        if (sc == LPAREN)
          {
          pe = true;
          break;
          }
        else
          {
          queryspace.push_back(sc);
          sl--;
          }
        }
      // If the stack runs out without finding a left parenthesis, then there are mismatched parentheses.
      if (!pe)
        {
        throw std::runtime_error("Query: parentheses mismatched");
        }
      // Pop the left parenthesis from the stack, but not onto the output queue.
      sl--;
      }
    }
  // When there are no more tokens to read:
  // While there are still operator tokens in the stack:
  while (sl > 0)
    {
    int sc = stack[sl - 1];
    if (sc == LPAREN || sc == RPAREN)
      {
      throw std::runtime_error("Query: parentheses mismatched");
      }
    queryspace.push_back(sc);
    --sl;
    }
  }


bool Query::evaluate(const std::string& version, const KDictionary& variants) const
  {
  if (queryspace.empty())
    {
    return true;
    }

  // TODO: change to use std::vector/std::array
  unsigned int stack[32];
  unsigned int sl = 0; // stack length

  for(int c : queryspace)
    {
    if (is_ident(c))
      {
      stack[sl++] = c;
      }
    else if (is_operator(c))
      {
      if (sl < 2)
        {
        std::cout << "(Error) insufficient values in the expression." << std::endl;
        return false;
        }
      int op2 = stack[--sl];
      int op1 = stack[--sl];
      int diff, res;
      if (op1 == VERSION)
        {
        op1 = to_quark(version);
        }
      else if (op1 > 1) // 1 == true!
        {
        op1 = get_variant(variants, op1);
        }
      if (is_relation(c))
        {
        diff = vercmp(quark_to_string(op1), quark_to_string(op2));
        }
      switch (c)
        {
        case LESS:
          res = diff < 0;
          break;
        case LESS_EQUAL:
          res = diff <= 0;
          break;
        case GREATER:
          res = diff > 0;
          break;
        case GREATER_EQUAL:
          res = diff >= 0;
          break;
        case NOT_EQUAL:
          res = op1 != op2 && op1 != ASTERISK && op2 != ASTERISK;
          break;
        case EQUAL:
          res = op1 == op2 || op1 == ASTERISK || op2 == ASTERISK;
          break;
        case AND:
          res = op1 && op2;
          break;
        case OR:
          res = op1 || op2;
          break;
        }
      stack[sl++] = res;
      }
    }
  if (sl != 1)
    {
    std::cout << "ERROR: " << sl << " elements left on the stack!" << std::endl;
    return false;
    }
  return stack[0] != 0;
  }

} // namespace Karrot
