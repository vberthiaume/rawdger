# juce_add_gui_app is a JUCE-provided CMake function (available because the
# top-level CMakeLists.txt did FetchContent_MakeAvailable(juce)). It creates an
# executable target configured as a GUI application with JUCE-specific setup
# (module handling, binary artefact output paths, etc.).
# PRODUCT_NAME sets the user-facing app name.
juce_add_gui_app(rawdger-juce
    PRODUCT_NAME "Rawdger JUCE"
)

# Add source files to the target. PRIVATE means these files are only used to
# build this target — they won't be propagated to anything that links rawdger-juce.
target_sources(rawdger-juce PRIVATE
    Source/Main.cpp
)

# Link libraries to this target. PRIVATE means these dependencies are only used
# internally — they won't leak to other targets.
#   - rawdger-core: our shared library (also brings in its PUBLIC include dirs)
#   - juce::juce_gui_basics: JUCE's GUI module (components, buttons, windows, etc.).
#     This transitively includes juce_core and juce_graphics.
#   - juce::juce_recommended_config_flags: JUCE-recommended compiler flags
#     (warnings, optimizations, etc.)
target_link_libraries(rawdger-juce PRIVATE
    rawdger-core
    juce::juce_gui_basics
    juce::juce_recommended_config_flags
)

# On Linux, JUCE's juce_core module uses libcurl for HTTP but doesn't link it
# automatically. We use pkg-config to find it and link it ourselves.
# find_package(PkgConfig) loads CMake's pkg-config integration.
# pkg_check_modules finds the library and creates the PkgConfig::CURL target.
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(CURL REQUIRED IMPORTED_TARGET libcurl)
    target_link_libraries(rawdger-juce PRIVATE PkgConfig::CURL)
endif()
