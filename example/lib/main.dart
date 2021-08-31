import 'package:flutter/material.dart';
import 'dart:async';

import 'package:flutter/services.dart';
import 'package:win_gamepad/gamepad.dart';
import 'package:win_gamepad/win_gamepad.dart';
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
  String _platformVersion = 'Unknown';
  @override
  void initState() {
    super.initState();
    connect();
  }

  @override
  void dispose() {
    super.dispose();
    disconnect();
  }

  void getData() async {
    Future.delayed(const Duration(milliseconds: 16)).then((value) async {
      await initPlatformState();
      getData();
    });
  }

  Future<void> connect() async {
    String res;
    try {
      res = await WinGamepad.connect() ?? 'Unknown platform version';
      print(res);
      // ignore: empty_catches
    } on PlatformException {
      res = 'Failed to connect.';
    }
  }

  Future<void> disconnect() async {
    String res;
    try {
      res = await WinGamepad.disconnect() ?? 'Unknown platform version';
      print(res);
      // ignore: empty_catches
    } on PlatformException {
      res = 'Failed to disconnect.';
    }
  }

  // Platform messages are asynchronous, so we initialize in an async method.
  Future<void> initPlatformState() async {
    String platformVersion;
    // Platform messages may fail, so we use a try/catch PlatformException.
    // We also handle the message potentially returning null.
    try {
      platformVersion =
          await WinGamepad.gamepadState ?? 'Unknown platform version';
    } on PlatformException {
      platformVersion = 'Failed to get platform version.';
    }

    // If the widget was removed from the tree while the asynchronous platform
    // message was in flight, we want to discard the reply rather than calling
    // setState to update our non-existent appearance.
    if (!mounted) return;

    setState(() {
      _platformVersion = platformVersion;
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
                  gamepadStateStream: WinGamepad.eventStream,
                ),
              ),
            ],
          ),
        ),
        floatingActionButton: FloatingActionButton(
          onPressed: () async {
            await initPlatformState();
          },
        ),
      ),
    );
  }
}
