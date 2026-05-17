#pragma once

#include <cstdint>

namespace NXTN {
	enum class MouseButtonCode : int32_t
	{
		Button0 = 0,   // Left
		Button1 = 1,   // Right
		Button2 = 2,   // Middle
		Button3 = 3,
		Button4 = 4,
		Button5 = 5,
		Button6 = 6,

		Left = Button0,
		Right = Button1,
		Middle = Button2
	};

	enum class KeyCode : int32_t
	{
		Unknown = -1,

		// Numbers - Top Row
		Num0 = 30,
		Num1 = 31,
		Num2 = 32,
		Num3 = 33,
		Num4 = 34,
		Num5 = 35,
		Num6 = 36,
		Num7 = 37,
		Num8 = 38,
		Num9 = 39,

		// Numbers - Keypad
		Kp0 = 40,
		Kp1 = 41,
		Kp2 = 42,
		Kp3 = 43,
		Kp4 = 44,
		Kp5 = 45,
		Kp6 = 46,
		Kp7 = 47,
		Kp8 = 48,
		Kp9 = 49,

		Space = 50,

		// Special Characters
		GraveAccent = 51, // ` ~
		Minus = 52, // - _
		Equal = 53, // = +
		LeftBracket = 54, // [ {
		RightBracket = 55, // ] }
		Backslash = 56, // \ |
		Semicolon = 57, // ; :
		Apostrophe = 58, // ' "
		Comma = 59, // , <
		Period = 60, // . >
		Slash = 61, // / ?
		World1 = 62, // non-US #1
		World2 = 63, // non-US #2

		// Letters
		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,

		// Utility Keys
		Backspace = 110,
		Tab = 111,
		Enter = 112,
		Insert = 113,
		Delete = 114,
		Home = 115,
		End = 116,
		PageUp = 117,
		PageDown = 118,
		Left = 119,
		Right = 120,
		Down = 121,
		Up = 122,
		Pause = 123,
		CapsLock = 124,
		NumLock = 125,
		ScrollLock = 126,
		PrintScreen = 127,
		Escape = 128,

		// Function Keys
		F1 = 171,
		F2 = 172,
		F3 = 173,
		F4 = 174,
		F5 = 175,
		F6 = 176,
		F7 = 177,
		F8 = 178,
		F9 = 179,
		F10 = 180,
		F11 = 181,
		F12 = 182,
		F13 = 183,
		F14 = 184,
		F15 = 185,
		F16 = 186,
		F17 = 187,
		F18 = 188,
		F19 = 189,
		F20 = 190,
		F21 = 191,
		F22 = 192,
		F23 = 193,
		F24 = 194,
		F25 = 195,

		KpDecimal = 200,
		KpAdd = 201,
		KpSubtract = 202,
		KpMultiply = 203,
		KpDivide = 204,
		KpEqual = 205,
		KpEnter = 206,

		LeftShift = 210,
		LeftControl = 211,
		LeftSuper = 212,
		LeftAlt = 213,
		RightShift = 214,
		RightControl = 215,
		RightSuper = 216,
		RightAlt = 217,
		Menu = 218
	};
}

/* -------- The Unknown Key ------- */
#define NXTN_KEY_UNKNOWN            -1

/* --------- Mouse Buttons -------- */
#define NXTN_MOUSE_BUTTON_0         0  // Left
#define NXTN_MOUSE_BUTTON_1         1  // Right
#define NXTN_MOUSE_BUTTON_2         2  // Middle
#define NXTN_MOUSE_BUTTON_3         3
#define NXTN_MOUSE_BUTTON_4         4
#define NXTN_MOUSE_BUTTON_5         5
#define NXTN_MOUSE_BUTTON_6         6
#define NXTN_MOUSE_BUTTON_LEFT      NXTN_MOUSE_BUTTON_0
#define NXTN_MOUSE_BUTTON_RIGHT     NXTN_MOUSE_BUTTON_1
#define NXTN_MOUSE_BUTTON_MIDDLE    NXTN_MOUSE_BUTTON_2

/* --------- Printable Keys -------- */
/* Numbers - Top Row */
#define NXTN_KEY_0                  30
#define NXTN_KEY_1                  31
#define NXTN_KEY_2                  32
#define NXTN_KEY_3                  33
#define NXTN_KEY_4                  34
#define NXTN_KEY_5                  35
#define NXTN_KEY_6                  36
#define NXTN_KEY_7                  37
#define NXTN_KEY_8                  38
#define NXTN_KEY_9                  39
/* Numbers - Keypad */
#define NXTN_KEY_KP_0               40
#define NXTN_KEY_KP_1               41
#define NXTN_KEY_KP_2               42
#define NXTN_KEY_KP_3               43
#define NXTN_KEY_KP_4               44
#define NXTN_KEY_KP_5               45
#define NXTN_KEY_KP_6               46
#define NXTN_KEY_KP_7               47
#define NXTN_KEY_KP_8               48
#define NXTN_KEY_KP_9               49
/* Space */
#define NXTN_KEY_SPACE              50
/* Special Characters */
#define NXTN_KEY_GRAVE_ACCENT       51  /* ` ~ */
#define NXTN_KEY_MINUS              52  /* - _ */
#define NXTN_KEY_EQUAL              53  /* = + */
#define NXTN_KEY_LEFT_BRACKET       54  /* [ { */
#define NXTN_KEY_RIGHT_BRACKET      55  /* ] } */
#define NXTN_KEY_BACKSLASH          56  /* \ | */
#define NXTN_KEY_SEMICOLON          57  /* ; : */
#define NXTN_KEY_APOSTROPHE         58  /* ' " */
#define NXTN_KEY_COMMA              59  /* , < */
#define NXTN_KEY_PERIOD             60  /* . > */
#define NXTN_KEY_SLASH              61  /* / ? */
#define NXTN_KEY_WORLD_1            62  /* non-US #1 */
#define NXTN_KEY_WORLD_2            63  /* non-US #2 */
/* Letters */
#define NXTN_KEY_A                  65
#define NXTN_KEY_B                  66
#define NXTN_KEY_C                  67
#define NXTN_KEY_D                  68
#define NXTN_KEY_E                  69
#define NXTN_KEY_F                  70
#define NXTN_KEY_G                  71
#define NXTN_KEY_H                  72
#define NXTN_KEY_I                  73
#define NXTN_KEY_J                  74
#define NXTN_KEY_K                  75
#define NXTN_KEY_L                  76
#define NXTN_KEY_M                  77
#define NXTN_KEY_N                  78
#define NXTN_KEY_O                  79
#define NXTN_KEY_P                  80
#define NXTN_KEY_Q                  81
#define NXTN_KEY_R                  82
#define NXTN_KEY_S                  83
#define NXTN_KEY_T                  84
#define NXTN_KEY_U                  85
#define NXTN_KEY_V                  86
#define NXTN_KEY_W                  87
#define NXTN_KEY_X                  88
#define NXTN_KEY_Y                  89
#define NXTN_KEY_Z                  90

/* ---------- Utility Keys --------- */
/* Basic Utility Keys */
#define NXTN_KEY_BACKSPACE          110
#define NXTN_KEY_TAB                111
#define NXTN_KEY_ENTER              112
#define NXTN_KEY_INSERT             113
#define NXTN_KEY_DELETE             114
#define NXTN_KEY_HOME               115
#define NXTN_KEY_END                116
#define NXTN_KEY_PAGE_UP            117
#define NXTN_KEY_PAGE_DOWN          118
#define NXTN_KEY_LEFT               119
#define NXTN_KEY_RIGHT              120
#define NXTN_KEY_DOWN               121
#define NXTN_KEY_UP                 122
#define NXTN_KEY_PAUSE              123
#define NXTN_KEY_CAPS_LOCK          124
#define NXTN_KEY_NUM_LOCK           125
#define NXTN_KEY_SCROLL_LOCK        126
#define NXTN_KEY_PRINT_SCREEN       127
#define NXTN_KEY_ESCAPE             128
/* Function Keys */
#define NXTN_KEY_F1                 171
#define NXTN_KEY_F2                 172
#define NXTN_KEY_F3                 173
#define NXTN_KEY_F4                 174
#define NXTN_KEY_F5                 175
#define NXTN_KEY_F6                 176
#define NXTN_KEY_F7                 177
#define NXTN_KEY_F8                 178
#define NXTN_KEY_F9                 179
#define NXTN_KEY_F10                180
#define NXTN_KEY_F11                181
#define NXTN_KEY_F12                182
#define NXTN_KEY_F13                183
#define NXTN_KEY_F14                184
#define NXTN_KEY_F15                185
#define NXTN_KEY_F16                186
#define NXTN_KEY_F17                187
#define NXTN_KEY_F18                188
#define NXTN_KEY_F19                189
#define NXTN_KEY_F20                190
#define NXTN_KEY_F21                191
#define NXTN_KEY_F22                192
#define NXTN_KEY_F23                193
#define NXTN_KEY_F24                194
#define NXTN_KEY_F25                195
#define NXTN_KEY_KP_DECIMAL         200
#define NXTN_KEY_KP_ADD             201
#define NXTN_KEY_KP_SUBTRACT        202
#define NXTN_KEY_KP_MULTIPLY        203
#define NXTN_KEY_KP_DIVIDE          204
#define NXTN_KEY_KP_EQUAL           205
#define NXTN_KEY_KP_ENTER           206
#define NXTN_KEY_LEFT_SHIFT         210
#define NXTN_KEY_LEFT_CONTROL       211
#define NXTN_KEY_LEFT_SUPER         212  /* WIN for Windows | CMD for Mac */
#define NXTN_KEY_LEFT_ALT           213
#define NXTN_KEY_RIGHT_SHIFT        214
#define NXTN_KEY_RIGHT_CONTROL      215
#define NXTN_KEY_RIGHT_SUPER        216  /* WIN for Windows | CMD for Mac */
#define NXTN_KEY_RIGHT_ALT          217
#define NXTN_KEY_MENU               218
