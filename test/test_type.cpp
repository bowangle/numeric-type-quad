#include <iostream>
#include <complex>

// Include your type headers
#include "type_double_double.h"
#include "type_float128_boost.h"
#include "type_int128.h"

int main() {
    std::cout << "=== Testing Numeric Type Instantiation ===\n\n";

    // ============================================================
    // 1. DOUBLE-DOUBLE TYPES (from type_double_double.h)
    // ============================================================
    std::cout << "--- Double-Double Types ---\n";
    
    // Real double-double
    dd_128 dd_real = 3.14;
    std::cout << "dd_128 (real): " << dd_real << "\n";
    
    // Complex double-double
    Cdd_128 dd_complex(1.0, 2.0);
    std::cout << "Cdd_128 (complex): " << dd_complex << "\n";
    
    // Test operations
    dd_128 dd_sum = dd_real + dd_128(2.71);
    std::cout << "dd_128 addition: " << dd_sum << "\n";
    
    Cdd_128 dd_prod = dd_complex * Cdd_128(2.0, -1.0);
    std::cout << "Cdd_128 multiplication: " << dd_prod << "\n";

    // ============================================================
    // 2. FLOAT128 TYPES (from type_float128_boost.h)
    // ============================================================
    std::cout << "\n--- Float128 Types ---\n";
    
    // Real float128
    float128 f128_real = 2.71828;
    std::cout << "float128 (real): " << f128_real << "\n";
    
    // Complex float128
    Cfloat128 f128_complex(3.0, 4.0);
    std::cout << "Cfloat128 (complex): " << f128_complex << "\n";
    
    // Test operations
    float128 f128_diff = f128_real - float128(1.0);
    std::cout << "float128 subtraction: " << f128_diff << "\n";
    
    Cfloat128 f128_sum = f128_complex + Cfloat128(1.0, -1.0);
    std::cout << "Cfloat128 addition: " << f128_sum << "\n";

    // ============================================================
    // 3. INT128 TYPE (from type_int128.h)
    // ============================================================
    std::cout << "\n--- Int128 Type ---\n";
    
    // Int128
    util::i128 i128_val = 1234567890123456789LL;
    std::cout << "util::i128: " << i128_val << "\n";
    
    // Test operations
    util::i128 i128_sum = i128_val + util::i128(1);
    std::cout << "util::i128 addition: " << i128_sum << "\n";
    
    util::i128 i128_prod = i128_val * util::i128(2);
    std::cout << "util::i128 multiplication: " << i128_prod << "\n";
    
    // Test negative
    util::i128 i128_neg = -i128_val;
    std::cout << "util::i128 negative: " << i128_neg << "\n";
    
    // Test abs
    std::cout << "util::i128 abs: " << util::abs(i128_neg) << "\n";

    // ============================================================
    // 4. MIXED TYPE OPERATIONS
    // ============================================================
    std::cout << "\n--- Mixed Type Operations ---\n";
    
    // Convert int128 to double-double using our conversion function
    dd_128 dd_from_i128 = dd_from_i128_type(i128_val);
    std::cout << "dd_128 from i128: " << dd_from_i128 << "\n";
    
    // Also test the template version
    dd_128 dd_from_i128_template = to_dd(i128_val);
    std::cout << "dd_128 from i128 (template): " << dd_from_i128_template << "\n";
    
    // Convert int128 to float128
    float128 f128_from_i128 = static_cast<float128>(i128_val);
    std::cout << "float128 from i128: " << f128_from_i128 << "\n";

    // ============================================================
    // 5. EDGE CASES
    // ============================================================
    std::cout << "\n--- Edge Cases ---\n";
    
    // Zero values
    dd_128 dd_zero = 0.0;
    float128 f128_zero = 0.0;
    util::i128 i128_zero = 0;
    std::cout << "Zero values: dd=" << dd_zero << ", f128=" << f128_zero << ", i128=" << i128_zero << "\n";
    
    // Complex zero
    Cdd_128 dd_czero(0.0, 0.0);
    Cfloat128 f128_czero(0.0, 0.0);
    std::cout << "Complex zeros: dd=" << dd_czero << ", f128=" << f128_czero << "\n";

    // Large values
    util::i128 i128_large = util::pow10(30);
    std::cout << "Large i128 (10^30): " << i128_large << "\n";

    std::cout << "\n=== All type instantiations successful! ===\n";
    
    return 0;
}
