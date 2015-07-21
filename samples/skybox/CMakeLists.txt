
file(GLOB SKYBOX_SRC src/*.cpp)


ADD_EXECUTABLE(skybox ${SKYBOX_SRC} )
TARGET_LINK_LIBRARIES(skybox ewa)

create_default_target_launcher(skybox ARGS ${CMAKE_SOURCE_DIR}  ${CMAKE_SOURCE_DIR}/samples/skybox  )
