#pragma once

#ifndef WINVER                          // 指定要求的最低平台是 Windows Vista。
#define WINVER 0x0500           // 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif

#ifndef _WIN32_WINNT            // 指定要求的最低平台是 Windows Vista。
#define _WIN32_WINNT 0x0500     // 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif

#ifndef _WIN32_WINDOWS          // 指定要求的最低平台是 Windows 98。
#define _WIN32_WINDOWS 0x0410 // 将此值更改为适当的值，以适用于 Windows Me 或更高版本。
#endif

#ifndef _WIN32_IE                       // 指定要求的最低平台是 Internet Explorer 7.0。
#define _WIN32_IE 0x0700        // 将此值更改为相应的值，以适用于 IE 的其他版本。
#endif

#define _QmToStr(str) #str
#define QmToStr(str) _QmToStr(str)

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <windows.h>
#include <atlconv.h>
#if _WIN32_WINNT < 0x0600
#include <winternl.h>
#endif
#ifndef __out_xcount
#define __out_xcount(x) // Workaround for the specstrings.h bug in the Platform SDK.
#endif
#define DBGHELP_TRANSLATE_TCHAR
#include <dbghelp.h>    // Provides portable executable (PE) image access functions.
