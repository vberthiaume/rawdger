juce_add_console_app(rawdger-juce
    PRODUCT_NAME "Rawdger JUCE"
)

target_sources(rawdger-juce PRIVATE
    Source/Main.cpp
)

target_link_libraries(rawdger-juce PRIVATE
    rawdger-core
    juce::juce_core
)
