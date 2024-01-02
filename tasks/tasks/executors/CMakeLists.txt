begin_task()

# Library
add_task_library(exe)

# Playground
add_task_playground(play)

# Tests

# Thread Pool
add_task_test(tp_unit_tests tests/executors/thread_pool/unit.cpp)
add_task_test(tp_stress_tests tests/executors/thread_pool/stress.cpp)

# Manual
add_task_test(manual_unit_tests tests/executors/manual/unit.cpp)

# Strand
add_task_test(strand_unit_tests tests/executors/strand/unit.cpp)
add_task_test(strand_stress_tests tests/executors/strand/stress.cpp)
add_task_test(strand_mo_tests tests/executors/strand/mo.cpp)
add_task_test(strand_lifetime_tests tests/executors/strand/lifetime.cpp)

end_task()
