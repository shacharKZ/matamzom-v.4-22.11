cmake_minimum_required(VERSION 3.9)
project(matamazom_project C)

set(CMAKE_C_STANDARD 99)


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Werror -pedantic-errors")
link_directories(.)

add_executable(self_test1 main_test1.c amount_set.c amount_set.h order.c order.h product.c product.h libmtm/set.h)
add_executable(amount_set_tests_from_METARGELIM amount_set.c amount_set.h tests/amount_set_main.c tests/amount_set_tests.c tests/amount_set_tests.h libmtm/set.h ../main_test5.c)
target_link_libraries(amount_set_tests_from_METARGELIM libmtm.a)
add_executable(self_test2 main_test2.c amount_set.c amount_set.h order.c order.h product.c product.h order.c order.h libmtm/set.h)
add_executable(self_test3 main_test3.c amount_set.c amount_set.h order.c order.h product.c product.h order.c order.h matamazom.c matamazom.h libmtm/set.h  )
target_link_libraries(self_test3 libmtm.a)


add_executable(self_test4 main_test4.c libmtm/set.h )
target_link_libraries(self_test4 libmtm.a)
