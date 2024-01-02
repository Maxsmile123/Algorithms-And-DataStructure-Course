begin_task()

add_task_library(stdlike)

# Tests
add_task_test(unit_tests tests/unit.cpp)
add_task_test(confuse_tests tests/confuse.cpp)
add_task_test(stress_tests tests/stress.cpp)

add_task_playground(play)
end_task()
