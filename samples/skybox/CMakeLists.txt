
file(GLOB SKYBOX_SRC src/*.cpp)

#message("hi: ${GAME_SRC}")

ADD_EXECUTABLE(skybox ${SKYBOX_SRC} )
TARGET_LINK_LIBRARIES(skybox ewa)
