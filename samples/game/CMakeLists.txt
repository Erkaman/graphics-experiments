
file(GLOB GAME_SRC src/*.cpp)

#message("hi: ${GAME_SRC}")

ADD_EXECUTABLE(game ${GAME_SRC} )
TARGET_LINK_LIBRARIES(game ewa)
