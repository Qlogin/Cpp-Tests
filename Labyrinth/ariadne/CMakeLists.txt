project(ariadne)

set(TARGET_NAME ${PROJECT_NAME})

file(GLOB_RECURSE headers "*.h")

add_executable(${TARGET_NAME}
   main.cpp
   arguments.cpp
   load_sections.cpp
   ${headers}
)

target_link_libraries(${TARGET_NAME}
   LINK_PRIVATE
      knossos
      Boost::system
      Boost::filesystem
      Boost::program_options
      ${BOOST_LINKING}
)

install(TARGETS ${TARGET_NAME} RUNTIME DESTINATION bin)
