cmake_minimum_required(VERSION 3.1)
project(psydapt)
set(CMAKE_CXX_STANDARD 17) # std::optional
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/modules/" ${CMAKE_MODULE_PATH})
add_subdirectory(ext)

option(PSYDAPT_BUILD_TESTS "Build unit tests" OFF)

add_library(psydapt INTERFACE)
target_link_libraries(psydapt INTERFACE xtensor)
target_include_directories(psydapt
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${xtensor_INCLUDE_DIRS}
)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

add_executable(Test test.cpp)
target_link_libraries(Test PUBLIC psydapt)

add_executable(XTensScratch xtensor_scratch.cpp)
target_link_libraries(XTensScratch PUBLIC psydapt)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND PSYDAPT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
