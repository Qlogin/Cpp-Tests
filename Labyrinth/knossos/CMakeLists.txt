project(knossos)

set(TARGET_NAME ${PROJECT_NAME})

file(GLOB_RECURSE headers "include/*.h")

set(cpps
   src/labyrinth.cpp
)

# Type is specified by BUILD_SHARED_LIBS option
add_library(${TARGET_NAME}
   ${headers}
   ${cpps}
)

# Generate export header
#   (realy needed only for shared library)
include(GenerateExportHeader)
generate_export_header(${TARGET_NAME}
  EXPORT_FILE_NAME ${TARGET_NAME}/export.h
)

# Paths to public headers and generated export header
target_include_directories(${TARGET_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(${TARGET_NAME}
   Boost::filesystem
   Boost::dynamic_linking
)

# Install public headers
install(DIRECTORY include/ DESTINATION include)

# Install generated export header
install(FILES $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}/export.h>
        DESTINATION include/${TARGET_NAME})

# Install library
install(TARGETS ${TARGET_NAME}
        EXPORT export_knossos
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static
)
install(EXPORT export_knossos DESTINATION share/knossos)
