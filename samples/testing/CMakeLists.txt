
file(GLOB TESTING_SRC src/*.cpp)

ADD_EXECUTABLE(testing ${TESTING_SRC} )
TARGET_LINK_LIBRARIES(testing ewa)

create_default_target_launcher(testing ARGS ${CMAKE_SOURCE_DIR}  ${CMAKE_SOURCE_DIR}/samples/testing  )
