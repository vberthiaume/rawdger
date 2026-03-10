juce_add_console_app(rawdger-juce
    PRODUCT_NAME "Rawdger JUCE"
)

target_sources(rawdger-juce PRIVATE
    Source/Main.cpp
)

target_link_libraries(rawdger-juce PRIVATE
    rawdger-core
    juce::juce_core
    juce::juce_recommended_config_flags
)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(CURL REQUIRED IMPORTED_TARGET libcurl)
    target_link_libraries(rawdger-juce PRIVATE PkgConfig::CURL)
endif()
