begin_task()

# Library
add_task_library(exe)

# Playground
add_task_playground(play)

# Tests

# Coroutine
add_task_test(coro_unit_tests tests/coro/unit.cpp)

# Thread Pool
add_task_test(tp_unit_tests tests/tp/unit.cpp)
add_task_test(tp_stress_tests tests/tp/stress.cpp)

# Fibers
add_task_test(fibers_sched_unit_tests tests/fibers/sched/unit.cpp)
add_task_test(fibers_sched_stress_tests tests/fibers/sched/stress.cpp)

end_task()
