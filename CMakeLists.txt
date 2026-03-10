cmake_minimum_required(VERSION 3.24)
project(rawdger VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Options ---
option(RAWDGER_BUILD_JUCE "Build the JUCE app" ON)
option(RAWDGER_BUILD_QT   "Build the Qt app"   ON)

# --- Dependencies ---
include(FetchContent)

if(RAWDGER_BUILD_JUCE)
    FetchContent_Declare(juce
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG        8.0.4
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(juce)
endif()

# --- Subdirectories ---
add_subdirectory(core)

if(RAWDGER_BUILD_JUCE)
    add_subdirectory(juce-app)
endif()

if(RAWDGER_BUILD_QT)
    add_subdirectory(qt-app)
endif()
