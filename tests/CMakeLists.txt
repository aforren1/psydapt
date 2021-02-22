cmake_minimum_required(VERSION 3.4)

project(psydapt_tests CXX)

find_package(Corrade REQUIRED TestSuite)
set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)

corrade_add_test(Staircase test_staircase.cpp common.cpp LIBRARIES psydapt)
corrade_add_test(QPWeibull test_qp_weibull.cpp LIBRARIES psydapt)
corrade_add_test(QPCSF test_qp_csf.cpp LIBRARIES psydapt)
corrade_add_test(Broadcast test_broadcast.cpp LIBRARIES xtensor)
