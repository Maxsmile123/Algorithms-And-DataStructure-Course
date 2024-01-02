begin_task()

# Library
add_task_library(exe)

# Examples
add_task_dir_target(examples_executors examples/executors)
add_task_dir_target(examples_futures examples/futures)
add_task_dir_target(examples_fibers examples/fibers)

# Playground
add_task_playground(play)

# Tests

end_task()
