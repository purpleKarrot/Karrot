/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <karrot/query.hpp>

#include "query_re2c.hpp"
#include "vercmp.hpp"
#include <iostream>
#include <sstream>
#include <stack>

namespace Karrot
{

static int op_preced(int op)
  {
  switch (op)
    {
    case STR_LESS:
    case STR_LESS_EQUAL:
    case STR_GREATER:
    case STR_GREATER_EQUAL:
      {
      return 4;
      }
    case STR_EQUAL:
    case STR_NOT_EQUAL:
      {
      return 3;
      }
    case STR_AND:
      {
      return 2;
      }
    case STR_OR:
      {
      return 1;
      }
    default:
      {
      return 0;
      }
    }
  }

static inline bool is_operator(int id)
  {
  return id >= STR_LESS && id <= STR_OR;
  }

static inline bool is_relation(int id)
  {
  return id >= STR_LESS && id <= STR_GREATER_EQUAL;
  }

static inline bool is_ident(int id)
  {
  return id > STR_OR;
  }

static inline int
get_variant(const Dictionary& variants, int key, StringPool const& pool)
  {
  auto it = variants.find(key);
  if (it != variants.end())
    {
    return it->second;
    }
  std::stringstream message;
  message
    << "Unknown variable '"
    << pool.to_string(key)
    << "' used in test. Known variables are: "
    ;
  for (auto& entry : variants)
    {
    message << "'" << pool.to_string(entry.first) << "', ";
    }
  message << "and 'version'.";
  throw std::runtime_error(message.str());
  }

Query::Query(std::string const& str, StringPool& pool)
  {
  if (str.empty())
    {
    return;
    }
  auto strpos = str.cbegin();
  auto strend = str.cend();

  // TODO: change to use std::vector/std::array
  int stack[32]; // operator stack
  unsigned int sl = 0; // stack length

  while (strpos != strend)
    {
    // read one token from the input stream
    int c = query_tokenize(strpos, strend, pool);
    // If the token is a number (identifier), then add it to the output queue.
    if (is_ident(c))
      {
      compiled.push_back(c);
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
          compiled.emplace_back(sc);
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
    else if (c == STR_LPAREN)
      {
      stack[sl++] = c;
      }
    // If the token is a right parenthesis:
    else if (c == STR_RPAREN)
      {
      int pe = false;
      // Until the token at the top of the stack is a left parenthesis,
      // pop operators off the stack onto the output queue
      while (sl > 0)
        {
        int sc = stack[sl - 1];
        if (sc == STR_LPAREN)
          {
          pe = true;
          break;
          }
        else
          {
          compiled.emplace_back(sc);
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
    if (sc == STR_LPAREN || sc == STR_RPAREN)
      {
      throw std::runtime_error("Query: parentheses mismatched");
      }
    compiled.emplace_back(sc);
    --sl;
    }
  }

std::string Query::to_string(StringPool const& pool) const
  {
  using Expression = std::pair<int, std::string>;
  std::stack<Expression> expr;
  for (int c : compiled)
    {
    if (is_ident(c))
      {
      expr.push({3, pool.to_string(c)});
      }
    else
      {
      Expression r = expr.top(); expr.pop();
      Expression l = expr.top(); expr.pop();
      int preced =  op_preced(c);
      if (l.first < preced)
        {
        l.second = '(' + l.second + ')';
        }
      if (r.first < preced)
        {
        r.second = '(' + r.second + ')';
        }
      expr.push({preced, l.second + pool.to_string(c) + r.second});
      }
    }
  return expr.top().second;
  }

bool Query::evaluate(int version, const Dictionary& variants, StringPool const& pool) const
  {
  if (compiled.empty())
    {
    return true;
    }

  // TODO: change to use std::vector/std::array
  int stack[32];
  unsigned int sl = 0; // stack length

  for (int c : compiled)
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
      int diff; bool res;
      if (is_ident(op1))
        {
        if (op1 == STR_VERSION)
          {
          op1 = version;
          }
        else
          {
          op1 = get_variant(variants, op1, pool);
          }
        }
      if (is_relation(c))
        {
        diff = vercmp(op1, op2, pool);
        }
      switch (c)
        {
        case STR_LESS:
          res = diff < 0;
          break;
        case STR_LESS_EQUAL:
          res = diff <= 0;
          break;
        case STR_GREATER:
          res = diff > 0;
          break;
        case STR_GREATER_EQUAL:
          res = diff >= 0;
          break;
        case STR_NOT_EQUAL:
          res = op1 != op2 && op1 != STR_ANY && op2 != STR_ANY;
          break;
        case STR_EQUAL:
          res = op1 == op2 || op1 == STR_ANY || op2 == STR_ANY;
          break;
        case STR_AND:
          res = op1 && op2;
          break;
        case STR_OR:
          res = op1 || op2;
          break;
        default:
          res = false;
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
  return stack[0];
  }

} // namespace Karrot
