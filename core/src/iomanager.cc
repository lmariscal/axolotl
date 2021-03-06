#include <GLFW/glfw3.h>
#include <algorithm>
#include <axolotl/iomanager.hh>
#include <axolotl/window.hh>

namespace axl {

  IOManager::IOManager(Window &owner): _sensitivity(0.3f), _last_scroll(0.0f) {
    for (i32 i = 0; i < (i32)Key::Last; ++i) {
      _keys[i] = false;
      _hold_keys[i] = false;
    }
    for (i32 i = 0; i < (i32)MouseButton::Last; ++i) {
      _mouse[i] = false;
      _hold_mouse[i] = false;
    }
    for (i32 i = 0; i < (i32)Pad::Last; ++i) {
      for (i32 j = 0; j < (i32)PadButton::Last; ++j) {
        _pad[i][j] = false;
        _hold_pad[i][j] = false;
      }
      for (i32 j = 0; j < (i32)JoyStick::Last; ++j) {
        _joy_sticks[i][j] = 0.0f;
      }
    }
  }

  IOManager::~IOManager() { }

  bool IOManager::WheelMoved() {
    return _last_scroll != v2 {};
  }

  v2 IOManager::GetWheelMovement() {
    return _last_scroll;
  }

  void IOManager::ScrollEvent(v2 offset) {
    _last_scroll = offset;
  }

  void IOManager::UpdatePads(GLFWwindow *glfwWindow) {
    for (i32 i = 0; i < (i32)Pad::Last; ++i) {
      if (!PadPresent((Pad)i))
        continue;

      i32 count;
      const float *axes = glfwGetJoystickAxes(i, &count);
      if (!axes)
        continue;

      if (count > (i32)JoyStick::Last)
        count = (i32)JoyStick::Last;
      for (i32 n = 0; n < count; ++n)
        _joy_sticks[i][n] = axes[n];

      GLFWgamepadstate state;
      if (glfwGetGamepadState(i, &state)) {
        for (i32 j = 0; j < (i32)PadButton::Last; ++j) {
          bool pressed = state.buttons[j];
          _pad[i][j] = pressed;
        }
      } else if (glfwJoystickPresent(i)) {
        const uchar *buttons = glfwGetJoystickButtons(i, &count);
        if (!buttons)
          continue;

        for (i32 j = 0; j < count; ++j) {
          if (count > 0)
            _pad[i][(i32)PadButton::A] = buttons[0];
          if (count > 1)
            _pad[i][(i32)PadButton::B] = buttons[1];
          if (count > 3)
            _pad[i][(i32)PadButton::X] = buttons[3];
          if (count > 4)
            _pad[i][(i32)PadButton::Y] = buttons[4];
          if (count > 7)
            _pad[i][(i32)PadButton::RightBumper] = buttons[7];
          if (count > 6)
            _pad[i][(i32)PadButton::LeftBumper] = buttons[6];
          if (count > 11)
            _pad[i][(i32)PadButton::Start] = buttons[11];
          if (count > 13)
            _pad[i][(i32)PadButton::LeftThumb] = buttons[13];
          if (count > 14)
            _pad[i][(i32)PadButton::RightThumb] = buttons[13];
          if (count > 15)
            _pad[i][(i32)PadButton::DPadUp] = buttons[15];
          if (count > 16)
            _pad[i][(i32)PadButton::DPadRight] = buttons[16];
          if (count > 17)
            _pad[i][(i32)PadButton::DPadDown] = buttons[17];
          if (count > 18)
            _pad[i][(i32)PadButton::DPadLeft] = buttons[18];
        }
      }
    }
  }

  bool IOManager::PadPresent(Pad pad) {
    return glfwJoystickPresent((i32)pad);
  }

  void IOManager::KeyEvent(i32 key, bool pressed) {
    _keys[key] = pressed;
  }

  void IOManager::MouseButtonEvent(i32 button, bool pressed) {
    _mouse[button] = pressed;
  }

  void IOManager::MouseEvent(f64 x, f64 y) {
    _mouse_pos.x = x;
    _mouse_pos.y = y;
  }

  void IOManager::UpdateHolds() {
    std::copy_n(_keys, (i32)Key::Last, _hold_keys);
    std::copy_n(_mouse, (i32)MouseButton::Last, _hold_mouse);
    for (i32 i = 0; i < (i32)Pad::Last; ++i)
      std::copy_n(_pad[i], (i32)PadButton::Last, _hold_pad[i]);

    _last_scroll = v2(0.0f);
  }

  bool IOManager::KeyDown(Key key) {
    return _keys[(i32)key];
  }

  bool IOManager::KeyHeld(Key key) {
    return _keys[(i32)key] && _hold_keys[(i32)key];
  }

  bool IOManager::KeyTriggered(Key key) {
    return _keys[(i32)key] && !_hold_keys[(i32)key];
  }

  bool IOManager::KeyReleased(Key key) {
    return _keys[(i32)key] && !_hold_keys[(i32)key];
  }

  bool IOManager::ButtonDown(MouseButton button) {
    return _mouse[(i32)button];
  }

  bool IOManager::ButtonHeld(MouseButton button) {
    return _mouse[(i32)button] && _hold_mouse[(i32)button];
  }

  bool IOManager::ButtonTriggered(MouseButton button) {
    return _mouse[(i32)button] && !_hold_mouse[(i32)button];
  }

  bool IOManager::ButtonReleased(MouseButton button) {
    return !_mouse[(i32)button] && _hold_mouse[(i32)button];
  }

  bool IOManager::ButtonDown(Pad pad, PadButton button) {
    return _pad[(i32)pad][(i32)button];
  }

  bool IOManager::ButtonHeld(Pad pad, PadButton button) {
    return _pad[(i32)pad][(i32)button] && _hold_pad[(i32)pad][(i32)button];
  }

  bool IOManager::ButtonTriggered(Pad pad, PadButton button) {
    return _pad[(i32)pad][(i32)button] && !_hold_pad[(i32)pad][(i32)button];
  }

  bool IOManager::ButtonReleased(Pad pad, PadButton button) {
    return _pad[(i32)pad][(i32)button] && !_hold_pad[(i32)pad][(i32)button];
  }

  f32 IOManager::GetAxis(Pad pad, JoyStick joy_stick) {
    return _joy_sticks[(i32)pad][(i32)joy_stick];
  }

  bool IOManager::DoubleClicked(MouseButton button) {
    return false;
  }

  void IOManager::UpdateRelativePositions() {
    _relative_mouse_pos = _mouse_pos - _last_mouse_pos;
    _last_mouse_pos = _mouse_pos;
  }

  v2 IOManager::GetAbsolutePosition() {
    return v2(_mouse_pos.x, _mouse_pos.y);
  }

  v2 IOManager::GetRelativePosition() {
    return v2(_relative_mouse_pos.x * _sensitivity, _relative_mouse_pos.y * _sensitivity);
  }

  void IOManager::SetMouseSensitivity(f32 sens) {
    _sensitivity = sens;
  }

} // namespace axl
