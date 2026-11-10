# numeric-type-quad

Standalone C++20 library for `dd_128`, Boost `float128`, their complex wrappers,
`util::i128`, and the supported `pi<T>()` specializations.

## External dependencies

Boost headers are loaded from the system using `find_package(Boost REQUIRED)`.
No compiled Boost libraries are needed.

Eigen headers come from the shared `extern/` sources. QD headers and binaries
come from the repository's local `install/` prefix:

```text
extern/eigen/Eigen/Core
install/include/qd/dd_real.h
install/lib/libqd.so          # or libqd.a / lib64/
```

Install QD once with `bash script/install-qd.sh` before building this layer.
This project will not build or download any dependencies. A GCC-compatible compiler with
`__int128`, `__float128`, and the quadmath runtime is required. The quadmath
runtime, like the C++ standard library, comes from the compiler toolchain.

## Build, test, install

From the repository root, configure, build, and install with:

```sh
bash numeric-type-quad/install.sh
```

This uses `numeric-type-quad/build/` and installs into the repository's local
`install/` directory, without `sudo`. Additional arguments are forwarded to
CMake, for example `-DCMAKE_INSTALL_PREFIX=/path/to/install`. The environment
variables `BUILD_DIR`, `BUILD_JOBS`, and `INSTALL_PREFIX` can also be overridden.
The script stops on failure and does not automatically run tests.

Run the already compiled tests without rebuilding:

```sh
bash numeric-type-quad/run-test.sh
```

It uses the same `BUILD_DIR` override and forwards additional arguments to CTest.
Full test output is shown in the terminal and saved to `BUILD_DIR/test-output.txt`
(`numeric-type-quad/build/test-output.txt` by default), replacing the previous log.
Test failures still produce a nonzero exit status.
Tests must have been built with `NUMERIC_TYPE_BUILD_TESTS=ON`.

Or run the individual steps:

```sh
cmake -S numeric-type-quad -B build/numeric-type-quad \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX="$PWD/install" \
  -DNUMERIC_TYPE_BUILD_TESTS=ON
cmake --build build/numeric-type-quad --parallel 2
ctest --test-dir build/numeric-type-quad --output-on-failure
cmake --install build/numeric-type-quad
```

Use `-DNUMERIC_TYPE_EXTERN_DIR=/absolute/path/to/extern` to override the Eigen
source root, `-DQD_ROOT=/absolute/path/to/install` to select another QD
installation, `-DNUMERIC_TYPE_BUILD_TESTS=OFF` to skip tests, or
`-DBUILD_SHARED_LIBS=ON` to build a shared rather than static library.
`QD_ROOT` is independent of `CMAKE_INSTALL_PREFIX`, which selects where the
numeric library itself is installed.

## Consume without rebuilding

Public headers live in `include/numeric-type-quad/` in the source tree and
`install/include/numeric-type-quad/` after installation. Include them using the
library prefix to avoid collisions with other libraries:

```cpp
#include <numeric-type-quad/type_double_double.h>
#include <numeric-type-quad/type_float128_boost.h>
#include <numeric-type-quad/type_int128.h>
#include <numeric-type-quad/pi.h>
```

In a higher layer's `CMakeLists.txt`:

```cmake
find_package(numeric-type-quad CONFIG REQUIRED)
target_link_libraries(my_library PUBLIC numeric_type_quad::numeric_type_quad)
```

Configure that layer with `-DCMAKE_PREFIX_PATH=/absolute/path/to/install`.
The installed target records the configured Eigen/QD include and library paths.
If those dependencies move, reconfigure numeric-type-quad with the new
`NUMERIC_TYPE_EXTERN_DIR` and/or `QD_ROOT` and reinstall it. The install prefix is not
self-contained: external dependencies must remain available. No source checkout
of numeric-type-quad is needed by consumers.

All handwritten CMake configuration lives in `CMakeLists.txt`. Package-config
and version files are generated in the build directory during configuration;
there is no source `cmake/` folder to maintain.

The public target propagates C++20, external includes/link libraries, and
`EIGEN_DEFAULT_DENSE_INDEX_TYPE=int`. All consumers must agree on this Eigen ABI.

## Compiled code

`src/` contains the `pi<T>()` specializations, integer helpers, float128 stream
and rounding functions, and explicit instantiations of `std::complex<dd_128>`,
`std::complex<float128>`, and common integer constructors of `dd_128`.

The scalar aliases are not templates themselves and cannot be explicitly
instantiated. Small arithmetic overloads remain inline. This library does not
precompile arbitrary downstream Eigen expressions or numerical algorithms:
those templates still compile in the layer that uses them.
