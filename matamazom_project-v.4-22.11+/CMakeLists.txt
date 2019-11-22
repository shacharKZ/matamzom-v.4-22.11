cmake_minimum_required(VERSION 3.15)
project(untitled C)

set(CMAKE_C_STANDARD 99)

add_executable(self_test1 main_test1.c amount_set.c amount_set.h order.c order.h product.c product.h)
add_executable(amount_set_tests_from_METARGELIM amount_set.c amount_set.h tests/amount_set_main.c tests/amount_set_tests.c tests/amount_set_tests.h)
add_executable(self_test2 main_test2.c amount_set.c amount_set.h order.c order.h product.c product.h order.c order.h)
