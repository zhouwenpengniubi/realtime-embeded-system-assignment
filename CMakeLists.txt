cmake_minimum_required(VERSION 3.10.0)

project(gomoku_robot VERSION 0.1.0 LANGUAGES C CXX)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/arm)

# Main
add_executable(gomoku_robot
    src/main.cpp
)

# Test (optional)
option(BUILD_TESTS "Build test programs" ON)
if(BUILD_TESTS)
    # add_executable(test_cam
    # tests/test_cam.cpp)

    # find_package(OpenCV REQUIRED)
    # target_link_libraries(test_cam ${OpenCV_LIBS})

    add_executable(test_pwm
    tests/test_pwm.cpp
)
endif()