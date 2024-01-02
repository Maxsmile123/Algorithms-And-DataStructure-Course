begin_task()

task_link_libraries(asio)

add_task_library(thread_pool)
add_task_library(task)

add_task_test_dir(tests)

end_task()
