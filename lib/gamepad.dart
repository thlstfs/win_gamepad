enum GamepadButton {
  a,
  b,
  x,
  y,
  dpadUp,
  dpadDown,
  dpadLeft,
  dpadRight,
  back,
  start,
  leftThumb,
  rightThumb,
  leftShoulder,
  rightShoulder,
}

class Gamepad {
  late bool isConnected;
  late GamepadState state;
  Gamepad() {
    isConnected = false;
    state = GamepadState();
  }
}

class GamepadState {
  // double leftThumbX;
  // double leftThumbY;
  // double rightThumbX;
  // double rightThumbY;
  // double leftTrigger;
  // double rightTrigger;
}
