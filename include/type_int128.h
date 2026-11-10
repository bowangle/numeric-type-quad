#pragma once

#include <iosfwd>
#include <string>

namespace util {

// A built-in scalar alias: there is no template to instantiate for i128.
using i128 = __int128;

std::string to_string(i128 x);
i128 parse_i128(const std::string& s);
std::istream& operator>>(std::istream& is, i128& x);
i128 abs(i128 x);
i128 pow10(int exp);

} // namespace util

std::ostream& operator<<(std::ostream& os, util::i128 x);
