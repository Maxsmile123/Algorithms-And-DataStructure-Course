begin_task()

set_task_sources(
        atomic_stamped_ptr.hpp
        shared_ptr.hpp
        lock_free_stack.hpp)

# Tests

# Shared ptr
add_task_test(shared_ptr_unit_tests tests/shared_ptr/unit.cpp)
add_task_test(shared_ptr_stress_tests tests/shared_ptr/stress.cpp)

# Stack
add_task_test(stack_unit_tests tests/stack/unit.cpp)
add_task_test(stack_stress_tests tests/stack/stress.cpp)

end_task()
