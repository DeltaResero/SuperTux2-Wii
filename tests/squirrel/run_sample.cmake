# Runs one sample through sq and compares what it prints with the recorded output

execute_process(
  COMMAND ${SQ} ${SCRIPT} ${ARGS}
  OUTPUT_VARIABLE ACTUAL
  ERROR_VARIABLE ACTUAL_ERR
  RESULT_VARIABLE RC)

if(NOT RC EQUAL 0)
  message(FATAL_ERROR "sq exited ${RC} on ${SCRIPT}\n${ACTUAL}${ACTUAL_ERR}")
endif()

file(READ ${EXPECTED} EXPECTED_TEXT)

# methcall.nut prints how long it took, the one line that isn't the VM's behaviour
string(REGEX REPLACE "TIME=[^\n]*" "TIME=" ACTUAL "${ACTUAL}${ACTUAL_ERR}")
string(REGEX REPLACE "TIME=[^\n]*" "TIME=" EXPECTED_TEXT "${EXPECTED_TEXT}")

if(NOT ACTUAL STREQUAL EXPECTED_TEXT)
  set(GOT "${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}.actual")
  file(WRITE ${GOT} "${ACTUAL}")
  message(FATAL_ERROR
    "${SCRIPT} no longer produces its recorded output.\n"
    "  recorded: ${EXPECTED}\n"
    "  produced: ${GOT}")
endif()
