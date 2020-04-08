#pragma once

class QcUtil
{
public:
	static u32 StringToPointList(const char* pStr, std::vector<QcVector2df>& pointList, const char* firstSplitCh="(", const char* secondSplitCh=",");
	static u32 StringToPointList(const char* pStr, std::vector<QcVector3df>& pointList, const char* firstSplitCh="(", const char* secondSplitCh=",");

};