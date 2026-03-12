# Create a static library target called "rawdger-core".
# STATIC means it produces a .a (macOS/Linux) or .lib (Windows) archive that
# gets linked directly into the final executables — no separate .dll/.dylib needed.
# All source files for this library are listed here.
add_library(rawdger-core STATIC
    src/Core.cpp
)

# Make the include/ directory available to anything that links against rawdger-core.
# PUBLIC means both this library AND any target that links it (via target_link_libraries)
# will have this directory in their include search path. This is why juce-app and
# qt-app can do #include "rawdger/Core.h" without specifying the path themselves.
# ${CMAKE_CURRENT_SOURCE_DIR} resolves to the directory this CMakeLists.txt is in (core/).
target_include_directories(rawdger-core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
