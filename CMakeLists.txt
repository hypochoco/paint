
cmake_minimum_required(VERSION 3.15)

project(paint)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_PREFIX_PATH "/usr/local/Qt-6.10.1" CACHE PATH "Qt installation path")

# engine sub directory
add_subdirectory(external/engine)

# app
if (!APPLE)
    add_executable(main main.cpp src/application.cpp src/input_system.cpp)
else()
    # For macOS app bundle (optional) — create a .app so MoltenVK frameworks can be embedded
    add_executable(main MACOSX_BUNDLE main.cpp)
    set_target_properties(main PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "PaintBundle"
        RESOURCE "${CMAKE_SOURCE_DIR}/resources"
    )
endif()

# glob headers and src
file(GLOB_RECURSE MY_SOURCES src/*.cpp include/*.h)
target_sources(main PRIVATE ${MY_SOURCES})
target_include_directories(main
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# engine
target_link_libraries(main PRIVATE engine::engine)

# qt
set_target_properties(main PROPERTIES AUTOMOC ON)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
target_link_libraries(main PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)
