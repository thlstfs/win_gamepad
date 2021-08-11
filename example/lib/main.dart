import 'package:flutter/material.dart';
import 'dart:async';

import 'package:flutter/services.dart';
import 'package:win_gamepad/win_gamepad.dart';

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
  String eventVal = 'test';
  @override
  void initState() {
    super.initState();
    getData();
    connect();
    init();
    //disconnect();
  }

  @override
  void dispose() {
    super.dispose();
  }

  void getData() async {
    Future.delayed(const Duration(milliseconds: 16)).then((value) async {
      await initPlatformState();
      getData();
    });
  }

  void init() async {
    WinGamepad.eventStream.listen((event) {
      eventVal = event.toString();
      setState(() {
        print(event);
      });
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
          title: const Text('Plugin example app'),
        ),
        body: Center(
          child: Column(
            children: [
              CircleAvatar(
                child: const Text(
                  'A',
                  style: TextStyle(color: Colors.white),
                ),
                backgroundColor: _platformVersion == "A"
                    ? Colors.red.shade600
                    : Colors.red.shade300,
              ),
              Text('Running on: $_platformVersion\n'),
              Text('$eventVal\n'),
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
