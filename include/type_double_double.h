#pragma once
#include "pi.h"
// ============================================================
// -1. EIGEN INDEX TYPE
// ============================================================
#if defined(EIGEN_CORE_H) && !defined(EIGEN_DEFAULT_DENSE_INDEX_TYPE)
#  error "type_double_double.h must be included BEFORE any Eigen header, or define EIGEN_DEFAULT_DENSE_INDEX_TYPE=int as a global build flag."
#endif
#ifndef EIGEN_DEFAULT_DENSE_INDEX_TYPE
#  define EIGEN_DEFAULT_DENSE_INDEX_TYPE int
#endif
#include <qd/dd_real.h>
#include <complex>
#include <cmath>
#include <Eigen/Core>
#include <type_traits>
#include <cstddef>
#include <limits>
#include <cstdint>
#include "type_int128.h"
// ============================================================
// 0. EXACT INTEGER -> dd_real CONVERSIONS (implementation)
// ============================================================
inline dd_real to_dd(double x) { return dd_real(x); }
inline dd_real to_dd(int x)    { return dd_real(x); }
// i128: hi (nearest double) + residual, renormalized by dd addition.
// Exact for |x| < 2^106. Residual computed in unsigned arithmetic to
// avoid UB when hi rounds up to 2^127 near the top of the range.
inline dd_real to_dd(util::i128 x) {
    using u128 = unsigned __int128;
    const bool neg = x < 0;
    u128 u = neg ? u128(0) - static_cast<u128>(x) : static_cast<u128>(x);
    double hi = static_cast<double>(u);
    double residual;
    if (hi >= 1.7014118346046923e38) {          // hi rounded up to 2^127
        residual = -static_cast<double>((u128(1) << 127) - u);
    } else {
        u128 hi_u = static_cast<u128>(hi);
        residual = (u >= hi_u)
            ?  static_cast<double>(u - hi_u)
            : -static_cast<double>(hi_u - u);
    }
    dd_real r = dd_real(hi) + dd_real(residual);
    return neg ? -r : r;
}
inline dd_real to_dd(long long x)          { return to_dd(static_cast<util::i128>(x)); }
inline dd_real to_dd(long x)               { return to_dd(static_cast<util::i128>(x)); }
inline dd_real to_dd(unsigned long x)      { return to_dd(static_cast<util::i128>(x)); }
inline dd_real to_dd(unsigned long long x) { return to_dd(static_cast<util::i128>(x)); }
// ============================================================
// 0b. dd_128: dd_real + the missing integer constructors
// ------------------------------------------------------------
// dd_real cannot be given new constructors from outside, so
// dd_128 is a derived type that adds them. Every dd_real
// operation (operators, sqrt, abs, ...) still applies through
// the base, and results convert back via dd_128(const dd_real&).
//
// This makes `Scalar(k)` compile for int, long, long long, and
// __int128 wherever Scalar = dd_128 — no changes in grid.h.
// ============================================================
struct dd_128 : dd_real {
    dd_128() : dd_real() {}
    dd_128(const dd_real& v) : dd_real(v) {}      // ops return dd_real -> back to dd_128
    dd_128(double v)             : dd_real(v) {}
    dd_128(int v)                : dd_real(v) {}
    dd_128(long v)               : dd_real(to_dd(v)) {}
    dd_128(long long v)          : dd_real(to_dd(v)) {}
    dd_128(unsigned long v)      : dd_real(to_dd(v)) {}
    dd_128(unsigned long long v) : dd_real(to_dd(v)) {}
    dd_128(util::i128 v)         : dd_real(to_dd(v)) {}   // <- the fix for line 150
};
using Cdd_128 = std::complex<dd_128>;
// ============================================================
// 0b'. std::numeric_limits<dd_128>
// ------------------------------------------------------------
// qd already specializes numeric_limits<dd_real> in
// qd/dd_real.h (inheriting from numeric_limits<double>), so we
// must NOT redefine that one. But qd's specialization does not
// override max_digits10, which therefore stays at double's 17 —
// not enough for the ~106-bit mantissa. We specialize for
// dd_128 only, inheriting qd's and fixing the constants.
//
// Placement matters: a specialization must appear before any
// implicit instantiation of numeric_limits<dd_128>, so this
// sits immediately after the type definition, before Eigen
// traits or any other code can touch it.
// ============================================================
namespace std {
template<>
struct numeric_limits<dd_128> : numeric_limits<dd_real> {
    static constexpr int digits       = 106;
    static constexpr int digits10     = 31;
    static constexpr int max_digits10 = 33;   // the fix: was inheriting 17

    // Re-wrap value-returning functions so they return dd_128
    // (qd's versions return dd_real or double).
    static dd_128 max()     { return dd_128(dd_real::_max); }
    static dd_128 lowest()  { return dd_128(-dd_real::_max); }
    static dd_128 epsilon() { return dd_128(dd_real::_eps); }
    static dd_128 min()     { return dd_128(numeric_limits<dd_real>::min()); }
};
} // namespace std
// ============================================================
// 0c. CLOSED ARITHMETIC FOR dd_128
// ------------------------------------------------------------
// Without these, dd_128 op dd_128 resolves to the inherited
// dd_real operators and yields dd_real. In assignment contexts
// the dd_128(const dd_real&) constructor hides this, but any
// common-type deduction (?:, auto, template deduction) inside
// Eigen then sees two different, mutually convertible types and
// fails (e.g. RankRevealingBase::threshold()).
//
// For two dd_128 arguments these overloads are exact matches,
// so they always beat the inherited dd_real operators (which
// need a derived-to-base conversion). The static_casts inside
// the bodies force resolution to the base operators, preventing
// infinite recursion. Compound assignments (+=, ...) and
// comparisons are inherited unchanged: they mutate in place or
// return bool, so no type leaks out.
// ============================================================
#define DD128_BINOP(op)                                                        \
    inline dd_128 operator op (const dd_128& a, const dd_128& b) {             \
        return dd_128(static_cast<const dd_real&>(a) op                        \
                      static_cast<const dd_real&>(b));                         \
    }                                                                          \
    inline dd_128 operator op (const dd_128& a, double b) {                    \
        return dd_128(static_cast<const dd_real&>(a) op b);                    \
    }                                                                          \
    inline dd_128 operator op (double a, const dd_128& b) {                    \
        return dd_128(a op static_cast<const dd_real&>(b));                    \
    }
DD128_BINOP(+)
DD128_BINOP(-)
DD128_BINOP(*)
DD128_BINOP(/)
#undef DD128_BINOP
inline dd_128 operator-(const dd_128& a) {
    return dd_128(-static_cast<const dd_real&>(a));
}
inline dd_128 operator+(const dd_128& a) { return a; }
// ============================================================
// 0d. CLOSED MATH FUNCTIONS FOR dd_128
// ------------------------------------------------------------
// Same issue as the operators: sqrt(dd_128) etc. would otherwise
// return dd_real. Eigen's numext wrappers find these via ADL
// (dd_128 lives in the global namespace, as do these overloads),
// and norms / Householder / Jacobi paths rely on the result type
// matching RealScalar exactly.
// ============================================================
#define DD128_UNARY_FN(fn)                                                     \
    inline dd_128 fn(const dd_128& a) {                                        \
        return dd_128(fn(static_cast<const dd_real&>(a)));                     \
    }
DD128_UNARY_FN(sqrt)
DD128_UNARY_FN(abs)
DD128_UNARY_FN(fabs)
DD128_UNARY_FN(floor)
DD128_UNARY_FN(ceil)
DD128_UNARY_FN(exp)
DD128_UNARY_FN(log)
DD128_UNARY_FN(log10)
DD128_UNARY_FN(sin)
DD128_UNARY_FN(cos)
DD128_UNARY_FN(tan)
DD128_UNARY_FN(asin)
DD128_UNARY_FN(acos)
DD128_UNARY_FN(atan)
DD128_UNARY_FN(sinh)
DD128_UNARY_FN(cosh)
DD128_UNARY_FN(tanh)
#undef DD128_UNARY_FN
inline dd_128 pow(const dd_128& a, const dd_128& b) {
    return dd_128(pow(static_cast<const dd_real&>(a),
                      static_cast<const dd_real&>(b)));
}
inline dd_128 pow(const dd_128& a, int b) {
    return dd_128(pow(static_cast<const dd_real&>(a), b));
}
inline dd_128 atan2(const dd_128& a, const dd_128& b) {
    return dd_128(atan2(static_cast<const dd_real&>(a),
                        static_cast<const dd_real&>(b)));
}
inline dd_128 fmod(const dd_128& a, const dd_128& b) {
    return dd_128(fmod(static_cast<const dd_real&>(a),
                       static_cast<const dd_real&>(b)));
}
// hypot(dd_128, dd_128): forwards to the dd_real hypot defined
// in section 1 below; declared after it via the trailing overload.
// ============================================================
// llround for dd_real / dd_128 (found via ADL from grid.h)
// ------------------------------------------------------------
// Returns util::i128 rather than long long so the same overload
// serves QTGrid<dd_128, int>, <dd_128, long long> and
// <dd_128, __int128>: grid.h's static_cast<Sint>(...) narrows
// as needed. Crucially, i128 also holds 2^63 (Sint = i128,
// nBits = 63, x == b) which would overflow a long long *before*
// the k == N clamp in coord_to_id.
//
// Rounds half away from zero, matching std::llround. The result
// is rebuilt from BOTH components of the double-double, so it is
// exact above 2^53 where llround(to_double(x)) would not be.
//
// Takes const dd_real&, so dd_128 arguments bind via the base —
// no separate dd_128 overload needed (the return type is an
// integer, so no dd_real can leak out).
// ============================================================
inline util::i128 llround(const dd_real& v) {
    // round to nearest, ties away from zero
    dd_real r = (v < dd_real(0.0)) ? ceil(v - dd_real(0.5))
                                   : floor(v + dd_real(0.5));
    // r is integer-valued, so both normalized components are
    // integer-valued doubles; each converts to i128 exactly and
    // their sum reconstructs r exactly (up to ~2^106).
    return static_cast<util::i128>(r.x[0]) + static_cast<util::i128>(r.x[1]);
}

// ============================================================
// pi() SPECIALISATION
// ============================================================
template<> inline dd_128 pi<dd_128>() { return dd_real::_pi; }
// ============================================================
// 1. MATH SUPPORT (Eigen ADL)
// ============================================================
inline dd_real hypot(const dd_real& a, const dd_real& b) {
    dd_real ax = abs(a), ay = abs(b);
    if (ax == dd_real(0.0)) return ay;
    if (ay == dd_real(0.0)) return ax;
    dd_real hi = (ax > ay) ? ax : ay;
    dd_real lo = (ax > ay) ? ay : ax;
    dd_real r = lo / hi;
    return hi * sqrt(dd_real(1.0) + r * r);
}
inline dd_128 hypot(const dd_128& a, const dd_128& b) {
    return dd_128(hypot(static_cast<const dd_real&>(a),
                        static_cast<const dd_real&>(b)));
}
// ============================================================
// 2. Eigen TRAITS (for both dd_real and dd_128)
// ============================================================
namespace Eigen {
template<>
struct NumTraits<dd_real> : GenericNumTraits<dd_real> {
    using Real = dd_real;
    using NonInteger = dd_real;
    using Nested = dd_real;
    using Literal = dd_real;
    enum {
        IsComplex = 0, IsInteger = 0, IsSigned = 1,
        RequireInitialization = 1, ReadCost = 2, AddCost = 16, MulCost = 32
    };
    static Real epsilon()         { return dd_real::_eps; }
    static Real dummy_precision() { return dd_real(1e-30); }
    static Real highest()         { return dd_real::_max; }
    static Real lowest()          { return -dd_real::_max; }
    static int digits10()         { return 31; }
    static int max_digits10()     { return 33; }
};
template<>
struct NumTraits<dd_128> : GenericNumTraits<dd_128> {
    using Real = dd_128;
    using NonInteger = dd_128;
    using Nested = dd_128;
    using Literal = dd_128;
    enum {
        IsComplex = 0, IsInteger = 0, IsSigned = 1,
        RequireInitialization = 1, ReadCost = 2, AddCost = 16, MulCost = 32
    };
    static Real epsilon()         { return dd_128(dd_real::_eps); }
    static Real dummy_precision() { return dd_128(1e-30); }
    static Real highest()         { return dd_128(dd_real::_max); }
    static Real lowest()          { return dd_128(-dd_real::_max); }
    static int digits10()         { return 31; }
    static int max_digits10()     { return 33; }
};
}