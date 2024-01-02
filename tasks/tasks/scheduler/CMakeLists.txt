begin_task()

# Dependencies

# https://github.com/cameron314/concurrentqueue
task_link_libraries(concurrentqueue)

# Library
add_task_library(exe)

# Playground
add_task_playground(play)

# Workloads
add_task_dir_target(workloads workloads)

# Tests

# Wait group
add_task_test(wg_unit_tests tests/threads/wait_group/unit.cpp)
add_task_test(wg_stress_tests tests/threads/wait_group/stress.cpp)
add_task_test(wg_storage_tests tests/threads/wait_group/storage.cpp)
add_task_test(wg_futex_tests tests/threads/wait_group/futex.cpp)

# Executors

# Thread Pool

add_task_test(tp_unit_tests tests/executors/thread_pool/just_works/unit.cpp)
add_task_test(tp_stress_tests tests/executors/thread_pool/just_works/stress.cpp)

# WaitIdle
add_task_test(tp_wait_idle_unit_tests tests/executors/thread_pool/wait_idle/unit.cpp)
add_task_test(tp_wait_idle_stress_tests tests/executors/thread_pool/wait_idle/stress.cpp)

# Balancing
add_task_test(tp_balancing_stress_tests tests/executors/thread_pool/balancing/stress.cpp)

# Parking
add_task_test(tp_parking_tests tests/executors/thread_pool/parking/futex.cpp)

# Manual
add_task_test(manual_unit_tests tests/executors/manual/unit.cpp)

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
add_task_test(fibers_mutex_stress_tests tests/fibers/sync/mutex/stress.cpp)
add_task_test(fibers_mutex_symm_transfer_tests tests/fibers/sync/mutex/symm_transfer.cpp)

# WaitGroup
add_task_test(fibers_wg_unit_tests tests/fibers/sync/wait_group/unit.cpp)
add_task_test(fibers_wg_stress_tests tests/fibers/sync/wait_group/stress.cpp)
add_task_test(fibers_wg_storage_tests tests/fibers/sync/wait_group/storage.cpp)

# Mixed
add_task_test(mixed_stress_tests tests/mixed/stress.cpp)

end_task()
