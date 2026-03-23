# Test executable that links against rawdger-core and Catch2.
# Catch2::Catch2WithMain provides a pre-built main(), so test files
# only need to define TEST_CASEs.
add_executable(rawdger-tests
    RecorderTests.cpp
)

target_link_libraries(rawdger-tests
    PRIVATE
        rawdger-core
        Catch2::Catch2WithMain
)

# include(Catch) loads Catch2's CMake integration module, which provides
# catch_discover_tests(). This auto-discovers TEST_CASEs in the binary at
# build time and registers each one as an individual CTest test.
include(Catch)
catch_discover_tests(rawdger-tests)
