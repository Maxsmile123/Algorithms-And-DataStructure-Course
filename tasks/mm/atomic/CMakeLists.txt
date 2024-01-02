enable_language(ASM)

begin_task()

set_task_sources(
        atomic_ops.hpp
        atomic_ops.S
        atomic.hpp
        atomic.cpp)

add_task_test_dir(tests/stress stress_tests)

end_task()
