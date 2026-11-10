#pragma once
#include <numeric-type-quad/pi.h>
#include <boost/multiprecision/float128.hpp>
#include <complex>
#include <ostream>
#include <Eigen/Dense>

#include <numeric-type-quad/type_int128.h>

using float128 = boost::multiprecision::float128;
using Cfloat128   = std::complex<float128>;
extern template class std::complex<float128>;
using MatrixX128 = Eigen::Matrix<Cfloat128, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

std::ostream& operator<<(std::ostream& os, const Cfloat128& z);
util::i128 llround(const float128& v);

// ============================================================
// pi() SPECIALISATION
// ============================================================
template<> float128 pi<float128>();