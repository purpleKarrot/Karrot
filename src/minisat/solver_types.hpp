/***********************************************************************************[SolverTypes.h]
MiniSat -- Copyright (c) 2003-2005, Niklas Een, Niklas Sorensson

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**************************************************************************************************/


#ifndef SolverTypes_h
#define SolverTypes_h

#include <cassert>
#include <cstdint>
#include <cstdlib>


//=================================================================================================
// Lifted booleans:


class lbool {
    int     value;
    explicit lbool(int v) : value(v) { }

public:
    lbool()       : value(0) { }
    lbool(bool x) : value((int)x*2-1) { }
    int toInt(void) const { return value; }

    bool  operator == (const lbool& other) const { return value == other.value; }
    bool  operator != (const lbool& other) const { return value != other.value; }
    lbool operator ~  (void)               const { return lbool(-value); }

    friend int   toInt  (lbool l);
    friend lbool toLbool(int   v);
};

inline int   toInt  (lbool l) { return l.toInt(); }
inline lbool toLbool(int   v) { return lbool(v);  }

const lbool l_True  = toLbool( 1);
const lbool l_False = toLbool(-1);
const lbool l_Undef = toLbool( 0);


//=================================================================================================
// Variables, literals, clause IDs:


// NOTE! Variables are just integers. No abstraction here. They should be chosen from 0..N,
// so that they can be used as array indices.

typedef int Var;
#define var_Undef (-1)


class Lit
  {
  public:
    Lit() // (lit_Undef)
        : x(2 * var_Undef)
      {
      }
    explicit Lit(Var var, bool sgn = false)
        : x((var + var) + (int) sgn)
      {
      }
    Lit operator~() const
      {
      Lit q;
      q.x = x ^ 1;
      return q;
      }
    uint hash() const
      {
      return (uint) x;
      }
  private:
    friend bool sign  (Lit p);
    friend int  var   (Lit p);
    friend int  index (Lit p);
    friend Lit  toLit (int i);
    friend Lit  unsign(Lit p);
    friend Lit  id    (Lit p, bool sgn);
    friend bool operator == (Lit p, Lit q);
    friend bool operator <  (Lit p, Lit q);
  private:
    int x;
  };

inline bool sign(Lit p)
  {
  return p.x & 1;
  }

inline int var(Lit p)
  {
  return p.x >> 1;
  }

// A "toInt" method that guarantees small, positive integers suitable for array indexing.
inline int index(Lit p)
  {
  return p.x;
  }

// Inverse of 'index()'.
inline Lit toLit(int i)
  {
  Lit p;
  p.x = i;
  return p;
  }

inline Lit unsign(Lit p)
  {
  Lit q;
  q.x = p.x & ~1;
  return q;
  }

inline Lit id(Lit p, bool sgn)
  {
  Lit q;
  q.x = p.x ^ (int) sgn;
  return q;
  }

inline bool operator ==(Lit p, Lit q)
  {
  return index(p) == index(q);
  }

// '<' guarantees that p, ~p are adjacent in the ordering.
inline bool operator <(Lit p, Lit q)
  {
  return index(p) < index(q);
  }

const Lit lit_Undef(var_Undef, false);  // }- Useful special constants.
const Lit lit_Error(var_Undef, true );  // }

inline int toDimacs(Lit p)
  {
  return sign(p) ? -var(p) - 1 : var(p) + 1;
  }


//=================================================================================================
// Clause -- a simple class for representing a clause:


class Clause
  {
  public:
    static Clause* create(std::vector<Lit> const& ps, bool learnt)
      {
      assert(sizeof(Lit) == sizeof(uint));
      assert(sizeof(float) == sizeof(uint));
      void* mem = std::malloc(sizeof(Clause) - sizeof(Lit) + sizeof(uint) * (ps.size() + (int) learnt));
      return new (mem) Clause(ps, learnt);
      }
  public:
    int size() const
      {
      return size_learnt >> 1;
      }
    bool learnt() const
      {
      return size_learnt & 1;
      }
    Lit const operator [](int i) const
      {
      return data[i];
      }
    Lit& operator [](int i)
      {
      return data[i];
      }
    float activity() const
      {
      return *((float*) &data[size()]);
      }
    float& activity()
      {
      return *((float*) &data[size()]);
      }
  private:
    // NOTE: This constructor cannot be used directly (doesn't allocate enough memory).
    Clause(std::vector<Lit> const& ps, bool learnt)
      {
      size_learnt = (ps.size() << 1) | (int) learnt;
      for (int i = 0; i < ps.size(); i++)
        {
        data[i] = ps[i];
        }
      if (learnt)
        {
        activity() = 0;
        }
      }
  private:
    uint size_learnt;
    Lit data[1];
  };


//=================================================================================================
// GClause -- Generalize clause:


// Either a pointer to a clause or a literal.
class GClause
  {
    using intp = std::ptrdiff_t;
    using uintp = unsigned intp;
  public:
    static GClause create(Lit p)
      {
      return GClause((void*) (((intp) index(p) << 1) + 1));
      }
    static GClause create(Clause* c)
      {
      assert(((uintp) c & 1) == 0);
      return GClause((void*) c);
      }
    bool isLit() const
      {
      return ((uintp) data & 1) == 1;
      }
    Lit lit() const
      {
      return toLit(((intp) data) >> 1);
      }
    Clause* clause() const
      {
      return (Clause*) data;
      }
    bool operator ==(GClause c) const
      {
      return data == c.data;
      }
    bool operator !=(GClause c) const
      {
      return data != c.data;
      }
  private:
    GClause(void* d)
        : data(d)
      {
      }
    void* data;
  };

static GClause GClause_NULL = GClause::create(nullptr);

#endif
