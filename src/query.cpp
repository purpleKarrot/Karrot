/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "query.hpp"
#include "string.hpp"

#include "query_re2c.hpp"
#include "vercmp.hpp"
#include "variants.hpp"
#include <iostream>
#include <sstream>

namespace Karrot
{

static const String ASTERISK{"*"};
static const String VERSION{"version"};

static int op_preced(Query::Token::Id op)
  {
  switch (op)
    {
    case Query::Token::Less:
    case Query::Token::LessEqual:
    case Query::Token::Greater:
    case Query::Token::GreaterEqual:
      {
      return 4;
      }
    case Query::Token::Equal:
    case Query::Token::NotEqual:
      {
      return 3;
      }
    case Query::Token::And:
      {
      return 2;
      }
    case Query::Token::Or:
      {
      return 1;
      }
    }
  return 0;
  }

static inline bool is_operator(Query::Token::Id id)
  {
  return id <= Query::Token::Less && id >= Query::Token::Or;
  }

static inline bool is_relation(Query::Token::Id id)
  {
  return id <= Query::Token::Less && id >= Query::Token::GreaterEqual;
  }

static inline bool is_ident(Query::Token::Id id)
  {
  return id == Query::Token::Identifier;
  }

static inline String
get_variant(const KDictionary& variants, String const& key)
  {
  auto it = variants.find(key);
  if (it != variants.end())
    {
    return String{it->second};
    }
  std::stringstream message;
  message
    << "Unknown variable '"
    << key
    << "' used in test. Known variables are: "
    ;
  for (auto& entry : variants)
    {
    message << "'" << entry.first << "', ";
    }
  message << "and 'version'.";
  throw std::runtime_error(message.str());
  }

Query::Implementation::Implementation(const std::string& string)
  {
  if (string.empty())
    {
    return;
    }
  auto strpos = string.begin();
  auto strend = string.end();

  // TODO: change to use std::vector/std::array
  Token::Id stack[32]; // operator stack
  unsigned int sl = 0; // stack length

  while (strpos != strend)
    {
    // read one token from the input stream
    Token c = query_tokenize(strpos, strend);
    // If the token is a number (identifier), then add it to the output queue.
    if (is_ident(c.id))
      {
      compiled.push_back(c);
      }
    // If the token is an operator, op1, then:
    else if (is_operator(c.id))
      {
      while (sl > 0)
        {
        Token::Id sc = stack[sl - 1];
        // While there is an operator token, op2, at the top of the stack
        // op1 is left-associative and its precedence is less than or equal to that of op2,
        // or op1 has precedence less than that of op2,
        // Let + and ^ be right associative.
        // Correct transformation from 1^2+3 is 12^3+
        // The differing operator priority decides pop / push
        // If 2 operators have equal priority then associativity decides.
        if (is_operator(sc) && op_preced(c.id) <= op_preced(sc))
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
      stack[sl++] = c.id;
      }
    // If the token is a left parenthesis, then push it onto the stack.
    else if (c.id == Token::LParen)
      {
      stack[sl++] = c.id;
      }
    // If the token is a right parenthesis:
    else if (c.id == Token::RParen)
      {
      int pe = false;
      // Until the token at the top of the stack is a left parenthesis,
      // pop operators off the stack onto the output queue
      while (sl > 0)
        {
        Token::Id sc = stack[sl - 1];
        if (sc == Token::LParen)
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
    Token::Id sc = stack[sl - 1];
    if (sc == Token::LParen || sc == Token::RParen)
      {
      throw std::runtime_error("Query: parentheses mismatched");
      }
    compiled.emplace_back(sc);
    --sl;
    }
  }


bool Query::evaluate(const std::string& version, const KDictionary& variants) const
  {
  auto& compiled = implementation.get().compiled;
  if (compiled.empty())
    {
    return true;
    }

  // TODO: change to use std::vector/std::array
  Token stack[32];
  unsigned int sl = 0; // stack length

  for (auto& c : compiled)
    {
    if (is_ident(c.id))
      {
      stack[sl++] = c;
      }
    else if (is_operator(c.id))
      {
      if (sl < 2)
        {
        std::cout << "(Error) insufficient values in the expression." << std::endl;
        return false;
        }
      Token op2 = stack[--sl];
      Token op1 = stack[--sl];
      int diff; bool res;
      if (is_ident(op1.id))
        {
        if (op1.value == VERSION)
          {
          op1.value = version;
          }
        else
          {
          op1.value = get_variant(variants, op1.value);
          }
        }
      if (is_relation(c.id))
        {
        diff = vercmp(op1.value, op2.value);
        }
      switch (c.id)
        {
        case Token::Less:
          res = diff < 0;
          break;
        case Token::LessEqual:
          res = diff <= 0;
          break;
        case Token::Greater:
          res = diff > 0;
          break;
        case Token::GreaterEqual:
          res = diff >= 0;
          break;
        case Token::NotEqual:
          res = op1.value != op2.value && op1.value != ASTERISK && op2.value != ASTERISK;
          break;
        case Token::Equal:
          res = op1.value == op2.value || op1.value == ASTERISK || op2.value == ASTERISK;
          break;
        case Token::And:
          res = op1.id != Token::False && op2.id != Token::False;
          break;
        case Token::Or:
          res = op1.id != Token::False || op2.id != Token::False;
          break;
        }
      stack[sl++] = Token{res};
      }
    }
  if (sl != 1)
    {
    std::cout << "ERROR: " << sl << " elements left on the stack!" << std::endl;
    return false;
    }
  return stack[0].id != Token::False;
  }

} // namespace Karrot
