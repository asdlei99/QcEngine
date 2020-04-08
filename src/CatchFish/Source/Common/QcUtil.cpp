#include "pch.hpp"
#include "QcUtil.hpp"


u32 QcUtil::StringToPointList(const char* pStr, std::vector<QcVector2df>& pointList, const char* firstSplitCh, const char* secondSplitCh)
{
	const char* p = strstr(pStr, firstSplitCh);
	while(p)
	{
		++p;
		f32 x = (f32)atof(p);
		f32 y = 0.f;
		const char* pp = strstr(p, secondSplitCh);
		if (pp)
		{
			++pp;
			p = pp;
			y = (f32)atof(pp);
		}
		pointList.push_back(QcVector2df(x, y));
		p = strstr(p, "(");
	}
	return pointList.size();
}

u32 QcUtil::StringToPointList(const char* pStr, std::vector<QcVector3df>& pointList, const char* firstSplitCh, const char* secondSplitCh)
{
	const char* p = strstr(pStr, firstSplitCh);
	while(p)
	{
		++p;
		f32 x = (f32)atof(p);
		const char* pp = strstr(p, secondSplitCh);
		if (pp)
		{
			++pp;
			p = pp;
			f32 y = (f32)atof(pp);
			f32 z = 0.f;
			const char* ppp = strstr(pp, secondSplitCh);
			if (ppp)
			{
				++ppp;
				p = ppp;
				z = (f32)atof(ppp);
			}
			pointList.push_back(QcVector3df(x, y, z));
		}
		p = strstr(p, "(");
	}
	return pointList.size();
}
