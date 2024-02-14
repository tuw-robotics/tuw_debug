add_executable(mx_demo_joystick demo.cpp)
target_link_libraries(mx_demo_joystick mx_joystick)


install(TARGETS mx_demo_joystick
  # IMPORTANT: Add the joystick executable to the "export-set"
  EXPORT MXJoystickTargets
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin)
  
