#include <GLFW/glfw3.h>
#include <algorithm>
#include <axolotl/iomanager.hh>
#include <axolotl/window.hh>

namespace axl {

  IOManager::IOManager(Window &owner): _sensitivity(0.3f) {
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
    }
    for (i32 i = 0; i < (i32)JoyStick::Last; ++i) {
      _joy_sticks[i] = 0.0f;
    }
  }

  IOManager::~IOManager() { }

  void IOManager::UpdatePads(GLFWwindow *glfwWindow) {
    for (i32 j = 0; j < (i32)Pad::Last; ++j) {
      if (!PadPresent((Pad)j)) continue;

      GLFWgamepadstate state;
      if (!glfwGetGamepadState(j, &state)) return;

      int count;
      const float *axes = glfwGetJoystickAxes(j, &count);
      if (count > (i32)JoyStick::Last) count = (i32)JoyStick::Last;
      for (int n = 0; n < count; ++n)
        _joy_sticks[n] = axes[n];

      for (i32 i = 0; i < (i32)PadButton::Last; ++i) {
        bool pressed = state.buttons[i];
        _pad[j][i] = pressed;
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
