add_executable(rawdger-qt
    src/main.cpp
)

target_link_libraries(rawdger-qt PRIVATE
    rawdger-core
    Qt6::Core
)
