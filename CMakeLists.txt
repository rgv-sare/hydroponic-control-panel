cmake_minimum_required(VERSION 3.2)

project(hyrdoponic-control-panel)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory("dep/glad")
add_subdirectory("dep/glfw")

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory("dep/json")

#Add assimp
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL " " FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER ON  CACHE BOOL " " FORCE)
set(ASSIMP_BUILD_PLY_IMPORTER ON  CACHE BOOL " " FORCE)
set(ASSIMP_NO_EXPORT          ON  CACHE BOOL " " FORCE)
set(ASSIMP_BUILD_TESTS        OFF CACHE BOOL " " FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL " " FORCE)
set(ASSIMP_INSTALL            OFF CACHE BOOL " " FORCE)
set(BUILD_SHARED_LIBS         OFF CACHE BOOL " " FORCE)
add_subdirectory("dep/assimp")

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/Logger.cpp
    src/Inputs.cpp
    src/MeshBuilder.cpp
    src/Shaders.cpp
    src/UIRender.cpp
    src/FontRenderer.cpp
    src/stb_image.c
    src/Widget.cpp
    src/Button.cpp
    src/Viewport.cpp
    src/Images.cpp
    src/Mesh.cpp
    src/UIWindow.cpp
    src/Animation.cpp
    src/TextField.cpp

    src/hcp/Application.cpp
    src/hcp/StartMenu.cpp
    src/hcp/Resources.cpp
    src/hcp/SelectionWindow.cpp
    src/hcp/MainMenu.cpp
    src/hcp/RobotRenderer.cpp
)

target_link_libraries(${PROJECT_NAME}
    glfw
    glad
    assimp
    nlohmann_json::nlohmann_json
)

target_include_directories(${PROJECT_NAME} PRIVATE
    include
    dep/glm
    dep/glfw/include
    dep/glad/include
    dep/assimp/include
    dep/json/include
)

#Copy resource files to current binary folder
if(NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)

    file(GLOB_RECURSE resfiles RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "res/*")

    foreach(resfile ${resfiles})
    
        add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_CURRENT_SOURCE_DIR}/${resfile}"
            "${CMAKE_CURRENT_BINARY_DIR}/${resfile}"
        )

    endforeach(resfile)

    file(COPY res DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
    
endif()