# Create a standard executable target. Unlike JUCE, Qt doesn't need a special
# CMake function — a regular add_executable works. The find_package(Qt6) call
# in the top-level CMakeLists.txt makes Qt6::Core available here.
add_executable(rawdger-qt
    src/main.cpp
)

# Link libraries to this target.
#   - rawdger-core: our shared library (also brings in its PUBLIC include dirs)
#   - Qt6::Core: Qt's core module (strings, event loop, debug output, etc.).
#     This is an "imported target" created by find_package(Qt6) — it carries
#     its own include dirs, compile flags, and library paths automatically.
target_link_libraries(rawdger-qt PRIVATE
    rawdger-core
    Qt6::Core
)
