begin_task()

set_task_sources(work_stealing_queue.hpp)

# Unit tests
add_task_test_dir(tests/unit unit_tests)

# Stress tests
add_task_test_dir(tests/stress stress_tests)

end_task()
