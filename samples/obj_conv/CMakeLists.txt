
file(GLOB OBJ_CONV_SRC src/*.cpp deps/glad/src/*.c)

ADD_EXECUTABLE(obj_conv ${OBJ_CONV_SRC} )
TARGET_LINK_LIBRARIES(obj_conv ewa)

create_default_target_launcher(obj_conv ARGS ${CMAKE_SOURCE_DIR}  ${CMAKE_SOURCE_DIR}/samples/obj_conv  )
