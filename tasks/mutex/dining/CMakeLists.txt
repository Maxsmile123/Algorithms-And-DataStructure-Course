begin_task()
set_task_sources(
        table.cpp
        table.hpp
        philosopher.cpp
        philosopher.hpp)
add_task_test(stress_tests stress_tests.cpp)
end_task()
