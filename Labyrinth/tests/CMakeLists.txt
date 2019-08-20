project(tests)

find_package(Boost QUIET
   COMPONENTS
      unit_test_framework
)

if (Boost_UNIT_TEST_FRAMEWORK_FOUND)
   message(STATUS "Found: unit_test_framework")
else()
   message(FATAL_ERROR "Boost.Test not found! Turn off BUILD_TESTING option")
endif()

add_executable(tests test1.cpp)
target_link_libraries(tests
   knossos
   Boost::unit_test_framework
)

add_test(NAME    TestKnossos
         COMMAND tests --log_level=test_suite
)
add_test(NAME    TestAriadne
         COMMAND ariadne --board ${CMAKE_CURRENT_SOURCE_DIR}/board.txt
                         --route "rrldd" -x 0 -y 0 -o test_output.txt
)
add_test(NAME    TestAriadneOutput
         COMMAND ${CMAKE_COMMAND} -E compare_files
                 test_output.txt ${CMAKE_CURRENT_SOURCE_DIR}/etalon.txt
)
