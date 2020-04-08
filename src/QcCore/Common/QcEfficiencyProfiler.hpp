#pragma once

struct QsProfilerData;
#ifdef QmBoostOn
typedef boost::function<void(const QsProfilerData* data, int nCount)> ProfilerPrinter;
#else
typedef void (*ProfilerPrinter)(const QsProfilerData* data, int nCount);
#endif

struct QsProfilerData
{
	QsProfilerData(int id, const char* psName)
		: _Id(id)
		, _iCount(0)
		, _iTotalTime(0)
		, _iMaxTime(1)
		, _iMinTime(0x7fffffffffffffff)
		, _psName(psName)
	{}
	int _Id;
	const char* _psName;
	int _iCount;
	u64 _iTotalTime;
	u64 _iMaxTime;
	u64 _iMinTime;
};

typedef std::vector<QsProfilerData> ProfilerDataList;

#ifdef QmEfficiencyProfilerOn
inline void DefaultPrinter(const QsProfilerData* data, int nCount)
{
	for (int i=0; i<nCount; ++i, ++data)
	{
		if (data->_iCount)
		{
			printf("***********************%s************************\n", data->_psName);
			printf("CallCount:%d\n", data->_iCount);
			printf("CallTotalTm:%.4f(ms)\n", QfCPUTicksToMS(data->_iTotalTime));
			printf("MaxTm:%.4f(ms)\n", QfCPUTicksToMS(data->_iMaxTime));
			printf("AverTm:%.4f(ms)\n", QfCPUTicksToMS(data->_iTotalTime)/data->_iCount);
		}
	}
}
class QcProfilerMgr
{
	QcProfilerMgr()
		: m_bProfilerOn(false)
	{
		_profilerDataList.reserve(100);
	}

	QsProfilerData* GetProfilerData(int id)
	{
		if (id <= 0 || id > (int)_profilerDataList.size())
			return NULL;
		return &_profilerDataList[id-1];
	}
	ProfilerDataList _profilerDataList;
	bool m_bProfilerOn;
public:
	static QcProfilerMgr& Get()
	{
		static QcProfilerMgr gsProfilerMgr;
		return gsProfilerMgr;
	}
	bool FragmentBegin(int& id, const char* psName)
	{
		if (id == 0)
		{
			ProfilerDataList::iterator iter(_profilerDataList.begin());
			ProfilerDataList::iterator endIter(_profilerDataList.end());
			for (; iter != endIter; ++iter)
			{
				if (strcmp(psName, iter->_psName) == 0)
				{
					break;
				}
			}
			if (iter == endIter)
			{
				id = _profilerDataList.size() + 1;
				_profilerDataList.push_back(QsProfilerData(id, psName));
			}
			else
				id = iter->_Id;
		}
		return m_bProfilerOn;
	}
	void FragmentEnd(int id, u64 iTm)
	{
		QsProfilerData* profiler = GetProfilerData(id);
		if (profiler)
		{
			++profiler->_iCount;
			profiler->_iTotalTime += iTm;
			profiler->_iMaxTime = max(profiler->_iMaxTime, iTm);
			profiler->_iMinTime = min(profiler->_iMinTime, iTm);
		}
	}
	void ProfileOn()
	{
		if (!m_bProfilerOn)
		{
			SetThreadAffinityMask(GetCurrentThread(), 0x00000002);
			m_bProfilerOn = true;
			ProfilerDataList::iterator iter = _profilerDataList.begin();
			ProfilerDataList::iterator endIter = _profilerDataList.end();
			for (; iter != endIter; ++iter)
			{
				iter->_iCount = 0;
				iter->_iTotalTime = 0;
				iter->_iMaxTime = 0;
				iter->_iMinTime = 0x7fffffffffffffff;
			}
		}
	}
	void ProfileOff()
	{
		if (m_bProfilerOn)
		{
			m_bProfilerOn = false;
		}
	}
	void GetProfilerData(ProfilerDataList& dataList)
	{
		dataList = _profilerDataList;
	}
	void Print(const ProfilerPrinter& printer)
	{
		int nSize = _profilerDataList.size();
		if (nSize)
		{
			const QsProfilerData* pData = &_profilerDataList[0];
			if (printer)
			{
				printer(pData, nSize);
			}
			else
			{
				DefaultPrinter(&_profilerDataList[0], nSize);
			}
		}
	}
};

class QcProfilerHelper
{
	QcProfilerHelper(const QcProfilerHelper&);
	void operator=(const QcProfilerHelper&);
public:
	explicit QcProfilerHelper(int& id, const char* psName)
		: m_id(0)
	{
		if (QcProfilerMgr::Get().FragmentBegin(id, psName))
		{
			m_id = id;
			m_iTm = QfGetCPUTicks();
		}
	}
	~QcProfilerHelper()
	{
		if (m_id)
		{
			m_iTm = QfGetCPUTicks() - m_iTm;
			QcProfilerMgr::Get().FragmentEnd(m_id, m_iTm);
		}
	}
private:
	u64 m_iTm;
	int m_id;
};

#define __QmProfileFragment(name, line)\
	static int ___id_##line = 0;\
	QcProfilerHelper ___obj_##line(___id_##line, #name);
#define _QmProfileFragment(name, line) __QmProfileFragment(name, line)
#define QmProfileFragment(name) _QmProfileFragment(name, __LINE__)

#define QmProfileOn() QcProfilerMgr::Get().ProfileOn()
#define QmProfileOff() QcProfilerMgr::Get().ProfileOff()
#define QmGetProfilerData(dataList) QcProfilerMgr::Get().GetProfilerData(dataList)
#define QmPrintData(printer) QcProfilerMgr::Get().Print(printer)
#else
#define QmProfileFragment(name) ((void)0)
#define QmProfileOn() ((void)0)
#define QmProfileOff() ((void)0)
#define QmGetProfilerData(dataList) ((void)0)
#define QmPrintData(printer) ((void)0)
#endif