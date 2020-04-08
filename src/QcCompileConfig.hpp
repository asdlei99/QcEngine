#pragma once

//#define QmReadLockerOn
//#define QmTcMallocOn
//#define QmFastMathTableOn
#define QmQcMatrix3On
#define QmThreadDeadLoopCheckOn
#define QmBoostOn

#if defined(DEBUG) || defined(_DEBUG)
#ifndef QmTestVersion
#define QmTestVersion
#endif
//#define QmMemLeakOn
#else
#define QmDumpReportOn
#endif

#ifdef QmMemLeakOn
#undef QmTcMallocOn
#endif

#ifdef QmTestVersion
#define QmEfficiencyProfilerOn
#define QmAssertOn
#define QmLogOn
#define QmDXFontOn
#define QmAlgIOOn
#else
//#define QmIOUsbOn
//#define QmRebootOn
#endif

#ifndef QmLogOn
//#define _SECURE_SCL 0
//#define _SECURE_SCL_THROWS 0
#endif

