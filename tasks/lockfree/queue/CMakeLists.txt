begin_task()

add_task_library(hazard)

set_task_sources(
        lock_free_stack.hpp
        lock_free_queue.hpp)

# Hazard

# Queue

# Unit tests
add_task_test(queue_unit_tests tests/queue/unit.cpp)

# Stress tests
add_task_test(queue_stress_lf_tests tests/queue/stress/lf.cpp)
add_task_test(queue_stress_mm_tests tests/queue/stress/mm.cpp)

# Stack
add_task_test(stack_unit_tests tests/stack/unit.cpp)
add_task_test(stack_stress_tests tests/stack/stress.cpp)

end_task()
