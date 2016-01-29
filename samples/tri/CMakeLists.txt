
file(GLOB TRI_SRC src/*.cpp)

ADD_EXECUTABLE(tri ${TRI_SRC} )
TARGET_LINK_LIBRARIES(tri ewa)

create_default_target_launcher(tri ARGS ${CMAKE_SOURCE_DIR}  ${CMAKE_SOURCE_DIR}/samples/tri  )
