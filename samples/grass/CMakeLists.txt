
file(GLOB GRASS_SRC src/*.cpp)

ADD_EXECUTABLE(grass ${GRASS_SRC} )
TARGET_LINK_LIBRARIES(grass ewa)

create_default_target_launcher(grass ARGS ${CMAKE_SOURCE_DIR}  ${CMAKE_SOURCE_DIR}/samples/grass  )
