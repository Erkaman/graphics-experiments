

file(GLOB AO_SRC src/*.cpp)

ADD_EXECUTABLE(ao ${AO_SRC} )
TARGET_LINK_LIBRARIES(ao ewa)

create_default_target_launcher(ao ARGS ${CMAKE_SOURCE_DIR}  ${CMAKE_SOURCE_DIR}/samples/ao  )
