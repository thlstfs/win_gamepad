import 'dart:async';

import 'package:flutter/services.dart';
import 'package:win_gamepad/gamepad.dart';

class WinGamepad {
  static const MethodChannel _channel =
      MethodChannel('com.thlstfs.win_gamepad/methods');
  static const EventChannel _eventChannel =
      EventChannel('com.thlstfs.win_gamepad/events');
  static late Stream<dynamic> _eventStream;

  static Stream<dynamic> get eventStream {
    _eventStream = _eventChannel.receiveBroadcastStream();
    return _eventStream;
  }

  static Future<String?> get platformVersion async {
    final String? version = await _channel.invokeMethod('getPlatformVersion');
    return version;
  }

  static Future<String?> get gamepadState async {
    final String? version = await _channel.invokeMethod('getGamepadState');
    return version;
  }

  static Future<String?> connect() async {
    final String? version = await _channel.invokeMethod('connect');
    return version;
  }

  static Future<String?> disconnect() async {
    final String? version = await _channel.invokeMethod('disconnect');
    return version;
  }
}
