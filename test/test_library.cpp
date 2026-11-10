// Deliberately include float128/Eigen first: the target's public index-type
// definition must make both header orders safe.
#include "type_float128_boost.h"
#include "type_double_double.h"

#include <iostream>
#include <limits>
#include <sstream>
#include <type_traits>

static_assert(std::is_same_v<Eigen::Index, int>);

int main() {
    const auto check = [](bool ok, const char* message) {
        if (!ok) std::cerr << message << '\n';
        return ok;
    };
    bool ok = true;
    ok &= check(pi<double>() > 3.14 && pi<double>() < 3.15, "double pi");
    ok &= check(pi<dd_128>() > dd_128(3.14) && pi<dd_128>() < dd_128(3.15), "dd pi");
    ok &= check(pi<float128>() > float128(3.14) && pi<float128>() < float128(3.15), "float128 pi");

    const Cdd_128 dd_product = Cdd_128(1, 2) * Cdd_128(2, -1);
    ok &= check(dd_product.real() == dd_128(4) && dd_product.imag() == dd_128(3), "dd complex");
    const Cfloat128 f_product = Cfloat128(1, 2) * Cfloat128(2, -1);
    ok &= check(f_product.real() == float128(4) && f_product.imag() == float128(3), "float128 complex");
    ok &= check(llround(float128("2.5")) == 3 && llround(float128("-2.5")) == -3, "float128 rounding");

    const util::i128 large = util::pow10(30);
    ok &= check(util::parse_i128(util::to_string(large)) == large, "i128 roundtrip");
    const auto minimum = std::numeric_limits<util::i128>::min();
    ok &= check(util::parse_i128(util::to_string(minimum)) == minimum, "i128 minimum roundtrip");
    std::ostringstream out;
    out << util::i128(-42) << ' ' << Cfloat128(1, 2);
    ok &= check(out.str() == "-42 (1,2)", "stream output");
    util::i128 parsed = 0;
    std::istringstream in("-123");
    util::operator>>(in, parsed);
    ok &= check(parsed == -123, "i128 input");
    return ok ? 0 : 1;
}
