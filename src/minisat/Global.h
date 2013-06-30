/****************************************************************************************[Global.h]
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

#ifndef Global_h
#define Global_h

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <climits>
#include <cfloat>
#include <new>


//=================================================================================================
// Basic Types & Minor Things:


#ifdef _MSC_VER
typedef __int64            int64;
typedef unsigned __int64   uint64;
typedef long               intp;
typedef unsigned long      uintp;
#define I64_fmt "I64d"
#else
typedef long long          int64;
typedef unsigned long long uint64;
typedef __PTRDIFF_TYPE__   intp;
typedef unsigned __PTRDIFF_TYPE__ uintp;
#define I64_fmt "lld"
#endif
typedef unsigned      uint;
typedef unsigned char uchar;
typedef const char    cchar;


//=================================================================================================
// 'malloc()'-style memory allocation -- never returns NULL; aborts instead:


template<class T> static inline T* xmalloc(size_t size) {
    T*   tmp = (T*)malloc(size * sizeof(T));
    assert(size == 0 || tmp != NULL);
    return tmp; }

template<class T> static inline T* xrealloc(T* ptr, size_t size) {
    T*   tmp = (T*)realloc((void*)ptr, size * sizeof(T));
    assert(size == 0 || tmp != NULL);
    return tmp; }

template<class T> static inline void xfree(T *ptr) {
    if (ptr != NULL) free((void*)ptr); }


//=================================================================================================
// 'vec' -- automatically resizable arrays (via 'push()' method):


// NOTE! Don't use this vector on datatypes that cannot be re-located in memory (with realloc)

template<class T>
class vec {
    T*  data = nullptr;
    int sz = 0;
    int cap = 0;

    void     init(int size, const T& pad);
    void     grow(int min_cap);

public:
    // Types:
    using iterator = T*;

    // Constructors:
    vec()                              = default;
    vec(int size)                      { growTo(size); }
    vec(int size, const T& pad)        { growTo(size, pad); }
   ~vec(void)                          { clear(); }

    // Duplicatation:
    vec(vec const& other)              { growTo(other.sz); for (int i = 0; i < sz; i++) new (&data[i]) T(other.data[i]); }
    vec& operator=(vec other)          { swap(other); return *this; }

    // Iterator access:
    iterator const begin() const       { return data; }
    iterator       begin()             { return data; }
    iterator const end() const         { return data + sz; }
    iterator       end()               { return data + sz; }

    // Size operations:
    int      size   (void) const       { return sz; }
    void     shrink (int nelems)       { assert(nelems <= sz); for (int i = 0; i < nelems; i++) sz--, data[sz].~T(); }
    void     pop_back()                { sz--, data[sz].~T(); }
    void     growTo (int size);
    void     growTo (int size, const T& pad);
    void     clear  ();

    // Stack interface:
    void     emplace_back()            { if (sz == cap) grow(sz+1); new (&data[sz]) T()    ; sz++; }
    void     push_back(const T& elem)  { if (sz == cap) grow(sz+1); new (&data[sz]) T(elem); sz++; }
    const T& back  (void) const        { return data[sz-1]; }
    T&       back  (void)              { return data[sz-1]; }

    // Vector interface:
    const T& operator [] (int index) const  { return data[index]; }
    T&       operator [] (int index)        { return data[index]; }

    void swap(vec& other)              { std::swap(data, other.data); std::swap(sz, other.sz); std::swap(cap, other.cap); }
};

template<class T>
void vec<T>::grow(int min_cap) {
    if (min_cap <= cap) return;
    if (cap == 0) cap = (min_cap >= 2) ? min_cap : 2;
    else          do cap = (cap*3+1) >> 1; while (cap < min_cap);
    data = xrealloc(data, cap); }

template<class T>
void vec<T>::growTo(int size, const T& pad) {
    if (sz >= size) return;
    grow(size);
    for (int i = sz; i < size; i++) new (&data[i]) T(pad);
    sz = size; }

template<class T>
void vec<T>::growTo(int size) {
    if (sz >= size) return;
    grow(size);
    for (int i = sz; i < size; i++) new (&data[i]) T();
    sz = size; }

template<class T>
void vec<T>::clear() {
    if (data != NULL){
        for (int i = 0; i < sz; i++) data[i].~T();
        sz = 0; xfree(data); data = NULL; cap = 0; } }


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
#endif
