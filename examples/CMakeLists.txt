find_package(mongocxx REQUIRED)

include_directories(
  ${MONGOCXX_INCLUDE_DIRS}
)

set(MONGOCXX_EXAMPLES
  insert_data.cpp
  find_or_query_data.cpp
)

foreach(EXAMPLE_SRC ${MONGOCXX_EXAMPLES})
  get_filename_component(EXAMPLE_TARGET ${EXAMPLE_SRC} NAME_WE)
  add_executable(${EXAMPLE_TARGET} ${EXAMPLE_SRC})
  target_link_libraries(${EXAMPLE_TARGET} ${MONGOCXX_LIBRARIES})
endforeach(EXAMPLE_SRC)

add_custom_target(examples DEPENDS ${MONGOCXX_EXAMPLES})
