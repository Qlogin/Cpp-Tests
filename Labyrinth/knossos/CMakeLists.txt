project(knossos)

set(TARGET_NAME ${PROJECT_NAME})

file(GLOB_RECURSE headers "include/*.h")

set(cpps
   src/labyrinth.cpp
)

add_library(${TARGET_NAME} STATIC
   ${headers}
   ${cpps}
)

target_include_directories(${TARGET_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(${TARGET_NAME}
   Boost::filesystem
)
