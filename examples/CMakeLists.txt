find_package(mongocxx REQUIRED)

include_directories(
  ${MONGOCXX_INCLUDE_DIRS}
)

set(MONGOCXX_EXAMPLES insert)

foreach(EXAMPLE ${MONGOCXX_EXAMPLES})
  add_executable(${EXAMPLE} ${EXAMPLE}.cpp)
  target_link_libraries(${EXAMPLE} ${MONGOCXX_LIBRARIES})
endforeach(EXAMPLE)

add_custom_target(examples DEPENDS ${MONGOCXX_EXAMPLES})
