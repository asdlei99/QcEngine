#include "pch.hpp"
#include "QcResBulletDesc.hpp"
#include "QcTexture.hpp"
#include "Scene\QcBullet.hpp"
#include "Collision\QcRegionMulti.hpp"
#include "Collision\QcRegionCircle.hpp"
#include "Collision\QcRegionPolygon.hpp"


ImplRuntimeClass2(QcResBulletDesc, 100);

QcResBulletDesc::QcResBulletDesc()
{
	m_pRuntimeClass = QmRuntimeClass(QcBullet);
}

QcResBulletDesc::~QcResBulletDesc()
{

}

bool QcResBulletDesc::LoadDesc(const QcXmlNodeIterator& node)
{
	if (!QcResImageDesc::LoadDesc(node))
		return false;

	QcXmlNodeIterator netNode = node.FirstNode("Net");
	if (netNode)
	{
		m_netObj = netNode.Value();
		m_netNameA = netNode.Attribute("NetNameA");
		m_netEffA = netNode.Attribute("NetEffA");

		m_netNameB = netNode.Attribute("NetNameB");
		m_netEffB = netNode.Attribute("NetEffB");

		m_bIsShock = netNode.Attribute("Shock", 0) != 0;

		QcUtil::StringToPointList(netNode.Attribute("SubFirePosList"), m_SubFirePosList, "(", ",");
	}
	
	return true;
}

void QcResBulletDesc::PostLoad(QcResManager* pResMgr)
{
	QcResImageDesc::PostLoad(pResMgr);
	if (!m_pSourceRegion)
	{
		const QsStateParameter* pState = GetStateParameter(eLiving);
		if (pState)
		{
			f32 fHalfWidth = pState->m_pTextureList[0]->GetWidth() * 0.5f;
			f32 fHalffHeight = pState->m_pTextureList[0]->GetHeight() * 0.5f;
#if 0
			if (fHalfWidth/fHalffHeight > 1.5f || fHalffHeight/fHalfWidth > 1.5f)
			{	
				QcPoint pointList[] = 
				{
					QcPoint(-fHalfWidth, -fHalffHeight), 
					QcPoint(fHalfWidth, -fHalffHeight),
					QcPoint(fHalfWidth, fHalffHeight),
					QcPoint(-fHalfWidth, fHalffHeight),
					QcPoint(-fHalfWidth, -fHalffHeight), 
				};
				m_pSourceRegion.Set(new QcRegionPolygon(pointList, sizeof(pointList)/sizeof(QcPoint)));
			}
			else
#endif
			{
				m_pSourceRegion = new QcRegionCircle(QcPoint(0.f, 0.f), (fHalfWidth + fHalffHeight) * 0.5f);
			}
		}
	}
}