begin_task()

# Library
add_task_library(exe)

# Playground
add_task_playground(play)

# Tests

# Wait group
add_task_test(wg_unit_tests tests/threads/wait_group/unit.cpp)
add_task_test(wg_stress_tests tests/threads/wait_group/stress.cpp)
add_task_test(wg_storage_tests tests/threads/wait_group/storage.cpp)
add_task_test(wg_futex_tests tests/threads/wait_group/futex.cpp)

# Executors / Thread Pool
add_task_test(tp_unit_tests tests/executors/thread_pool/unit.cpp)
add_task_test(tp_stress_tests tests/executors/thread_pool/stress.cpp)

# Executors / Manual
add_task_test(manual_unit_tests tests/executors/manual/unit.cpp)

# Futures
add_task_test(futures_unit_tests tests/futures/unit.cpp)
add_task_test(futures_lazy_tests tests/futures/lazy.cpp)
add_task_test(futures_stress_tests tests/futures/stress.cpp)
add_task_test_dir(tests/futures/alloc futures_alloc_tests)

end_task()
