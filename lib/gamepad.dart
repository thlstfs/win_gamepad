import 'package:win_gamepad/win_gamepad.dart';

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
  late bool isConnected = false;
  late GamepadState state = GamepadState();
  late int deviceIndex = 0;

  Future<void> initialize({Function(GamepadState state)? onCallback}) async {
    var res;
    try {
      List<int> devices = await getAvaibleDevices();
      if (devices.isNotEmpty) {
        await selectGamepad(devices[0]);
      }
      res = await WinGamepad.initialize();
      isConnected = true;
    } on Exception {}
    WinGamepad.eventStream.listen((event) {
      update(Map<String, dynamic>.from(event));
      if (onCallback != null) {
        onCallback(state);
      }
      print(state);
    });
  }

  Future<List<int>> getAvaibleDevices() async {
    List<int> res;
    try {
      res = await WinGamepad.getAvaibleDevices;
      return res;
    } on Exception {
      return [];
    }
  }

  Future<bool> setAutoVibration(bool value) async {
    try {
      bool res = await WinGamepad.setAutoVibration(value);
      return res;
    } on Exception {
      return false;
    }
  }

  Future<bool> setVibration(
      double leftMotorSpeed, double rightMotorSpeed) async {
    try {
      var res = await WinGamepad.setVibration(leftMotorSpeed, rightMotorSpeed);
      return res;
    } on Exception {
      return false;
    }
  }

  Future<bool> selectGamepad(int index) async {
    try {
      var res = await WinGamepad.selectGamepad(index);
      deviceIndex = index;
      return res;
    } on Exception {
      return false;
    }
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
