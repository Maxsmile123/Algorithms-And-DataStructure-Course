begin_task()

# Library
add_task_library(exe)

# Playground
add_task_playground(play)

# Tests

# Executors / Thread Pool
add_task_test_dir(tests/executors/thread_pool/unit thread_pool_unit_tests)
add_task_test_dir(tests/executors/thread_pool/stress thread_pool_stress_tests)

# Executors / Manual
add_task_test_dir(tests/executors/manual/unit manual_unit_tests)

# Tasks

# Tasks / Core
add_task_test_dir(tests/tasks/core/unit tasks_core_unit_tests)

# Tasks / Mutex
add_task_test_dir(tests/tasks/mutex/unit tasks_mutex_unit_tests)
add_task_test_dir(tests/tasks/mutex/stress tasks_mutex_stress_tests)

# Tasks / WaitGroup
add_task_test_dir(tests/tasks/wait_group/unit tasks_wait_group_unit_tests)
add_task_test_dir(tests/tasks/wait_group/stress tasks_wait_group_stress_tests)

end_task()
