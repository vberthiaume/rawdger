add_library(rawdger-core STATIC
    src/Core.cpp
)

target_include_directories(rawdger-core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
