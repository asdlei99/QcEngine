#pragma once

#include ".\..\QcCompileConfig.hpp"

#include <windows.h>
#include <ShlObj.h>
#include <process.h>
#include <tchar.h>
#include <iosfwd>
#include <errno.h>
#include <dbghelp.h>
#include <shlwapi.h>
#include <atlconv.h>
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <Psapi.h>
#include <signal.h>
#include <fcntl.h>
#include <io.h>

#include <type_traits>
#include <iosfwd>
#include <sstream>
#include <fstream>
#include <strstream>
#include <hash_map>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <deque>
#include <functional>
#include <algorithm>
#include <new.h>
using namespace std;
 
#ifdef QmBoostOn
#include "boost\any.hpp"
#include "boost\function.hpp"
#include "boost\bind.hpp"
#endif

#ifdef QmMemLeakOn
#include "vld.h"
#endif

#ifdef QmTcMallocOn
#pragma comment(lib, "libtcmalloc_minimal.lib")
#pragma comment(linker, "/include:__tcmalloc")
#endif

#include "QcCore.hpp"