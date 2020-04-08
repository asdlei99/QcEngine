#pragma once

#ifndef WINVER                          // ָ��Ҫ������ƽ̨�� Windows Vista��
#define WINVER 0x0500           // ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#endif

#ifndef _WIN32_WINNT            // ָ��Ҫ������ƽ̨�� Windows Vista��
#define _WIN32_WINNT 0x0500     // ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#endif

#ifndef _WIN32_WINDOWS          // ָ��Ҫ������ƽ̨�� Windows 98��
#define _WIN32_WINDOWS 0x0410 // ����ֵ����Ϊ�ʵ���ֵ���������� Windows Me ����߰汾��
#endif

#ifndef _WIN32_IE                       // ָ��Ҫ������ƽ̨�� Internet Explorer 7.0��
#define _WIN32_IE 0x0700        // ����ֵ����Ϊ��Ӧ��ֵ���������� IE �������汾��
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
