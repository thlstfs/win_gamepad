#include "include/win_gamepad/win_gamepad_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>
#include <flutter/event_channel.h>

#include <atomic>
#include <chrono>
#include <thread>
#include <map>
#include <memory>
#include <sstream>
#include <mutex>

#include <Xinput.h>
#include <iostream>

#pragma comment(lib, "XInput.lib")

#pragma comment(lib, "Xinput9_1_0.lib")


namespace
{

  template <typename T = flutter::EncodableValue>
  class GamepadStreamHandler : public flutter::StreamHandler<T>
  {
  public:
    GamepadStreamHandler() = default;
    virtual ~GamepadStreamHandler() = default;

    void OnCallback(const flutter::EncodableMap _map)
    {
      if (uint64_t(this) == 0xddddddddddddddddul)
        return;
      std::unique_lock<std::mutex> _ul(m_mtx);
      m_value = _map;
      if (m_sink.get())
        m_sink.get()->Success(m_value);
    }

  protected:
    std::unique_ptr<flutter::StreamHandlerError<T>> OnListenInternal(const T *arguments, std::unique_ptr<flutter::EventSink<T>> &&events) override
    {
      std::unique_lock<std::mutex> _ul(m_mtx);
      m_sink = std::move(events);
      return nullptr;
    }

    std::unique_ptr<flutter::StreamHandlerError<T>> OnCancelInternal(const T *arguments) override
    {
      std::unique_lock<std::mutex> _ul(m_mtx);
      m_sink.release();
      return nullptr;
    }

  private:
    flutter::EncodableMap m_value;
    std::mutex m_mtx;
    std::unique_ptr<flutter::EventSink<T>> m_sink;
  };

  flutter::EncodableMap XINPUT_GAMEPAD_to_EncodableMap(const XINPUT_GAMEPAD gamepad)
  {
    std::vector<uint8_t> buttons;
    if (gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
      buttons.push_back(0);
    if (gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
      buttons.push_back(1);
    if (gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
      buttons.push_back(2);
    if (gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
      buttons.push_back(3);
    if (gamepad.wButtons & XINPUT_GAMEPAD_START)
      buttons.push_back(4);
    if (gamepad.wButtons & XINPUT_GAMEPAD_BACK)
      buttons.push_back(5);
    if (gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)
      buttons.push_back(6);
    if (gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)
      buttons.push_back(7);
    if (gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)
      buttons.push_back(8);
    if (gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
      buttons.push_back(9);
    if (gamepad.wButtons & XINPUT_GAMEPAD_A)
      buttons.push_back(10);
    if (gamepad.wButtons & XINPUT_GAMEPAD_B)
      buttons.push_back(11);
    if (gamepad.wButtons & XINPUT_GAMEPAD_X)
      buttons.push_back(12);
    if (gamepad.wButtons & XINPUT_GAMEPAD_Y)
      buttons.push_back(13);
    std::vector<uint8_t> triggers;
    triggers.reserve(2);
    triggers.push_back((uint8_t)gamepad.bLeftTrigger);
    triggers.push_back((uint8_t)gamepad.bRightTrigger);

    std::vector<int32_t> thumbs;
    triggers.reserve(4);
    thumbs.push_back((int32_t)gamepad.sThumbLX);
    thumbs.push_back((int32_t)gamepad.sThumbLY);
    thumbs.push_back((int32_t)gamepad.sThumbRX);
    thumbs.push_back((int32_t)gamepad.sThumbRY);
    return flutter::EncodableMap{
        {flutter::EncodableValue("buttons"), buttons},
        {flutter::EncodableValue("triggers"), triggers},
        {flutter::EncodableValue("thumbs"), thumbs}};
  }
  class WinGamepadPlugin : public flutter::Plugin
  {
  public:
    GamepadStreamHandler<> *stream_handler = nullptr;
    std::atomic<bool> is_connected;
    std::atomic<DWORD> deviceIndex{0};
    std::atomic<bool> autoVibration{false};

  public:
    static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

    WinGamepadPlugin();

    virtual ~WinGamepadPlugin();

  private:
    std::unique_ptr<flutter::MethodChannel<flutter::EncodableValue>> m_method_channel;
    std::unique_ptr<flutter::EventChannel<flutter::EncodableValue>> m_event_channel;
    std::thread thread;

    // Called when a method is called on this plugin's channel from Dart.
    void HandleMethodCall(
        const flutter::MethodCall<flutter::EncodableValue> &method_call,
        std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
  };

  // static
  void WinGamepadPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarWindows *registrar)
  {
    auto plugin = std::make_unique<WinGamepadPlugin>();
    plugin->is_connected.store(false);

    plugin->m_method_channel =
        std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
            registrar->messenger(), "com.thlstfs.win_gamepad/methods",
            &flutter::StandardMethodCodec::GetInstance());

    plugin->m_method_channel->SetMethodCallHandler(
        [plugin_pointer = plugin.get()](const auto &call, auto result)
        {
          plugin_pointer->HandleMethodCall(call, std::move(result));
        });
    plugin->m_event_channel =
        std::make_unique<flutter::EventChannel<flutter::EncodableValue>>(
            registrar->messenger(), "com.thlstfs.win_gamepad/events",
            &flutter::StandardMethodCodec::GetInstance());

    plugin->stream_handler = new GamepadStreamHandler<>();
    auto _obj_stm_handle = static_cast<flutter::StreamHandler<flutter::EncodableValue> *>(plugin->stream_handler);
    std::unique_ptr<flutter::StreamHandler<flutter::EncodableValue>> _ptr{_obj_stm_handle};
    plugin->m_event_channel->SetStreamHandler(std::move(_ptr));
    registrar->AddPlugin(std::move(plugin));
  }

  WinGamepadPlugin::WinGamepadPlugin() {}

  WinGamepadPlugin::~WinGamepadPlugin() {}

  void WinGamepadPlugin::HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result)
  {
    std::string _method_name = method_call.method_name();
    if (_method_name.compare("getPlatformVersion") == 0)
    {
      std::ostringstream version_stream;
      version_stream << "Windows ";
      if (IsWindows10OrGreater())
      {
        version_stream << "10+";
      }
      else if (IsWindows8OrGreater())
      {
        version_stream << "8";
      }
      else if (IsWindows7OrGreater())
      {
        version_stream << "7";
      }
      result->Success(flutter::EncodableValue(version_stream.str()));
    }
    else if (method_call.method_name().compare("selectGamepad") == 0)
    {
      flutter::EncodableMap arguments = std::get<flutter::EncodableMap>(*method_call.arguments());
      DWORD id = std::get<int>(arguments[flutter::EncodableValue("id")]);
      deviceIndex.store(id);
      result->Success(true);
    }
    else if (method_call.method_name().compare("setAutoVibration") == 0)
    {
      flutter::EncodableMap arguments = std::get<flutter::EncodableMap>(*method_call.arguments());
      bool value = std::get<bool>(arguments[flutter::EncodableValue("value")]);
      autoVibration.store(value);
      result->Success(true);
    }
    else if (method_call.method_name().compare("setVibration") == 0)
    {
      XINPUT_VIBRATION vibration;
      flutter::EncodableMap arguments = std::get<flutter::EncodableMap>(*method_call.arguments());
      double leftMotorSpeed = std::get<double>(arguments[flutter::EncodableValue("leftMotorSpeed")]);
      double rightMotorSpeed = std::get<double>(arguments[flutter::EncodableValue("rightMotorSpeed")]);
      leftMotorSpeed = leftMotorSpeed > 1 ? 1 : (leftMotorSpeed < 0 ? 0 : leftMotorSpeed);
      rightMotorSpeed = rightMotorSpeed > 1 ? 1 : (rightMotorSpeed < 0 ? 0 : rightMotorSpeed);

      vibration.wLeftMotorSpeed = static_cast<WORD>(leftMotorSpeed * 0xFFFF);
      vibration.wRightMotorSpeed = static_cast<WORD>(rightMotorSpeed * 0xFFFF);
      XInputSetState(deviceIndex.load(), &vibration);
      result->Success(true);
    }
    else if (_method_name.compare("getAvaibleDevices") == 0)
    {
      flutter::EncodableList devices;
      XINPUT_STATE state;

      for (DWORD i = 0; i < 10; i++)
      {
        DWORD dwresult = XInputGetState(i, &state);
        if (dwresult == ERROR_SUCCESS && state.dwPacketNumber)
        {
          devices.push_back((uint8_t)i);
        }
      }
      result->Success(devices);
    }
    else if (_method_name.compare("initialize") == 0)
    {
      if (!is_connected.load())
      {
        is_connected.store(true);
        thread = std::thread([this]()
          {
            auto timeNow = std::chrono::system_clock::now();
            auto sleepTime = std::chrono::milliseconds(30);
            XINPUT_STATE state;
            DWORD dwresult;
            XINPUT_VIBRATION vibration;
            while (is_connected.load())
            {
              timeNow = std::chrono::system_clock::now();
              dwresult = XInputGetState(deviceIndex.load(), &state);

              if (dwresult == ERROR_SUCCESS && state.dwPacketNumber)
              {
                stream_handler->OnCallback(XINPUT_GAMEPAD_to_EncodableMap(state.Gamepad));

                if (autoVibration.load())
                {
                  vibration.wLeftMotorSpeed = state.Gamepad.bLeftTrigger * 0xFFFF / 0xFF;
                  vibration.wRightMotorSpeed = state.Gamepad.bRightTrigger * 0xFFFF / 0xFF;
                  XInputSetState(deviceIndex.load(), &vibration);
                }
              }
              std::this_thread::sleep_until(timeNow + sleepTime);
            }
          });
        thread.detach();
        result->Success(flutter::EncodableValue("initialized"));
      }
      else
      {
        result->Success(flutter::EncodableValue("already initialized"));
      }
    }
    else if (_method_name.compare("cleanup") == 0)
    {
      is_connected.store(false);
      deviceIndex.store(0);
      autoVibration.store(false);
      thread.~thread();
      result->Success(flutter::EncodableValue("cleanup"));
    }
    else
    {
      result->NotImplemented();
    }
  }

} // namespace

void WinGamepadPluginRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar)
{
  WinGamepadPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
