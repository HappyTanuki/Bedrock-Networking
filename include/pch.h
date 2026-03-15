#pragma once

#ifdef _WIN32
// Minimize windows.h footprint
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

// OpenSSL heavy header included transitively by encryption interfaces
#include <openssl/evp.h>

// Common STL headers used across the project
#include <string>
#include <vector>
#include <memory>
#include <span>
#include <cstdint>
#include <array>
