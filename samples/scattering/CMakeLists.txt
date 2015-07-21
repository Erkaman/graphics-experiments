
file(GLOB SCATTERING_SRC src/*.cpp)

ADD_EXECUTABLE(scattering ${SCATTERING_SRC} )
TARGET_LINK_LIBRARIES(scattering ewa)

create_default_target_launcher(scattering ARGS ${CMAKE_SOURCE_DIR}  ${CMAKE_SOURCE_DIR}/samples/scattering  )

