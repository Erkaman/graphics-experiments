
file(GLOB CELLULAR_SRC src/*.cpp)

ADD_EXECUTABLE(cellular ${CELLULAR_SRC} )
TARGET_LINK_LIBRARIES(cellular ewa)

create_default_target_launcher(cellular ARGS ${CMAKE_SOURCE_DIR}  ${CMAKE_SOURCE_DIR}/samples/cellular  )
