#pragma once

#include "Input/Keycode.h"

#include <unordered_map>

#include <GLFW/glfw3.h>

namespace NXTN {
    int MapToGLFWKeyCode(KeyCode keyCode);
    KeyCode MapFromGLFWKeyCode(int glfwKeyCode);

    int MapToGLFWMouseButton(MouseButtonCode mouseButton);
    MouseButtonCode MapFromGLFWMouseButton(int glfwMouseButton);
}