#include "pch.hpp"
#include "QcResImageDesc.hpp"
#include "Common\QcUtil.hpp"
#include "Collision\QcRegionMulti.hpp"
#include "Collision\QcRegionCircle.hpp"
#include "Collision\QcRegionPolygon.hpp"
#include "Collision\QcRegionMultiCircle.hpp"
#include "Scene\QcSceneNode.hpp"
#include "QcResManager.hpp"
#include "QcTexture.hpp"

ImplRuntimeClass2(QcResImageDesc, 500);

QcResImageDesc::QcResImageDesc()
: m_fWidth(0.f)
, m_fHeight(0.f)
, m_fRotation(0.f)
, m_iForceTexInVideoMem(0)
{
	m_pRuntimeClass = QmRuntimeClass(QcSceneNode);
}

QcResImageDesc::~QcResImageDesc()
{
}

bool QcResImageDesc::LoadDesc(const char* psObjType, const char* psPrefix, const char* psFileExt)
{
	m_objType = psObjType;
	if (psPrefix == NULL)
		m_sPrefix = psObjType;
	else
		m_sPrefix = psPrefix;

	if (psFileExt == NULL)
		m_sFileExt = "png";
	else
		m_sFileExt = psFileExt;

	LoadAttibute(QcXmlNodeIterator());

	if (QmResMgr->GetImageDesc(m_objType) == NULL)
	{
		LoadStateDesc(QcXmlNodeIterator());
	}
	return true;
}

bool QcResImageDesc::LoadDesc(const QcXmlNodeIterator& node)
{
	if (m_objType.empty())
		m_objType = node.Attribute("name");

	LoadAttibute(node.FirstNode("Attribute"));

	if (m_sFileExt.size() == 0)
		m_sFileExt = "png";
	if (m_sPrefix.size() == 0)
		m_sPrefix = m_objType;

	//加载State.
	if (QmResMgr->GetImageDesc(m_objType) == NULL)
		LoadStateDesc(node);

	//加载附加参数
	QcResDesc::LoadDesc(node);

	LoadSubDesc(node);

	m_pSourceRegion = LoadRegion(node);

	return true;
}

bool QcResImageDesc::LoadAttibute(const QcXmlNodeIterator& node)
{
	if (m_sPrefix.empty())
		m_sPrefix = node.Attribute("Prefix");
	if (m_sFileExt.empty())
		m_sFileExt = node.Attribute("fileext");

	m_fWidth = /*node.Attribute("Width", 0.0f)*/0.f;
	m_fHeight = /*node.Attribute("Height", 0.0f)*/0.f;
	m_vPos = node.Attribute("position", QcPoint(0.f, 0.f));
	m_fRotation = QcMath::degToRad(node.Attribute("Rotation", 0.0f));
	m_sEffectRender = node.Attribute("Render");
	m_iForceTexInVideoMem = node.Attribute("ForceInVideo", 1);

	const char* pChar = node.Attribute("class");
	if (strlen(pChar) > 0)
	{
		QcRuntimeClass* pRuntimeClass = QmClassFactoryInstance->Find(pChar);
		QmAssert(pRuntimeClass != NULL);
		m_pRuntimeClass = pRuntimeClass;
	}
	return true;
}


bool QcResImageDesc::LoadSubDesc(const QcXmlNodeIterator& node)
{
	//加载子对象
	QcXmlNodeIterator subNode = node.FirstNode("subObject");
	if (subNode)
	{
		QcRuntimeClass* pBaseRuntimeClass = QmRuntimeClass(QcResImageDesc);
		const char* pSubClass = subNode.Attribute("class");
		if (strlen(pSubClass) >= 1)
			pBaseRuntimeClass = QmClassFactoryInstance->Find(pSubClass);

		subNode = subNode.FirstNode();
		while (subNode)
		{
			QcResImageDesc* pSubDesc = NULL;
			pSubDesc = const_cast<QcResImageDesc*>(QmResMgr->GetImageDesc(subNode.SubValue("name")));
			if (pSubDesc == NULL)
			{
				const char* pChar = node.Attribute("class");
				QcRuntimeClass* pRuntimeClass = pBaseRuntimeClass;
				if (strlen(pChar) >= 1)
				{
					pRuntimeClass = QmClassFactoryInstance->Find(pChar);
				}
				pSubDesc = pRuntimeClass->CreateObject()->As<QcResImageDesc*>();
				pSubDesc->LoadDesc(subNode);
			}
			m_arSubList.PushBack(pSubDesc);
			subNode = subNode.NextSibling();
		}
	}
	return m_arSubList.size() > 0;
}

void QcResImageDesc::LoadStateDesc(const QcXmlNodeIterator& node)
{
	bool bLoadState = false;
	if (node)
	{
		QcXmlNodeIterator stateElment = node.FirstNode("states");
		if (stateElment)
		{
			stateElment = stateElment.FirstNode();
			while (stateElment)
			{
				f32 fTime = 0.f;
				int bCycle = 0;
				int iFPS = 0;
				const char* psStateName = stateElment.Attribute("name");
				fTime = stateElment.Attribute("time", 1.0f);
				bCycle = stateElment.Attribute("iscycle", 1);
				iFPS = stateElment.Attribute("FPS", 0);
				if(AddState(StringToState(psStateName), fTime, bCycle ? true : false, iFPS))
				{
					m_arState.back().m_arFrameRegionList = LoadFrameRegionList(stateElment);
				}
				stateElment = stateElment.NextSibling();
				bLoadState = true;
			}
		}
	}
	if (GetStateParameter(eLiving) == NULL)
		AddState(eLiving, 1.0f, true);
	if (GetStateParameter(eDying) == NULL)
		AddState(eDying, 2.0f, false);
	//若没有指定状态列表，则搜索文件自动产生状态列表
}

QcRegionAutoPtr QcResImageDesc::LoadRegion(const QcXmlNodeIterator& node)
{
	QcRegionAutoPtr pRet;
	QcXmlNodeIterator borderNode = node.FirstNode("border");
	if (borderNode)
	{
		//新的描述方式
		pRet = LoadRegion2(borderNode);
		if (pRet)
			return pRet;

		QcList<QcRegion*> regionList;
		QcXmlNodeIterator regionNode = borderNode.FirstNode();
		while(regionNode)
		{
			QcRegion* pRegion = NULL;

			const char* value = regionNode.Attribute("value");
			switch (QcRegion::Type(regionNode.Attribute("type")))
			{
			case QcRegion::eCircle:
			case QcRegion::eMutilCircle:
				{
					vector<QcCircle> circleList;
					const char* p = strstr(value, "(");
					while(p)
					{
						f32 x = 0;
						f32 y = 0;
						f32 radiu = 0;
						QmAssert2(sscanf_s(p, "(%f,%f,%f)", &x, &y, &radiu) == 3);

						circleList.push_back(QcCircle(x,y,radiu));
						++p;
						p = strstr(p, "(");
					}
					if (circleList.size())
					{
						if (circleList.size() == 1)
							pRegion = new QcRegionCircle(circleList[0]);
						else
							pRegion = new QcRegionMultiCircle(circleList);
					}
					break;
				}
			case QcRegion::ePolygon:
				{
					QcPointList pointList;
					const char* p = strstr(value, "(");
					while(p)
					{
						f32 x = 0;
						f32 y = 0;
						QmAssert2(sscanf_s(p, "(%f,%f)", &x, &y) == 3);
						pointList.push_back(QcPoint(x, y));
						++p;
						p = strstr(p, "(");
					}
					pRegion = new QcRegionPolygon(pointList);
					break;
				}
			case QcRegion::eMulti:
				{
					assert(false);
					break;
				}
			}
			regionNode = regionNode.NextSibling();
			if (pRegion)
			{
				regionList.push_back(pRegion);
			}
		}
		if (regionList.size())
		{
			if (regionList.size() == 1)
			{
				pRet = regionList[0];
			}
			else
			{
				QcRegionMulti* pMutilRegion = new QcRegionMulti();
				QcList<QcRegion*>::iterator iter(regionList.begin());
				QcList<QcRegion*>::iterator endIter(regionList.end());
				for (; iter != endIter; ++iter)
				{
					pMutilRegion->AddRegion(*iter);
				}
				pRet = pMutilRegion;
			}
		}
	}
	return pRet;
}

QsFrameRegionList QcResImageDesc::LoadFrameRegionList(const QcXmlNodeIterator& node)
{
	QsFrameRegionList arRet;
	QcXmlNodeIterator borderNode = node.FirstNode("border");
	if (borderNode)
	{
		int iFrame = 0;
		QcXmlNodeIterator regionNode = borderNode.FirstNode();
		while(regionNode)
		{
			QcRegionAutoPtr pRegion = LoadRegion2(regionNode);
			if (pRegion)
			{
				QsFrameRegion frameRegion;
				frameRegion.m_iBeginFrame = regionNode.Attribute("EndFrame", iFrame);
				frameRegion.m_pRegion = pRegion;
				arRet.PushBack(frameRegion);
			}
			regionNode = regionNode.NextSibling();
		}
	}
	return arRet;
}

QcRegionAutoPtr QcResImageDesc::LoadRegion2(const QcXmlNodeIterator& node)
{
	QcRegionAutoPtr pRet;
	if (node)
	{
		QcRegionList regionList;

		//Circle
		const char* value = node.Attribute("Circle");
		if (value && strlen(value))
		{
			vector<QcCircle> circleList;
			const char* p = strstr(value, "(");
			while(p)
			{
				f32 x = 0;
				f32 y = 0;
				f32 radiu = 0;
				QmAssert2(sscanf_s(p, "(%f,%f,%f)", &x, &y, &radiu) == 3);

				circleList.push_back(QcCircle(x,y,radiu));
				++p;
				p = strstr(p, "(");
			}
			if (circleList.size())
			{
				if (circleList.size() == 1)
					regionList.PushBack(new QcRegionCircle(circleList[0]));
				else
					regionList.PushBack(new QcRegionMultiCircle(circleList));
			}
		}

		//polygon
		value = node.Attribute("Polygon");
		if (value && strlen(value))
		{
			QcStringList stringList;
			QfSplitString(value, stringList, ';');
			QcStringListIter iter(stringList.begin());
			for (; iter != stringList.end(); ++iter)
			{
				QcPointList pointList;
				const char* p = strstr(iter->c_str(), "(");
				while(p)
				{
					f32 x = 0;
					f32 y = 0;
					QmAssert2(sscanf_s(p, "(%f,%f)", &x, &y) == 3);
					pointList.push_back(QcPoint(x, y));
					++p;
					p = strstr(p, "(");
				}
				QmAssert(pointList.size());
				regionList.PushBack(new QcRegionPolygon(pointList));
			}
		}

		if (regionList.size())
		{
			if (regionList.size() == 1)
			{
				pRet = regionList[0];
			}
			else
			{
				QcRegionMulti* pMutilRegion = new QcRegionMulti();
				QcRegionList::iterator iter(regionList.begin());
				QcRegionList::iterator endIter(regionList.end());
				for (; iter != endIter; ++iter)
				{
					pMutilRegion->AddRegion(*iter);
				}
				pRet = pMutilRegion;
			}
		}
	}
	return pRet;
}

void QcResImageDesc::PostLoad(QcResManager* pResMgr)
{
	if (m_fWidth < ROUNDING_ERROR_f32 || m_fHeight < ROUNDING_ERROR_f32)
	{
		const QsStateParameter* pState = GetStateParameter(eLiving);	
		if (pState)
		{
			QcList<QsStateParameter>::iterator iter(m_arState.begin());
			QcList<QsStateParameter>::iterator endIter(m_arState.end());
			for (; iter != endIter; ++iter)
			{
				const QcTexturePtrList& textureList = iter->m_pTextureList;
				QcTexturePtrList::const_iterator texIter(textureList.begin());
				QcTexturePtrList::const_iterator texEndIter(textureList.end());
				for (; texIter != texEndIter; ++texIter)
				{
					iter->m_fWidth = QcMath::max_((*texIter)->GetWidth(), iter->m_fWidth);
					iter->m_fHeight = QcMath::max_((*texIter)->GetHeight(), iter->m_fHeight);
				}
				m_fWidth = QcMath::max_(m_fWidth, iter->m_fWidth);
				m_fHeight = QcMath::max_(m_fHeight, iter->m_fHeight);
			}
		}
		else
		{
			QcRect rect;
			QcList<QcResImageDescPtr>::iterator iter(m_arSubList.begin());
			for (; iter != m_arSubList.end(); ++iter)
			{
				QcResImageDesc* pImageDesc = iter->Get();
				QcRect subRect;
				subRect._TopRightPt.X = pImageDesc->GetWidth() * 0.5f;
				subRect._TopRightPt.Y = pImageDesc->GetHeight() * 0.5f;

				f32 fRotation = pImageDesc->GetRotation();
				if (!(QcMath::equals(fRotation, 0.f) || QcMath::equals(fRotation, PI) || QcMath::equals(fRotation, TWO_PI)))
				{
					if (QcMath::equals(fRotation, HALF_PI) || QcMath::equals(fRotation, HALF_PI + PI))
					{
						QcMath::swap(subRect._TopRightPt.X, subRect._TopRightPt.Y);
						subRect._BottomLeftPt = - subRect._TopRightPt;
					}
					else
					{
						const f64 cs = cos(fRotation);
						const f64 sn = sin(fRotation);

						QcPoint leftTop(-subRect._TopRightPt.X, subRect._TopRightPt.Y);
						leftTop.rotateBy(cs, sn);
						subRect._TopRightPt.rotateBy(cs, sn);

						subRect._BottomLeftPt.X = 0;
						subRect._BottomLeftPt.Y = 0;
						subRect.addInternalPoint(leftTop);
						subRect.addInternalPoint(-leftTop);
					}
				}
				else
				{
					subRect._BottomLeftPt = - subRect._TopRightPt;
				}
				subRect += pImageDesc->GetPos();
				rect.unionRect(subRect);
			}
			m_fWidth = rect.getWidth();
			m_fHeight = rect.getHeight();
		}
	}
}

bool QcResImageDesc::AddState(QeState eState, f32 fTime, bool bCycle, int iFPS)
{
	const char*  psState = StateToString(eState);

	QcList<QsStateParameter>::iterator iter(m_arState.begin());
	QcList<QsStateParameter>::iterator endIter(m_arState.end());
	for (; iter != endIter; ++iter)
	{
		if (iter->m_eState == eState)
		{
			iter->m_bCycle = bCycle;
			iter->m_fPlayTime = fTime;
			if (iFPS!=0)
			{
				iter->m_fPlayTime = iter->m_nQty /(f32)(iFPS);
			}
			break;
		}
	}
	if (iter == endIter)
	{
		char buf[MAX_PATH];
		int iCount = 0;
		QcResManager* pResMgr = QmResMgr;
		for (int i=0; i<1000; ++i)
		{
			sprintf_s(buf, MAX_PATH, "%s_%s_%03d.%s", m_sPrefix.c_str(), psState, i, m_sFileExt.c_str());

			if (pResMgr->FindImageFile(buf))
				++iCount;
			else
				break;
		}
		if (iCount)
		{
			QsStateParameter statePara;
			statePara.m_sName = psState;
			statePara.m_eState = eState;
			statePara.m_fPlayTime = fTime;
			statePara.m_bCycle = bCycle;
			statePara.m_nQty = iCount;
			if (iFPS)
			{
				statePara.m_fPlayTime = statePara.m_nQty /(f32)iFPS;
			}
			m_arState.push_back(statePara);
			return true;
		}
	}
	return false;
}

const QsStateParameter* QcResImageDesc::GetStateParameter(QeState eState) const
{
	if (m_arState.size())
	{
		QcList<QsStateParameter>::const_iterator iter(m_arState.begin());
		QcList<QsStateParameter>::const_iterator endIter(m_arState.end());
		for (; iter != endIter; ++iter)
			if (iter->m_eState == eState)
				return &(*iter);
	}
	return NULL;
}

int QcResImageDesc::GetStateImageList(QeState eState, QcStringList& imageList) const
{
	const QsStateParameter* pState = GetStateParameter(eState);
	if (pState)
		GetStateImageList(*pState, imageList);
	return imageList.size();
}

int QcResImageDesc::GetStateImageList(const QsStateParameter& state, QcStringList& imageList) const
{
	char buf[MAX_PATH] = {0};
	for (u32 i=0; i<state.m_nQty; ++i)
	{
		sprintf_s(buf, MAX_PATH, "%s_%s_%03d.%s", m_sPrefix.c_str(), state.m_sName.c_str(), i, m_sFileExt.c_str());
		imageList.push_back(QcString(buf));
	}
	return imageList.size();
}

QcSceneNode* QcResImageDesc::MakeInstance(bool bLoadSubNode) const
{
	QcSceneNode* pRet = m_pRuntimeClass->CreateObject()->As<QcSceneNode*>();
	pRet->SetState(eReady, false);
	pRet->Init(*this, bLoadSubNode);
	return pRet;
}