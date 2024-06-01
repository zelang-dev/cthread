cmake_minimum_required(VERSION 2.8...3.14)

set(TARGET_LIST thread-test_c11 pool-test_heavy pool-test_shutdown pool-test_wait )
foreach (TARGET ${TARGET_LIST})
    add_executable(${TARGET} ${TARGET}.c )
    target_link_libraries(${TARGET} cthread)
endforeach()
