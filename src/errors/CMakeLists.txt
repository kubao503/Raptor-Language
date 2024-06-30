add_library(errors base_exception.cpp)

target_include_directories(errors INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(
    errors
    PUBLIC utils
)

add_library(
    semantic_errors
    semantic_errors.cpp
)

target_link_libraries(
    semantic_errors
    PRIVATE magic_enum::magic_enum
    PUBLIC errors
)