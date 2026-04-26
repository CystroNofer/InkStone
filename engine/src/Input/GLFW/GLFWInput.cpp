#include "pch.h"

#include "GLFWInput.h"

namespace NXTN {
	GLFWwindow* GLFWInput::m_Window = nullptr;

	GLFWInput::GLFWInput(void* windowPtr)
	{
		m_Window = static_cast<GLFWwindow*>(windowPtr);
	}

	bool GLFWInput::IsKeyPressedImpl(int keycode)
	{
		int state = glfwGetKey(m_Window, keyCodeMapToGLFW[keycode]);
		
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool GLFWInput::IsMouseButtonPressedImpl(int button)
	{
		int state = glfwGetMouseButton(m_Window, keyCodeMapToGLFW[button]);

		return state == GLFW_PRESS;
	}

	vec2 GLFWInput::GetMousePosImpl()
	{
		double resX, resY;
		glfwGetCursorPos(m_Window, &resX, &resY);

		return vec2(resX, resY);
	}

	std::unordered_map<int, int> GLFWInput::keyCodeMapToGLFW = {
		{ NXTN_KEY_UNKNOWN         , GLFW_KEY_UNKNOWN },

		/* --------- Mouse Buttons -------- */
		{ NXTN_MOUSE_BUTTON_0      , GLFW_MOUSE_BUTTON_7      },
		{ NXTN_MOUSE_BUTTON_1      , GLFW_MOUSE_BUTTON_1      },
		{ NXTN_MOUSE_BUTTON_2      , GLFW_MOUSE_BUTTON_2      },
		{ NXTN_MOUSE_BUTTON_3      , GLFW_MOUSE_BUTTON_3      },
		{ NXTN_MOUSE_BUTTON_4      , GLFW_MOUSE_BUTTON_4      },
		{ NXTN_MOUSE_BUTTON_5      , GLFW_MOUSE_BUTTON_5      },
		{ NXTN_MOUSE_BUTTON_6      , GLFW_MOUSE_BUTTON_6      },
		{ NXTN_MOUSE_BUTTON_LEFT   , GLFW_MOUSE_BUTTON_LEFT   },
		{ NXTN_MOUSE_BUTTON_RIGHT  , GLFW_MOUSE_BUTTON_RIGHT  },
		{ NXTN_MOUSE_BUTTON_MIDDLE , GLFW_MOUSE_BUTTON_MIDDLE },

		/* --------- Printable Keys -------- */
		/* Numbers - Top Row */
		{ NXTN_KEY_0               , GLFW_KEY_0 },
		{ NXTN_KEY_1               , GLFW_KEY_1 },
		{ NXTN_KEY_2               , GLFW_KEY_2 },
		{ NXTN_KEY_3               , GLFW_KEY_3 },
		{ NXTN_KEY_4               , GLFW_KEY_4 },
		{ NXTN_KEY_5               , GLFW_KEY_5 },
		{ NXTN_KEY_6               , GLFW_KEY_6 },
		{ NXTN_KEY_7               , GLFW_KEY_7 },
		{ NXTN_KEY_8               , GLFW_KEY_8 },
		{ NXTN_KEY_9               , GLFW_KEY_9 },
		/* Numbers - Keypad */
		{ NXTN_KEY_KP_0            , GLFW_KEY_KP_0 },
		{ NXTN_KEY_KP_1            , GLFW_KEY_KP_1 },
		{ NXTN_KEY_KP_2            , GLFW_KEY_KP_2 },
		{ NXTN_KEY_KP_3            , GLFW_KEY_KP_3 },
		{ NXTN_KEY_KP_4            , GLFW_KEY_KP_4 },
		{ NXTN_KEY_KP_5            , GLFW_KEY_KP_5 },
		{ NXTN_KEY_KP_6            , GLFW_KEY_KP_6 },
		{ NXTN_KEY_KP_7            , GLFW_KEY_KP_7 },
		{ NXTN_KEY_KP_8            , GLFW_KEY_KP_8 },
		{ NXTN_KEY_KP_9            , GLFW_KEY_KP_9 },
		/* Space */
		{ NXTN_KEY_SPACE           , GLFW_KEY_SPACE },
		/* Special Characters */
		{ NXTN_KEY_GRAVE_ACCENT    , GLFW_KEY_GRAVE_ACCENT  },  /* ` ~ */
		{ NXTN_KEY_MINUS           , GLFW_KEY_MINUS         },  /* - _ */
		{ NXTN_KEY_EQUAL           , GLFW_KEY_EQUAL         },  /* = + */
		{ NXTN_KEY_LEFT_BRACKET    , GLFW_KEY_LEFT_BRACKET  },  /* [ { */
		{ NXTN_KEY_RIGHT_BRACKET   , GLFW_KEY_RIGHT_BRACKET },  /* ] } */
		{ NXTN_KEY_BACKSLASH       , GLFW_KEY_BACKSLASH     },  /* \ | */
		{ NXTN_KEY_SEMICOLON       , GLFW_KEY_SEMICOLON     },  /* ; : */
		{ NXTN_KEY_APOSTROPHE      , GLFW_KEY_APOSTROPHE    },  /* ' " */
		{ NXTN_KEY_COMMA           , GLFW_KEY_COMMA         },  /* , < */
		{ NXTN_KEY_PERIOD          , GLFW_KEY_PERIOD        },  /* . > */
		{ NXTN_KEY_SLASH           , GLFW_KEY_SLASH         },  /* / ? */
		{ NXTN_KEY_WORLD_1         , GLFW_KEY_WORLD_1       },  /* non-US #1 */
		{ NXTN_KEY_WORLD_2         , GLFW_KEY_WORLD_2       },  /* non-US #2 */
		/* Letters */
		{ NXTN_KEY_A               , GLFW_KEY_A },
		{ NXTN_KEY_B               , GLFW_KEY_B },
		{ NXTN_KEY_C               , GLFW_KEY_C },
		{ NXTN_KEY_D               , GLFW_KEY_D },
		{ NXTN_KEY_E               , GLFW_KEY_E },
		{ NXTN_KEY_F               , GLFW_KEY_F },
		{ NXTN_KEY_G               , GLFW_KEY_G },
		{ NXTN_KEY_H               , GLFW_KEY_H },
		{ NXTN_KEY_I               , GLFW_KEY_I },
		{ NXTN_KEY_J               , GLFW_KEY_J },
		{ NXTN_KEY_K               , GLFW_KEY_K },
		{ NXTN_KEY_L               , GLFW_KEY_L },
		{ NXTN_KEY_M               , GLFW_KEY_M },
		{ NXTN_KEY_N               , GLFW_KEY_N },
		{ NXTN_KEY_O               , GLFW_KEY_O },
		{ NXTN_KEY_P               , GLFW_KEY_P },
		{ NXTN_KEY_Q               , GLFW_KEY_Q },
		{ NXTN_KEY_R               , GLFW_KEY_R },
		{ NXTN_KEY_S               , GLFW_KEY_S },
		{ NXTN_KEY_T               , GLFW_KEY_T },
		{ NXTN_KEY_U               , GLFW_KEY_U },
		{ NXTN_KEY_V               , GLFW_KEY_V },
		{ NXTN_KEY_W               , GLFW_KEY_W },
		{ NXTN_KEY_X               , GLFW_KEY_X },
		{ NXTN_KEY_Y               , GLFW_KEY_Y },
		{ NXTN_KEY_Z               , GLFW_KEY_Z },

		/* ---------- Utility Keys --------- */
		/* Basic Utility Keys */
		{ NXTN_KEY_BACKSPACE       , GLFW_KEY_BACKSPACE     },
		{ NXTN_KEY_TAB             , GLFW_KEY_TAB           },
		{ NXTN_KEY_ENTER           , GLFW_KEY_ENTER         },
		{ NXTN_KEY_INSERT          , GLFW_KEY_INSERT        },
		{ NXTN_KEY_DELETE          , GLFW_KEY_DELETE        },
		{ NXTN_KEY_HOME            , GLFW_KEY_HOME          },
		{ NXTN_KEY_END             , GLFW_KEY_END           },
		{ NXTN_KEY_PAGE_UP         , GLFW_KEY_PAGE_UP       },
		{ NXTN_KEY_PAGE_DOWN       , GLFW_KEY_PAGE_DOWN     },
		{ NXTN_KEY_LEFT            , GLFW_KEY_LEFT          },
		{ NXTN_KEY_RIGHT           , GLFW_KEY_RIGHT         },
		{ NXTN_KEY_DOWN            , GLFW_KEY_DOWN          },
		{ NXTN_KEY_UP              , GLFW_KEY_UP            },
		{ NXTN_KEY_PAUSE           , GLFW_KEY_PAUSE         },
		{ NXTN_KEY_CAPS_LOCK       , GLFW_KEY_CAPS_LOCK     },
		{ NXTN_KEY_NUM_LOCK        , GLFW_KEY_NUM_LOCK      },
		{ NXTN_KEY_SCROLL_LOCK     , GLFW_KEY_SCROLL_LOCK   },
		{ NXTN_KEY_PRINT_SCREEN    , GLFW_KEY_PRINT_SCREEN  },
		{ NXTN_KEY_ESCAPE          , GLFW_KEY_ESCAPE        },
		/* Function Keys */
		{ NXTN_KEY_F1              , GLFW_KEY_F1            },
		{ NXTN_KEY_F2              , GLFW_KEY_F2            },
		{ NXTN_KEY_F3              , GLFW_KEY_F3            },
		{ NXTN_KEY_F4              , GLFW_KEY_F4            },
		{ NXTN_KEY_F5              , GLFW_KEY_F5            },
		{ NXTN_KEY_F6              , GLFW_KEY_F6            },
		{ NXTN_KEY_F7              , GLFW_KEY_F7            },
		{ NXTN_KEY_F8              , GLFW_KEY_F8            },
		{ NXTN_KEY_F9              , GLFW_KEY_F9            },
		{ NXTN_KEY_F10             , GLFW_KEY_F10           },
		{ NXTN_KEY_F11             , GLFW_KEY_F11           },
		{ NXTN_KEY_F12             , GLFW_KEY_F12           },
		{ NXTN_KEY_F13             , GLFW_KEY_F13           },
		{ NXTN_KEY_F14             , GLFW_KEY_F14           },
		{ NXTN_KEY_F15             , GLFW_KEY_F15           },
		{ NXTN_KEY_F16             , GLFW_KEY_F16           },
		{ NXTN_KEY_F17             , GLFW_KEY_F17           },
		{ NXTN_KEY_F18             , GLFW_KEY_F18           },
		{ NXTN_KEY_F19             , GLFW_KEY_F19           },
		{ NXTN_KEY_F20             , GLFW_KEY_F20           },
		{ NXTN_KEY_F21             , GLFW_KEY_F21           },
		{ NXTN_KEY_F22             , GLFW_KEY_F22           },
		{ NXTN_KEY_F23             , GLFW_KEY_F23           },
		{ NXTN_KEY_F24             , GLFW_KEY_F24           },
		{ NXTN_KEY_F25             , GLFW_KEY_F25           },
		{ NXTN_KEY_KP_DECIMAL      , GLFW_KEY_KP_DECIMAL    },
		{ NXTN_KEY_KP_ADD          , GLFW_KEY_KP_ADD        },
		{ NXTN_KEY_KP_SUBTRACT     , GLFW_KEY_KP_SUBTRACT   },
		{ NXTN_KEY_KP_MULTIPLY     , GLFW_KEY_KP_MULTIPLY   },
		{ NXTN_KEY_KP_DIVIDE       , GLFW_KEY_KP_DIVIDE     },
		{ NXTN_KEY_KP_EQUAL        , GLFW_KEY_KP_EQUAL      },
		{ NXTN_KEY_KP_ENTER        , GLFW_KEY_KP_ENTER      },
		{ NXTN_KEY_LEFT_SHIFT      , GLFW_KEY_LEFT_SHIFT    },
		{ NXTN_KEY_LEFT_CONTROL    , GLFW_KEY_LEFT_CONTROL  },
		{ NXTN_KEY_LEFT_SUPER      , GLFW_KEY_LEFT_SUPER    },  /* WIN for Windows | CMD for Mac */
		{ NXTN_KEY_LEFT_ALT        , GLFW_KEY_LEFT_ALT      },
		{ NXTN_KEY_RIGHT_SHIFT     , GLFW_KEY_RIGHT_SHIFT   },
		{ NXTN_KEY_RIGHT_CONTROL   , GLFW_KEY_RIGHT_CONTROL },
		{ NXTN_KEY_RIGHT_SUPER     , GLFW_KEY_RIGHT_SUPER   },  /* WIN for Windows | CMD for Mac */
		{ NXTN_KEY_RIGHT_ALT       , GLFW_KEY_RIGHT_ALT     },
		{ NXTN_KEY_MENU            , GLFW_KEY_MENU          }
	};
}