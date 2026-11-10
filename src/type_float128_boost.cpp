#include "type_float128_boost.h"

#include <boost/math/constants/constants.hpp>

// float128 is an alias of Boost's number type, not a new class template.
// Avoid explicitly instantiating Boost's entire implementation.
template class std::complex<float128>;

std::ostream& operator<<(std::ostream& os, const Cfloat128& z) {
    return os << "(" << z.real() << "," << z.imag() << ")";
}

util::i128 llround(const float128& v) {
    using boost::multiprecision::ceil;
    using boost::multiprecision::floor;
    // Round to nearest, ties away from zero, without narrowing to long long.
    float128 r = (v < float128(0.0)) ? ceil(v - float128(0.5))
                                    : floor(v + float128(0.5));
    return static_cast<util::i128>(r);
}

template<>
float128 pi<float128>() {
    return boost::math::constants::pi<float128>();
}
