#include ".\..\QcCompileConfig.hpp"
#include <Windows.h>
#include <shlwapi.h>
#include <string>
#include <vector>
#include <xutility>
#include "tinyxml.h"
#include <fstream>
#include <algorithm>
#include <shlobj.h>
#include <map>
#include <fcntl.h>
#include <io.h>
#include <xiosbase>
#include <time.h>
#include <deque>
#include "ImageLoader.h"
#include "QcCore.hpp"
#pragma comment(lib, "QcCore.lib")

//#pragma comment(lib, "libtcmalloc_minimal.lib")
//#pragma comment(linker, "/include:__tcmalloc")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "DbgHelp.lib")
#pragma comment(lib,"Psapi.lib")
#pragma comment(lib, "shlwapi.lib")
#define QmMutilThreadOn
#define QmThreadNum (3)
#ifndef QmMutilThreadOn
//#define QmQImageOn
#endif

