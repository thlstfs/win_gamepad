import 'dart:async';

import 'package:flutter/services.dart';

class WinGamepad {
  static const MethodChannel _channel =
      MethodChannel('com.thlstfs.win_gamepad/methods');
  static const EventChannel _eventChannel =
      EventChannel('com.thlstfs.win_gamepad/events');
  static Stream<dynamic>? _eventStream;

  static Stream<dynamic> get eventStream {
    _eventStream ??= _eventChannel.receiveBroadcastStream();
    return _eventStream as Stream<dynamic>;
  }

  static Future<String?> get platformVersion async {
    final String? version = await _channel.invokeMethod('getPlatformVersion');
    return version;
  }

  static Future<bool> selectGamepad(int id) async {
    final bool? res = await _channel.invokeMethod('selectGamepad', {"id": id});
    return res ?? false;
  }

  static Future<bool> setAutoVibration(bool value) async {
    final bool? res =
        await _channel.invokeMethod('setAutoVibration', {"value": value});
    return res ?? false;
  }

  static Future<bool> setVibration(
      double leftMotorSpeed, double rightMotorSpeed) async {
    final bool? res = await _channel.invokeMethod('setVibration',
        {"leftMotorSpeed": leftMotorSpeed, "rightMotorSpeed": rightMotorSpeed});
    return res ?? false;
  }

  static Future<List<int>> get getAvaibleDevices async {
    List<int>? list = await _channel.invokeListMethod('getAvaibleDevices');
    return list ?? [];
  }

  static Future<String?> initialize() async {
    final String? version = await _channel.invokeMethod('initialize');
    return version;
  }

  static Future<String?> cleanup() async {
    final String? res = await _channel.invokeMethod('cleanup');
    return res;
  }
}
