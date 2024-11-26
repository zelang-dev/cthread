cmake_minimum_required(VERSION 2.8...3.14)

set(TARGET_LIST atomicity-test thread-tls thread-test_c11)
foreach (TARGET ${TARGET_LIST})
    add_executable(${TARGET} ${TARGET}.c )
    target_link_libraries(${TARGET} cthread)
endforeach()
