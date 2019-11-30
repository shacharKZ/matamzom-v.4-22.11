cmake_minimum_required(VERSION 3.9)
project(matamazom_project C)

set(CMAKE_C_STANDARD 99)


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Werror -pedantic-errors")
link_directories(.)

add_executable(self_test1 main_test1.c amount_set.c order.c order.h product.c)
add_executable(noa_test_for_AS main_test_noaAmountSet.c amount_set.c amount_set.h)
add_executable(AS_test_from_METARGELIM tests/amount_set_tests.h tests/amount_set_tests.c tests/amount_set_main.c amount_set.c amount_set.h)
add_executable(self_test2 main_test2.c amount_set.c order.c product.c order.c libmtm/set.h)
add_executable(self_test3 main_test3.c amount_set.c order.c product.c order.c matamazom.c)
target_link_libraries(self_test3 libmtm.a)





add_executable(self_test4 main_test4.c libmtm/set.h order.h order.c product.h product.c matamazom.h matamazom.c matamazom_print.h matamazom_print.c)
target_link_libraries(self_test4 libmtm.a)

add_executable(matamazom_test_from_METARGELIM tests/matamazom_main.c tests/matamazom_tests.h tests/matamazom_tests.c tests/test_utilities.h libmtm/set.h order.h order.c product.h product.c matamazom.h matamazom.c matamazom_print.h matamazom_print.c)
target_link_libraries(matamazom_test_from_METARGELIM libmtm.a)