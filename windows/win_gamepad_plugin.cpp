#include "include/win_gamepad/win_gamepad_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <map>
#include <memory>
#include <sstream>

#include <Xinput.h>
#include <stdio.h>

#pragma comment(lib,"XInput.lib")

#pragma comment(lib,"Xinput9_1_0.lib")
namespace {

class WinGamepadPlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  WinGamepadPlugin();

  virtual ~WinGamepadPlugin();

 private:
  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
};

// static
void WinGamepadPlugin::RegisterWithRegistrar(
    flutter::PluginRegistrarWindows *registrar) {
  auto channel =
      std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
          registrar->messenger(), "win_gamepad",
          &flutter::StandardMethodCodec::GetInstance());

  auto plugin = std::make_unique<WinGamepadPlugin>();

  channel->SetMethodCallHandler(
      [plugin_pointer = plugin.get()](const auto &call, auto result) {
        plugin_pointer->HandleMethodCall(call, std::move(result));
      });

  registrar->AddPlugin(std::move(plugin));
}

WinGamepadPlugin::WinGamepadPlugin() {}

WinGamepadPlugin::~WinGamepadPlugin() {}

void WinGamepadPlugin::HandleMethodCall(
    const flutter::MethodCall<flutter::EncodableValue> &method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  if (method_call.method_name().compare("getPlatformVersion") == 0) {
    std::ostringstream version_stream;
    version_stream << "Windows ";
    if (IsWindows10OrGreater()) {
      version_stream << "10+";
    } else if (IsWindows8OrGreater()) {
      version_stream << "8";
    } else if (IsWindows7OrGreater()) {
      version_stream << "7";
    }
    result->Success(flutter::EncodableValue(version_stream.str()));
  } else if (method_call.method_name().compare("getGamepadState") == 0) {
    std::ostringstream version_stream;
    XINPUT_STATE state;    
    DWORD playerIndex = 0;

    DWORD dwresult = XInputGetState(playerIndex, &state);
    if (dwresult == ERROR_SUCCESS && state.dwPacketNumber)
    {
      version_stream << state.Gamepad.sThumbLX;
      XINPUT_VIBRATION vibration;
      vibration.wLeftMotorSpeed = state.Gamepad.bLeftTrigger * 0xFFFF / 0xFF;
      vibration.wRightMotorSpeed = state.Gamepad.bRightTrigger * 0xFFFF / 0xFF;
      XInputSetState(0, &vibration);
      result->Success(flutter::EncodableValue(version_stream.str()));
    }
  } else {
    result->NotImplemented();
  }
}

}  // namespace

void WinGamepadPluginRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  WinGamepadPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
