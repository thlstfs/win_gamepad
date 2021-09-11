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
