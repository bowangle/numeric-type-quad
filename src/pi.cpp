#include "pi.h"

#include <numbers>

template<>
double pi<double>() {
    return std::numbers::pi_v<double>;
}
