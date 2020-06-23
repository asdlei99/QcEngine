#pragma once

#include ".\..\QcCompileConfig.hpp"

#include <windows.h>
#include <process.h>
#include <tchar.h>
#include <iosfwd>
#include <errno.h>
#include <dbghelp.h>
#include <shlwapi.h>
#include <atlconv.h>
#include <time.h>
#include <stdio.h>
#include <Psapi.h>
#include <signal.h>
#include <fcntl.h>
#include <io.h>

#include <iosfwd>
#include <sstream>
#include <fstream>
#include <strstream>
#include <unordered_map>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <deque>
#include <functional>
#include <algorithm>
using namespace std;
 
#include "QcCore.hpp"
#pragma comment(lib, "QcCore.lib")

#include "DXUT.h"
#ifdef QmBoostOn
#include "boost\any.hpp"
#include "boost\function.hpp"
#include "boost\bind.hpp"
#endif
#include "fmod.hpp"
#include "ImageLoader.h"

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "DbgHelp.lib")
#pragma comment(lib,"Psapi.lib")
#pragma comment(lib,"DXUT.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "magic.lib")
#pragma comment(lib, "fmodex_vc_dll.lib")

#ifdef QmMemLeakOn
#include "vld.h"
#endif

#ifndef QmIOUsbOn
#undef QmHardAlg
#endif
#include ".\..\new_usb_pc\Include\USB_IO.h"
#ifdef QmIOUsbOn
#pragma comment(lib,"new_usb_pc.lib")
#endif

#include "CatchFish.hpp"