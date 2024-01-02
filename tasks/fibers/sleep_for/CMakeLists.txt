begin_task()

# Dependencies
task_link_libraries(asio)

# Library
add_task_library(exe)

# Playground
add_task_playground(play)

# Tests

# Coroutine
add_task_test(coro_unit_tests tests/coro/unit.cpp)

# Fibers

# Yield
add_task_test(fibers_yield_unit_tests tests/fibers/yield/unit.cpp)
add_task_test(fibers_yield_stress_tests tests/fibers/yield/stress.cpp)

# SleepFor

# One thread
add_task_test(fibers_sleep_for_1t_unit_tests tests/fibers/sleep_for/1t/unit.cpp)
add_task_test(fibers_sleep_for_1t_stress_tests tests/fibers/sleep_for/1t/stress.cpp)

# Multi Thread
add_task_test(fibers_sleep_for_mt_unit_tests tests/fibers/sleep_for/mt/unit.cpp)
add_task_test(fibers_sleep_for_mt_stress_tests tests/fibers/sleep_for/mt/stress.cpp)

end_task()
