# Runs one sample through sq and compares its output with the recorded one.
# Driven by add_test in the top-level CMakeLists; SQ, SCRIPT and EXPECTED come
# in as -D arguments.

EXECUTE_PROCESS(
  COMMAND ${SQ} ${SCRIPT} ${ARGS}
  OUTPUT_VARIABLE ACTUAL
  ERROR_VARIABLE ACTUAL_ERR
  RESULT_VARIABLE RC)

IF(NOT RC EQUAL 0)
  MESSAGE(FATAL_ERROR "sq exited ${RC} on ${SCRIPT}\n${ACTUAL}${ACTUAL_ERR}")
ENDIF()

FILE(READ ${EXPECTED} EXPECTED_TEXT)

# methcall.nut prints how long it took, which is the one thing here that is not
# a function of the VM's behaviour.
STRING(REGEX REPLACE "TIME=[^\n]*" "TIME=" ACTUAL "${ACTUAL}${ACTUAL_ERR}")
STRING(REGEX REPLACE "TIME=[^\n]*" "TIME=" EXPECTED_TEXT "${EXPECTED_TEXT}")

IF(NOT ACTUAL STREQUAL EXPECTED_TEXT)
  SET(GOT "${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}.actual")
  FILE(WRITE ${GOT} "${ACTUAL}")
  MESSAGE(FATAL_ERROR
    "${SCRIPT} no longer produces its recorded output.\n"
    "  recorded: ${EXPECTED}\n"
    "  produced: ${GOT}")
ENDIF()
