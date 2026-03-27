# Create a standard executable target. Unlike JUCE, Qt doesn't need a special
# CMake function — a regular add_executable works. The find_package(Qt6) call
# in the top-level CMakeLists.txt makes Qt6::Widgets available here.
add_executable(rawdger-qt
    src/QtMain.cpp
)

# Mark as a GUI app on macOS and Windows:
#   MACOSX_BUNDLE — produces a .app bundle instead of a bare executable
#   WIN32_EXECUTABLE — links with WinMain so no console window pops up
set_target_properties(rawdger-qt PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Link libraries to this target.
#   - rawdger-core: our shared library (also brings in its PUBLIC include dirs)
#   - Qt6::Widgets: Qt's widgets module (buttons, windows, layouts, etc.).
#     This is an "imported target" created by find_package(Qt6) — it carries
#     its own include dirs, compile flags, and library paths automatically.
#     Widgets transitively includes Qt6::Core and Qt6::Gui.
target_link_libraries(rawdger-qt PRIVATE
    rawdger-core
    Qt6::Widgets
)
