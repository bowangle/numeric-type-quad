#pragma once
#include <cmath>

template<typename Real>
Real pi() {
    static_assert(sizeof(Real) == 0,
        "pi() not specialised for this type — add a specialisation or include the right type header");
    return Real(0);
}

template<> inline double pi<double>() { return M_PI; }
