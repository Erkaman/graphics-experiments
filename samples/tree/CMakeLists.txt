
file(GLOB TREE_SRC src/*.cpp)

ADD_EXECUTABLE(tree ${TREE_SRC} )
TARGET_LINK_LIBRARIES(tree ewa)


create_default_target_launcher(tree ARGS ${CMAKE_SOURCE_DIR}  ${CMAKE_SOURCE_DIR}/samples/tree  )
