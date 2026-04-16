file(GLOB SOURCES "*.cpp")
add_executable(main ${SOURCES})
target_include_directories(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
