#include <numeric-type-quad/type_double_double.h>

// dd_128 itself is concrete, not a class template. Instantiate its templated
// integer constructors and its complex wrapper, not the scalar type itself.
template dd_128::dd_128(int);
template dd_128::dd_128(unsigned int);
template dd_128::dd_128(long);
template dd_128::dd_128(unsigned long);
template dd_128::dd_128(long long);
template dd_128::dd_128(unsigned long long);
template class std::complex<dd_128>;

template<>
dd_128 pi<dd_128>() {
    return dd_real::_pi;
}
