enum GamepadButton {
  dpadUp,
  dpadDown,
  dpadLeft,
  dpadRight,
  start,
  back,
  leftThumb,
  rightThumb,
  leftShoulder,
  rightShoulder,
  a,
  b,
  x,
  y,
}

class Gamepad {
  late bool isConnected;
  late GamepadState state;
  Gamepad() {
    isConnected = false;
    state = GamepadState();
  }

  void update(Map<String, dynamic> map) {
    state.update(map);
  }

  @override
  String toString() {
    return state.toString();
  }
}

class GamepadState {
  int leftThumbX;
  int leftThumbY;
  int rightThumbX;
  int rightThumbY;
  int leftTrigger;
  int rightTrigger;
  late List<GamepadButton> buttons;

  GamepadState({
    this.leftThumbX = 0,
    this.leftThumbY = 0,
    this.leftTrigger = 0,
    this.rightThumbX = 0,
    this.rightThumbY = 0,
    this.rightTrigger = 0,
  }) {
    buttons = <GamepadButton>[];
  }

  void update(Map<String, dynamic> map) {
    leftThumbX = map['thumbs'][0];
    leftThumbY = map['thumbs'][1];
    rightThumbX = map['thumbs'][2];
    rightThumbY = map['thumbs'][3];

    leftTrigger = map['triggers'][0];
    rightTrigger = map['triggers'][1];

    buttons = List.generate((map['buttons'] as List).length,
        (index) => GamepadButton.values[map['buttons'][index] as int]);
  }

  bool isPressed(GamepadButton button) {
    return buttons.any((element) => element == button);
  }

  @override
  String toString() {
    String buttons = this.buttons.toString();
    return "[$leftThumbX, $leftThumbY, $rightThumbX, $rightThumbY], [$leftTrigger, $rightTrigger] $buttons";
  }
}
