begin_task()

set_task_sources(
        atomic_stamped_ptr.hpp
        lock_free_stack.hpp)

# Playground
add_task_playground(play)

# Unit tests
add_task_test(unit_tests tests/unit.cpp)

# Stress tests
add_task_test(stress_tests tests/stress.cpp)

end_task()
