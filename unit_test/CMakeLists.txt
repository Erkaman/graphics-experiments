
file(GLOB TEST_SRC src/*.cpp)

ADD_EXECUTABLE(unit_test ${TEST_SRC} )
TARGET_LINK_LIBRARIES(unit_test ewa)