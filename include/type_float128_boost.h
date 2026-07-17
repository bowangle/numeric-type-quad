#pragma once
#include <boost/multiprecision/float128.hpp>
#include <Eigen/Dense>

#include "type_int128.h"

using float128 = boost::multiprecision::float128;
using Cfloat128   = std::complex<float128>;
using MatrixX128 = Eigen::Matrix<Cfloat128, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

inline std::ostream& operator<<(std::ostream& os,
                                const Cfloat128& z)
{
    os << "(" << z.real() << "," << z.imag() << ")";
    return os;
}

inline util::i128 llround(const float128& v) {                                         
       using boost::multiprecision::floor;                                                
       using boost::multiprecision::ceil;                                                 
                                                                                          
       // round to nearest, ties away from zero                                           
       float128 r = (v < float128(0.0)) ? ceil(v - float128(0.5))                         
                                        : floor(v + float128(0.5));                       
                                                                                          
       // float128 has ~34 decimal digits of mantissa, i128 holds ~38 digits,             
       // so the conversion is exact for integer-valued r (up to ~2^113).                 
       return static_cast<util::i128>(r);                                                 
   }         