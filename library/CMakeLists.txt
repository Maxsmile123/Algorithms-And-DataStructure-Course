include(FetchContent)

# --------------------------------------------------------------------

# Offline mode (uncomment next line to enable)
# set(FETCHCONTENT_FULLY_DISCONNECTED ON)

# set(FETCHCONTENT_QUIET OFF)

# --------------------------------------------------------------------

# Libraries

# --------------------------------------------------------------------

# fmt with println

project_log("FetchContent: fmt")

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 87c066a35b7cc70bb7d438a543c8b49b577e61f4
)
FetchContent_MakeAvailable(fmt)

# --------------------------------------------------------------------

# Google test

project_log("FetchContent: Google test")

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG main
)
FetchContent_MakeAvailable(googletest)

# --------------------------------------------------------------------

# Memory allocation

project_log("FetchContent: mimalloc")

FetchContent_Declare(
        mimalloc
        GIT_REPOSITORY https://github.com/microsoft/mimalloc
        GIT_TAG master
)
FetchContent_MakeAvailable(mimalloc)


