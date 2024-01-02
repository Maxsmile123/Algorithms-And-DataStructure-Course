begin_task()

set_task_sources(coroutine.hpp coroutine.cpp)

# Playground
add_task_playground(play)

# Tests
add_task_test(unit_tests tests/unit.cpp)

end_task()
