cmake_minimum_required(VERSION 3.14)

project(AlgorithmsAndDataStructure-Course)

include(cmake/Logging.cmake)
include(cmake/Ccache.cmake)
include(cmake/Helpers.cmake)
include(cmake/CheckCompiler.cmake)
include(cmake/CompileOptions.cmake)
include(cmake/Sanitize.cmake)
include(cmake/Platform.cmake)
include(cmake/Development.cmake)

add_subdirectory(library)
# This must be included after library so that
# third party code is not affected
include(cmake/Werror.cmake)
include(cmake/Task.cmake)

add_subdirectory(tasks)
