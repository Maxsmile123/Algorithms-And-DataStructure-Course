begin_task()

set_task_sources(
        semaphore.hpp
        tagged_semaphore.hpp
        queue.hpp)

add_task_test(unit_tests tests/unit.cpp)
add_task_test(stress_tests tests/stress.cpp)

end_task()
