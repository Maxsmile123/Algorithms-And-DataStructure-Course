begin_task()

set_task_sources(wait_group.hpp)

# Tests
add_task_test(unit_tests tests/unit.cpp)
add_task_test(stress_tests tests/stress.cpp)
add_task_test(futex_tests tests/futex.cpp)
add_task_test(storage_tests tests/storage.cpp)

end_task()
