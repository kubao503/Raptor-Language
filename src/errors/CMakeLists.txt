add_library(errors errors.cpp)

target_include_directories(errors INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(errors PUBLIC utils)
