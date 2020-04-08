#include "pch.hpp"
#include "QcPathMgr.hpp"
#include "QcResPathDesc.hpp"

QcPathMgr::QcPathMgr()
{

}

void QcPathMgr::Parse(const QcXmlNodeIterator& pRootNode, void* pParam)
{
	const QcXmlNodeIterator& topNode = pRootNode.FirstNode();
	if (!topNode)
		return;
	LoadPath(topNode.FirstNode("NormalScene"), 0);
	LoadPath(topNode.FirstNode("SceneA"), QmArrayFind(QaSceneList, QmObjName(SceneA)) + 1);
	LoadPath(topNode.FirstNode("SceneB"), QmArrayFind(QaSceneList, QmObjName(SceneB)) + 1);
	LoadPath(topNode.FirstNode("SceneC"), QmArrayFind(QaSceneList, QmObjName(SceneC)) + 1);
}

void QcPathMgr::LoadPath(const QcXmlNodeIterator& sceneNode, u32 nSceneId)
{
	LoadPath(sceneNode.FirstNode("SmallFishPath"), nSceneId, eSmallFishPath);
	LoadPath(sceneNode.FirstNode("MiddleFishPath"), nSceneId, eMiddleFishPath);
	LoadPath(sceneNode.FirstNode("LargeFishPath"), nSceneId, eLargeFishPath);
	LoadPath(sceneNode.FirstNode("RegularPath"), nSceneId, eRegularPath);
	LoadPath(sceneNode.FirstNode("GroupPath"), nSceneId, eGroupPath);
	LoadPath(sceneNode.FirstNode("GroupPath2"), nSceneId, eGroupPath2);
}

void QcPathMgr::LoadPath(const QcXmlNodeIterator& pathNode, u32 nSceneId, QePathType eType)
{
	if (pathNode)
	{
		QcList<QcResPathDesc*>& pathList = GetPathList(nSceneId, eType);
		QcXmlNodeIterator node = pathNode.FirstNode();
		while (node)
		{
			QcResPathDescPtr pPathDesc(new QcResPathDesc());
			if (pPathDesc->Load(node) == pPathDesc)
			{
				pPathDesc->m_iPathType = nSceneId * ePathCount + eType;
				pathList.PushBack(pPathDesc.Get());
			}
			node = node.NextSibling();
		}
	}
}

QcResPathDesc* QcPathMgr::LoadPathDesc(QcResPathDesc* pPathDesc)
{
	const QcResPathDesc* pDestPathDesc = GetPathDesc(pPathDesc->GetObjType());
	if (pDestPathDesc == NULL)
	{
		pPathDesc->LoadPath();
		m_pathMap[pPathDesc->GetObjType()] = pPathDesc;
		pDestPathDesc = pPathDesc;
	}
	return const_cast<QcResPathDesc*>(pDestPathDesc);
}

QcResPathDesc* QcPathMgr::GetUsablePathDesc(QePathType eType)	
{
	return FindUsablePathDesc(GetNormalPathList(eType));
}

QcResPathDesc* QcPathMgr::GetUsableScenePathDesc(u32 nSceneId, QePathType eType)
{
	return FindUsablePathDesc(GetPathList(nSceneId + 1, eType));
}

QcResPathDesc* QcPathMgr::FindUsablePathDesc(const QcList<QcResPathDesc*>& pathList)
{
	QcResPathDesc* pRet = NULL;
	int index = RandomInt(0, pathList.size());
	if (pathList.size() > 0)
	{
		int iBeginIndex = index;
		pRet = pathList[index];
		for (; index < (int)pathList.size(); ++index)
		{
			if (pathList[index]->IsUsable() == 0)
			{
				pRet = pathList[index];
				break;
			}
		}
		if (index == (int)pathList.size())
		{
			index = 0;
			for (; index < iBeginIndex; ++index) 
			{
				if (pathList[index]->IsUsable() == 0)
				{
					pRet = pathList[index];
					break;
				}
			}
		}
	}
	return const_cast<QcResPathDesc*>(pRet);
}

QcResPathDesc* QcPathMgr::GetPathDesc(const QcObjType& objType)
{
	QcResPathDesc* pDesc = NULL;
	std::map<QcObjType, QcSharedPtr<QcResPathDesc>>::iterator iter(m_pathMap.find(objType));
	if (iter != m_pathMap.end())
		pDesc = iter->second.Get();
	return pDesc;
}
