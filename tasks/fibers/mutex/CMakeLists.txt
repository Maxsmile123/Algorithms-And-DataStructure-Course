begin_task()

# Library
add_task_library(exe)

# Playground
add_task_playground(play)

# Tests

# Coroutine
add_task_test(coro_unit_tests tests/coro/unit.cpp)

# Thread Pool
add_task_test(tp_unit_tests tests/executors/tp/unit.cpp)
add_task_test(tp_stress_tests tests/executors/tp/stress.cpp)

# Fibers

# Sched
add_task_test(fibers_sched_unit_tests tests/fibers/sched/unit.cpp)
add_task_test(fibers_sched_stress_tests tests/fibers/sched/stress.cpp)

# Sync

# Event
add_task_test(fibers_event_unit_tests tests/fibers/sync/event/unit.cpp)
add_task_test(fibers_event_stress_tests tests/fibers/sync/event/stress.cpp)
add_task_test(fibers_event_storage_tests tests/fibers/sync/event/storage.cpp)

# Mutex
add_task_test(fibers_mutex_unit_tests tests/fibers/sync/mutex/unit.cpp)
add_task_test(fibers_mutex_symm_transfer_tests tests/fibers/sync/mutex/symm_transfer.cpp)
add_task_test(fibers_mutex_stress_tests tests/fibers/sync/mutex/stress.cpp)

# WaitGroup
add_task_test(fibers_wg_unit_tests tests/fibers/sync/wait_group/unit.cpp)
add_task_test(fibers_wg_stress_tests tests/fibers/sync/wait_group/stress.cpp)
add_task_test(fibers_wg_storage_tests tests/fibers/sync/wait_group/storage.cpp)

end_task()
