begin_task()

add_task_library(stdlike)
add_task_library(tp)

# Queue
add_task_test(queue_unit_tests tests/queue/unit.cpp)
add_task_test(queue_stress_tests tests/queue/stress.cpp)

# ThreadPool
add_task_test(tp_unit_tests tests/thread_pool/unit.cpp)
add_task_test(tp_stress_tests tests/thread_pool/stress.cpp)

add_task_playground(play)

end_task()
