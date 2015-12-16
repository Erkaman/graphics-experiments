
file(GLOB GAME_SRC src/*.cpp)

ADD_EXECUTABLE(game ${GAME_SRC} )
TARGET_LINK_LIBRARIES(game game_engine ewa)

include_directories("../game_engine/src" )

create_default_target_launcher(game ARGS ${CMAKE_SOURCE_DIR}  ${CMAKE_SOURCE_DIR}/samples/game  )
