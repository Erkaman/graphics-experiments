
file(GLOB GAME_ENGINE_SRC src/*.cpp)

ADD_LIBRARY(game_engine ${GAME_ENGINE_SRC} )
TARGET_LINK_LIBRARIES(game_engine ewa)

create_default_target_launcher(game_engine ARGS ${CMAKE_SOURCE_DIR}  ${CMAKE_SOURCE_DIR}/samples/game  )
