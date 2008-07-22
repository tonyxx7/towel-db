include_directories(${TOWELDB_SOURCE_DIR}/include)
link_directories(${TOWELDB_SOURCE_DIR}/lib)

add_executable(test_db test_db.c)
target_link_libraries(test_db toweldb)

set_target_properties(test_db PROPERTIES COMPILE_FLAGS -Wall)
add_test(database test_db)
