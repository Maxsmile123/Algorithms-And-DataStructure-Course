if(NOT DEFINED UNIX)
    set(UNIX false)
endif()
if(NOT DEFINED APPLE)
    set(APPLE false)
endif()

set(LINUX false)
if(DEFINED UNIX AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(LINUX true)
endif()

add_definitions(-DUNIX=${UNIX})
add_definitions(-DAPPLE=${APPLE})
add_definitions(-DLINUX=${LINUX})

