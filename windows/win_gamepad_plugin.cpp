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
#include <stdio.h>

#pragma comment(lib, "XInput.lib")

#pragma comment(lib, "Xinput9_1_0.lib")



namespace
{
  
template<typename T = flutter::EncodableValue>
class MyStreamHandler: public flutter::StreamHandler<T> {
public:
	MyStreamHandler () = default;
	virtual ~MyStreamHandler () = default;

	void on_callback (const flutter::EncodableMap _map) {
        if (uint64_t (this) == 0xddddddddddddddddul)
            return;
        std::unique_lock<std::mutex> _ul (m_mtx);


		    m_value = _map;
        if (m_sink.get())
		      m_sink.get()->Success (m_value);
	}

protected:
	std::unique_ptr<flutter::StreamHandlerError<T>> OnListenInternal (const T *arguments, std::unique_ptr<flutter::EventSink<T>> &&events) override {
        std::unique_lock<std::mutex> _ul (m_mtx);
		m_sink = std::move (events);
        return nullptr;
	}

	std::unique_ptr<flutter::StreamHandlerError<T>> OnCancelInternal (const T *arguments) override {
        std::unique_lock<std::mutex> _ul (m_mtx);
		m_sink.release ();
        return nullptr;
	}

private:
	flutter::EncodableValue m_value;
    std::mutex m_mtx;
	std::unique_ptr<flutter::EventSink<T>> m_sink;
};

flutter::EncodableMap XINPUT_GAMEPAD_to_EncodableMap(const XINPUT_GAMEPAD& gamepad)
{
  std::vector<uint8_t> buttons;
  if (gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)        buttons.push_back(0);
  if (gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)      buttons.push_back(1);
  if (gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)      buttons.push_back(2);
  if (gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)     buttons.push_back(3);
  if (gamepad.wButtons & XINPUT_GAMEPAD_START)          buttons.push_back(4);
  if (gamepad.wButtons & XINPUT_GAMEPAD_BACK)           buttons.push_back(5);
  if (gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)     buttons.push_back(6);
  if (gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)    buttons.push_back(7);
  if (gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)  buttons.push_back(8);
  if (gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) buttons.push_back(9);
  if (gamepad.wButtons & XINPUT_GAMEPAD_A)              buttons.push_back(10);
  if (gamepad.wButtons & XINPUT_GAMEPAD_B)              buttons.push_back(11);
  if (gamepad.wButtons & XINPUT_GAMEPAD_X)              buttons.push_back(12);
  if (gamepad.wButtons & XINPUT_GAMEPAD_Y)              buttons.push_back(13);
  std::vector<uint8_t> triggers;
  triggers.push_back((uint8_t) gamepad.bLeftTrigger);
  triggers.push_back((uint8_t) gamepad.bRightTrigger);

  std::vector<int32_t> thumbs;
  thumbs.push_back((int32_t)gamepad.sThumbLX);
  thumbs.push_back((int32_t)gamepad.sThumbLY);
  thumbs.push_back((int32_t)gamepad.sThumbRX);
  thumbs.push_back((int32_t)gamepad.sThumbRY);
  return flutter::EncodableMap{
    {flutter::EncodableValue("buttons"),buttons},
    {flutter::EncodableValue("triggers"),triggers},
    {flutter::EncodableValue("thumbs"),thumbs}
  };


}
  class WinGamepadPlugin : public flutter::Plugin
  {
  public:
    MyStreamHandler<> *stream_handler = nullptr;
    std::atomic<bool> is_connected;
  public:
    static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

    WinGamepadPlugin();

    virtual ~WinGamepadPlugin();

  private:
    std::unique_ptr<flutter::MethodChannel<flutter::EncodableValue>> m_method_channel;
    std::unique_ptr<flutter::EventChannel<flutter::EncodableValue>> m_event_channel;
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

    plugin->stream_handler = new MyStreamHandler<>();
    auto _obj_stm_handle = static_cast<flutter::StreamHandler<flutter::EncodableValue>*> (plugin->stream_handler);
    std::unique_ptr<flutter::StreamHandler<flutter::EncodableValue>> _ptr {_obj_stm_handle};
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
    else if (_method_name.compare("getGamepadState") == 0)
    {
      std::ostringstream version_stream;
      XINPUT_STATE state;
      DWORD playerIndex = 0;

      DWORD dwresult = XInputGetState(playerIndex, &state);
      if (dwresult == ERROR_SUCCESS && state.dwPacketNumber)
      {
        if (state.Gamepad.wButtons & XINPUT_GAMEPAD_A)
        {
          version_stream << "A";
        }
        if (state.Gamepad.wButtons & XINPUT_GAMEPAD_B)
        {
          version_stream << "B";
        }

        XINPUT_VIBRATION vibration;
        vibration.wLeftMotorSpeed = state.Gamepad.bLeftTrigger * 0xFFFF / 0xFF;
        vibration.wRightMotorSpeed = state.Gamepad.bRightTrigger * 0xFFFF / 0xFF;
        XInputSetState(0, &vibration);
        result->Success(flutter::EncodableValue(version_stream.str()));
      }
    }
    else if (_method_name.compare("connect") == 0) {
      if (!is_connected.load()) {
        is_connected.store(true);
        std::thread([this] () {
          auto timeNow = std::chrono::system_clock::now();
          auto sleepTime = std::chrono::milliseconds(30);
          while (is_connected.load()) {
            timeNow = std::chrono::system_clock::now();
            XINPUT_STATE state;
            DWORD playerIndex = 0;
            
            DWORD dwresult = XInputGetState(playerIndex, &state);
            if (dwresult == ERROR_SUCCESS && state.dwPacketNumber)
            {
              stream_handler->on_callback(XINPUT_GAMEPAD_to_EncodableMap(state.Gamepad));
              
            }
            std::this_thread::sleep_until(timeNow + sleepTime);
          }
        }).detach();
        result->Success(flutter::EncodableValue("connected"));
      }
    }
    else if (_method_name.compare("disconnect") == 0) {
      is_connected.store(false);
      result->Success(flutter::EncodableValue("disconnected"));
      
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
