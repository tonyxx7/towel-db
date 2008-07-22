set(Source database.c record.c parser.c)

include_directories(${TOWELDB_SOURCE_DIR}/include)
add_library(toweldb SHARED ${Source} )
set_target_properties(toweldb PROPERTIES COMPILE_FLAGS -Wall)
install(TARGETS toweldb DESTINATION lib)
