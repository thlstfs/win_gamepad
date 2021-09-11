# win_gamepad

A Flutter plugin for Windows for receiving input from game controllers which support XInput. This plugin uses XInput Game Controller API for receiving input from game controllers. 

## Installation

To use this plugin, add `win_gamepad` as a dependency in your pubspec.yaml file.

## Usage

Import it
``` dart
import 'package:win_gamepad/gamepad.dart';
```
Initialize gamepad
``` dart
// default id = 0
// default auto vibration = false
Gamepad gamepad = Gamepad();
@override
void initState() {
    super.initState();
    // initialize gamepad and selects first available device or 0
    gamepad.initialize(onCallback: (gamepadState) {
        print(gamepadState);
        setState(() {});
    });
}
```
Get available gamepad ids
``` dart
// returns avaible device ids
gamepad.getAvaibleDevices();
```
Select gamepad
``` dart
gamepad.selectGamepad(id);
```
Activate auto vibration
``` dart
// when auto vibration is true, gamepad vibrates via triggers.
gamepad.setAutoVibration(true);
```
Vibrate gamepad
``` dart
// leftMotorSpeed and rightMotorSpeed are should be between 0.0 and 1.0
gamepad.setVibration(leftMotorSpeed, rightMotorSpeed);
```

For testing there is a gamepad layout
``` dart
import 'package:win_gamepad/gamepad_layout.dart';
...

    GamepadLayout(gamepad: gamepad,)

...
```

## Example
``` dart
import 'package:flutter/material.dart';

import 'package:win_gamepad/gamepad.dart';
import 'package:win_gamepad/gamepad_layout.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({Key? key}) : super(key: key);

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  Gamepad gamepad = Gamepad();
  @override
  void initState() {
    super.initState();
    gamepad.initialize(onCallback: (gamepadState) {
      setState(() {});
    });
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Plugin win_gamepad'),
        ),
        body: Center(
          child: Column(
            children: [
              Opacity(
                opacity: 1,
                child: GamepadLayout(
                  gamepad: gamepad,
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}
```