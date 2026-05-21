#pragma once

#ifdef NXTN_PLATFORM_WINDOWS
#define NOMINMAX
#include <Windows.h>
#endif // NXTN_PLATFORM_WINDOWS

// Utility
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <concepts>
#include <functional>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
//#include <regex>
#include <sstream>
#include <typeindex>
#include <utility>

// Data structure
#include <array>
#include <deque>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Misc
#include <cstddef>
#include <limits>