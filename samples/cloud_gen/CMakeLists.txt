
file(GLOB CLOUD_GEN_SRC src/*.cpp)

ADD_EXECUTABLE(cloud_gen ${CLOUD_GEN_SRC} )
TARGET_LINK_LIBRARIES(cloud_gen ewa)

create_default_target_launcher(cloud_gen ARGS ${CMAKE_SOURCE_DIR}  ${CMAKE_SOURCE_DIR}/samples/cloud_gen  )
