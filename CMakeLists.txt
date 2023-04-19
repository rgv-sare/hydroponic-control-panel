cmake_minimum_required(VERSION 3.2)

project(hyrdoponic-control-panel)

add_subdirectory("dep/glad")
add_subdirectory("dep/glfw")

add_executable(${PROJECT_NAME}
    main.cpp
    Logger.cpp
)

target_link_libraries(${PROJECT_NAME}
    glfw
    glad
)

target_include_directories(${PROJECT_NAME} PRIVATE
    include
    dep/glm
    dep/glfw/include
    dep/glad/include
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