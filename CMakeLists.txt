
cmake_minimum_required(VERSION 3.15)

project(paint)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# engine sub directory
add_subdirectory(external/engine)

# app
if (!APPLE)
    add_executable(main main.cpp src/application.cpp src/input_system.cpp)
else()
    # For macOS app bundle (optional) — create a .app so MoltenVK frameworks can be embedded
    add_executable(main MACOSX_BUNDLE main.cpp src/application.cpp src/input_system.cpp)
    set_target_properties(main PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "PaintBundle"
        RESOURCE "${CMAKE_SOURCE_DIR}/resources"
    )
endif()






# Include directories for headers
target_include_directories(main
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(main PRIVATE engine::engine)
# target_link_libraries(game PRIVATE engine::graphics) # link only part of engine