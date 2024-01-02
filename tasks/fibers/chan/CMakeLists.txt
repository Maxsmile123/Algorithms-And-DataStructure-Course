begin_task()

# Library
add_task_library(exe)

# Playground
add_task_playground(play)

# Workloads
add_task_dir_target(workloads workloads)

# Tests

# Coroutine
add_task_test_dir(tests/coroutine/standalone coroutine_tests)

# Executors

# Executors / Thread Pool
add_task_test_dir(tests/executors/thread_pool/unit thread_pool_unit_tests)
add_task_test_dir(tests/executors/thread_pool/stress thread_pool_stress_tests)

# Executors / Manual
add_task_test_dir(tests/executors/manual/unit manual_unit_tests)

# Fibers

# Fibers / Core

add_task_test_dir(tests/fibers/core/ fibers_core_tests)
add_task_test_dir(tests/fibers/racy fibers_racy_tests)

# Fibers / Sync

add_task_test_dir(tests/fibers/mutex/unit fibers_mutex_unit_tests)
add_task_test_dir(tests/fibers/mutex/stress fibers_mutex_stress_tests)

add_task_test_dir(tests/fibers/wait_group/unit fibers_wait_group_unit_tests)
add_task_test_dir(tests/fibers/wait_group/stress fibers_wait_group_stress_tests)

add_task_test_dir(tests/fibers/channel/unit fibers_channel_unit_tests)
add_task_test_dir(tests/fibers/channel/stress fibers_channel_stress_tests)

add_task_test_dir(tests/fibers/select/unit fibers_select_unit_tests)
add_task_test_dir(tests/fibers/select/stress fibers_select_stress_tests)

end_task()
