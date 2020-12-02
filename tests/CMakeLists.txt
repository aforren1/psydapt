cmake_minimum_required(VERSION 3.4)

project(psydapt_tests CXX)

find_package(Corrade REQUIRED TestSuite)
set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)

enable_testing()
corrade_add_test(Staircase test_staircase.cpp LIBRARIES psydapt)
