#include "type_int128.h"

#include <algorithm>
#include <istream>
#include <ostream>

namespace util {

std::string to_string(i128 x) {
    if (x == 0) return "0";
    const bool neg = x < 0;
    // Unsigned magnitude also handles the most negative signed value.
    using u128 = unsigned __int128;
    u128 magnitude = neg ? u128(0) - static_cast<u128>(x) : static_cast<u128>(x);
    std::string s;
    while (magnitude > 0) {
        s.push_back('0' + (magnitude % 10));
        magnitude /= 10;
    }
    if (neg) s.push_back('-');
    std::reverse(s.begin(), s.end());
    return s;
}

i128 parse_i128(const std::string& s) {
    i128 x = 0;
    const bool neg = !s.empty() && s[0] == '-';
    for (size_t i = neg ? 1 : 0; i < s.size(); ++i) {
        // Accumulate negative numbers negatively to support the signed minimum.
        x = neg ? x * 10 - (s[i] - '0') : x * 10 + (s[i] - '0');
    }
    return x;
}

std::istream& operator>>(std::istream& is, i128& x) {
    std::string s;
    if (is >> s) x = parse_i128(s);
    return is;
}

i128 abs(i128 x) {
    return x < 0 ? -x : x;
}

i128 pow10(int exp) {
    i128 r = 1;
    for (int i = 0; i < exp; ++i) r *= 10;
    return r;
}

} // namespace util

std::ostream& operator<<(std::ostream& os, util::i128 x) {
    return os << util::to_string(x);
}
