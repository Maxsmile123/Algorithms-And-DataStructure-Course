begin_task()
set_task_sources(condvar.hpp mutex.hpp)

# Condition Variable
add_task_test(cv_unit_tests tests/condvar/unit.cpp)
add_task_test(cv_stress_tests tests/condvar/stress.cpp)

# Mutex
add_task_test(mutex_unit_tests tests/mutex/unit.cpp)
add_task_test(mutex_stress_tests tests/mutex/stress.cpp)
add_task_test(mutex_futex_tests tests/mutex/futex.cpp)

# Event
add_task_test(event_stress_tests tests/event/stress.cpp)

end_task()
