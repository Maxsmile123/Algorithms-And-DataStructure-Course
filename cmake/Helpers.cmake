# Helper functions

# Prepend ${PREFIX} to all arguments and put them in ${VAR}
# Usage: prepend(FILES "${DIR}/" ${NAMES})

function(prepend VAR PREFIX)
    set(LIST_VAR "")
    foreach(ELEM ${ARGN})
        list(APPEND LIST_VAR "${PREFIX}${ELEM}")
    endforeach()
    set(${VAR} "${LIST_VAR}" PARENT_SCOPE)
endfunction()

# Add ordered run target
# Usage: run_chain("all_tests" ${TESTS})

function(run_chain NAME)
    set(BINARIES ${ARGN})

    set(PREV_LINK_TARGET ${NAME}_init)

    add_custom_target(${NAME}_init true)

    foreach(BIN ${BINARIES})
        set(LINK_TARGET "${NAME}_run_${BIN}")

        add_custom_target(${LINK_TARGET} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${BIN})
        add_dependencies(${LINK_TARGET} ${PREV_LINK_TARGET} ${BIN})

        set(PREV_LINK_TARGET ${LINK_TARGET})
    endforeach()

    add_custom_target(${NAME})
    add_dependencies(${NAME} ${PREV_LINK_TARGET})
endfunction()
