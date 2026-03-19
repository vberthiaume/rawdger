# Minimum CMake version required. The range syntax (min...max) tells CMake
# "I need at least 3.24, but I've tested up to 4.2" — this avoids deprecation
# warnings on newer CMake versions while keeping backward compatibility.
cmake_minimum_required(VERSION 3.24...4.2)

# Declare the project name, version, and the languages used.
# VERSION sets variables like PROJECT_VERSION, PROJECT_VERSION_MAJOR, etc.
project(rawdger VERSION 0.1.0 LANGUAGES C CXX)

# Set the C++ standard for all targets in this project.
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Options ---
# option() defines a boolean cache variable that can be toggled from the command
# line with -D, e.g.: cmake -B build -DRAWDGER_BUILD_QT=OFF
# These must be declared before they're used in if() blocks below, and it's a good
# idea to prefix cmake variables with the project name so they don't clash with other variables.
option(RAWDGER_BUILD_JUCE "Build the JUCE app" ON)
option(RAWDGER_BUILD_QT   "Build the Qt app"   ON)

# --- Dependencies ---
# FetchContent lets us download and build external projects (like JUCE) at
# configure time, without requiring the user to manually clone or install them.
include(FetchContent)

if(RAWDGER_BUILD_JUCE)
    # FetchContent_Declare defines where to get JUCE from but doesn't download yet.
    # GIT_SHALLOW TRUE avoids cloning the full history (faster).
    FetchContent_Declare(juce
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG        8.0.4
        GIT_SHALLOW    TRUE
    )
    # FetchContent_MakeAvailable triggers the download (if not cached) and runs
    # JUCE's own CMakeLists.txt via add_subdirectory, making its targets (like
    # juce::juce_core) available to our project.
    FetchContent_MakeAvailable(juce)
endif()

# --- Subdirectories ---
# add_subdirectory tells CMake to process the CMakeLists.txt in each folder.
# The core library is always built since both apps depend on it.
add_subdirectory(core)

if(RAWDGER_BUILD_JUCE)
    add_subdirectory(juce-app)
endif()

if(RAWDGER_BUILD_QT)
    # find_package searches for Qt6 on the system. REQUIRED means CMake will
    # error if Qt isn't found. COMPONENTS Core specifies which Qt module we need.
    # This is done at the top level (not inside qt-app/) so that CMAKE_PREFIX_PATH
    # (passed via -D on the command line) is properly consumed by CMake.
    # Unlike for juce, Qt needs to be installed on the system with, e.g., brew install qt.
    # Widgets includes Core and Gui transitively, so we only need to list Widgets.
    find_package(Qt6 REQUIRED COMPONENTS Widgets)
    add_subdirectory(qt-app)
endif()
