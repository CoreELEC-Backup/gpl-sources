#ifndef RUBY_INPUT_MOUSE_RAWINPUT
#define RUBY_INPUT_MOUSE_RAWINPUT

namespace ruby {

struct InputMouseRawInput {
  uintptr_t handle = 0;
  bool mouseAcquired = false;

  struct Mouse {
    HID::Mouse hid;

    signed relativeX = 0;
    signed relativeY = 0;
    signed relativeZ = 0;
    bool buttons[5] = {0};
  } ms;

  bool acquire() {
    if(mouseAcquired == false) {
      mouseAcquired = true;
      ShowCursor(false);
    }
    return true;
  }

  bool unacquire() {
    if(mouseAcquired == true) {
      mouseAcquired = false;
      ReleaseCapture();
      ClipCursor(NULL);
      ShowCursor(true);
    }
    return true;
  }

  bool acquired() {
    if(mouseAcquired == true) {
      SetFocus((HWND)handle);
      SetCapture((HWND)handle);
      RECT rc;
      GetWindowRect((HWND)handle, &rc);
      ClipCursor(&rc);
    }
    return GetCapture() == (HWND)handle;
  }

  void update(RAWINPUT* input) {
    if((input->data.mouse.usFlags & 1) == MOUSE_MOVE_RELATIVE) {
      ms.relativeX += input->data.mouse.lLastX;
      ms.relativeY += input->data.mouse.lLastY;
    }

    if(input->data.mouse.usButtonFlags & RI_MOUSE_WHEEL) {
      ms.relativeZ += (int16_t)input->data.mouse.usButtonData;
    }

    if(input->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_1_DOWN) ms.buttons[0] = 1;
    if(input->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_1_UP  ) ms.buttons[0] = 0;
    if(input->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_2_DOWN) ms.buttons[1] = 1;
    if(input->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_2_UP  ) ms.buttons[1] = 0;
    if(input->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_3_DOWN) ms.buttons[2] = 1;
    if(input->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_3_UP  ) ms.buttons[2] = 0;
    if(input->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN) ms.buttons[3] = 1;
    if(input->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_UP  ) ms.buttons[3] = 0;
    if(input->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN) ms.buttons[4] = 1;
    if(input->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_UP  ) ms.buttons[4] = 0;
  }

  void assign(unsigned groupID, unsigned inputID, int16_t value) {
    auto& group = ms.hid.group[groupID];
    if(group.input[inputID].value == value) return;
    if(input.onChange) input.onChange(ms.hid, groupID, inputID, group.input[inputID].value, value);
    group.input[inputID].value = value;
  }

  void poll(vector<HID::Device*>& devices) {
    assign(HID::Mouse::GroupID::Axis, 0, ms.relativeX);
    assign(HID::Mouse::GroupID::Axis, 1, ms.relativeY);
    assign(HID::Mouse::GroupID::Axis, 2, ms.relativeZ);

    //keys are intentionally reordered below:
    //in ruby, button order is {left, middle, right, up, down}
    assign(HID::Mouse::GroupID::Button, 0, ms.buttons[0]);
    assign(HID::Mouse::GroupID::Button, 2, ms.buttons[1]);
    assign(HID::Mouse::GroupID::Button, 1, ms.buttons[2]);
    assign(HID::Mouse::GroupID::Button, 4, ms.buttons[3]);
    assign(HID::Mouse::GroupID::Button, 3, ms.buttons[4]);

    ms.relativeX = 0;
    ms.relativeY = 0;
    ms.relativeZ = 0;

    devices.append(&ms.hid);
  }

  bool init(uintptr_t handle) {
    this->handle = handle;

    ms.hid.id = 2;

    ms.hid.axis().append({"X"});
    ms.hid.axis().append({"Y"});
    ms.hid.axis().append({"Z"});

    ms.hid.button().append({"Left"});
    ms.hid.button().append({"Middle"});
    ms.hid.button().append({"Right"});
    ms.hid.button().append({"Up"});
    ms.hid.button().append({"Down"});

    rawinput.updateMouse = {&InputMouseRawInput::update, this};
    return true;
  }

  void term() {
    unacquire();
  }
};

}

#endif
