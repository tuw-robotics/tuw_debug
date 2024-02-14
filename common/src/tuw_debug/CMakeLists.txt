configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h" @ONLY)

add_library(mx_joystick joystick.cpp)

install(TARGETS mx_joystick
  # IMPORTANT: Add the joystick library to the "export-set"
  EXPORT MXJoystickTargets
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/mx"
    COMPONENT dev)
