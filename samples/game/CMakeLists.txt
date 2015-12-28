

file(GLOB GAME_SRC src/*.cpp)

ADD_EXECUTABLE(game ${GAME_SRC} )
TARGET_LINK_LIBRARIES(game ewa)

create_default_target_launcher(game ARGS ${CMAKE_SOURCE_DIR}  ${CMAKE_SOURCE_DIR}/samples/game  )
