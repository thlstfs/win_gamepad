import 'dart:async';

import 'package:flutter/services.dart';

class WinGamepad {
  static const MethodChannel _channel = MethodChannel('win_gamepad');

  static Future<String?> get platformVersion async {
    final String? version = await _channel.invokeMethod('getPlatformVersion');
    return version;
  }

  static Future<String?> get gamepadState async {
    final String? version = await _channel.invokeMethod('getGamepadState');
    return version;
  }
}
