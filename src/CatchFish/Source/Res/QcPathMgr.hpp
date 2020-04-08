#pragma once

enum QePathType
{
	eSmallFishPath = 0,
	eMiddleFishPath = 1,
	eLargeFishPath = 2,
	eRegularPath = 3,
	eGroupPath = 4,
	eGroupPath2 = 5,
	ePathCount = 6,
};

#include "Common\QcEncryptXmlLoader.hpp"
class QcResPathDesc;
class QcPathMgr : public QcEncryptXmlLoader
{
	friend class QcDebugHelper;
public:
	QcPathMgr();
	QcResPathDesc* LoadPathDesc(QcResPathDesc* pPathDesc);

	QcResPathDesc* GetPathDesc(const QcObjType& objType);
	QcResPathDesc* GetUsablePathDesc(QePathType eType);
	QcResPathDesc* GetUsableScenePathDesc(u32 nSceneId, QePathType eType);
	QcList<QcResPathDesc*>& GetNormalPathList(QePathType eType){return GetPathList(0, eType);}
	QcList<QcResPathDesc*>& GetScenePathList(u32 nSceneId, QePathType eType){ return GetPathList(nSceneId + 1, eType);}

	static QcResPathDesc* FindUsablePathDesc(const QcList<QcResPathDesc*>& pathList);
protected:
	virtual void Parse(const QcXmlNodeIterator& pRootNode, void* pParam);
	void LoadPath(const QcXmlNodeIterator& pathNode, u32 nSceneId);
	void LoadPath(const QcXmlNodeIterator& pathNode, u32 nSceneId, QePathType eType);
	QcList<QcResPathDesc*>& GetPathList(u32 nSceneId, QePathType eType){ return m_arFishPath[eType + nSceneId * ePathCount];}
private:
	QcList<QcResPathDesc*> m_arFishPath[ePathCount * (QmCountOf(QaSceneList) + 1)];
	std::map<QcObjType, QcResPathDescPtr> m_pathMap;
};