project(test-system-interface)

set(CMAKE_CXX_STANDARD 17)

add_executable(test-system-interface
    main.cpp

    ../../src/hcp/Serial.cpp
    ../../src/hcp/Serial_Windows.cpp
    ../../src/hcp/Serial_Linux.cpp
    ../../src/hcp/SystemInterface.cpp

    ../../src/Logger.cpp
    ../../src/Animation.cpp
)

set(LIBS glfw)

#Link Setupapi on windows
if (WIN32)
    list(APPEND LIBS "Setupapi")
endif()

target_link_libraries(test-system-interface ${LIBS})

target_include_directories(test-system-interface PRIVATE
    ../../include
    ../../dep/glm
)