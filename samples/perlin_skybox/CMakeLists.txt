
file(GLOB PERLIN_SKYBOX_SRC src/*.cpp)

ADD_EXECUTABLE(perlin_skybox ${PERLIN_SKYBOX_SRC} )
TARGET_LINK_LIBRARIES(perlin_skybox ewa)

create_default_target_launcher(perlin_skybox ARGS ${CMAKE_SOURCE_DIR}  ${CMAKE_SOURCE_DIR}/samples/perlin_skybox  )
