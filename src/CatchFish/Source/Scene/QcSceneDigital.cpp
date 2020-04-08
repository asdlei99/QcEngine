#include "pch.hpp"
#include "QcSceneDigital.hpp"
#include "Res\QcResDescParam.hpp"
#include "Res\QcResImageDesc.hpp"

ImplRuntimeClass2(QcSceneDigital, 100);

QcSceneDigital::QcSceneDigital()
{

}

void QcSceneDigital::Init(const QcResImageDesc& desc, bool bLoadSubNode)
{
	QcSceneNode::Init(desc, bLoadSubNode);

	QcResDescParam* pDescParam = desc.GetParam();
	if (pDescParam)
	{
		QcString sValue;
		sValue = pDescParam->GetProperty("DigitalRes", sValue);
		if (!sValue.empty())
			m_oDigitalNode.SetDigitalType(QcObjType(sValue.c_str()));

		f32 fValue = 0.f;
		m_oDigitalNode.SetRotation(QcMath::degToRad(pDescParam->GetProperty("DigitalRotation", fValue)));

		sValue = "Center";
		sValue = pDescParam->GetProperty("DigitalAlign", sValue);
		QeAlign eAlign = QfStringToAlign(sValue.c_str());
		m_oDigitalNode.SetAlign(eAlign);
		QcPoint tempPt;
		m_oDigitalNode.SetPosition(ReverseAlginPos(eAlign, QcPoint(0.f, 0.f), GetWidth(), GetHeight(), tempPt) );
	}
	else
	{
		m_oDigitalNode.SetVisible(false);
	}
}

void QcSceneDigital::RenderSub(QcDeviceRender& render, const QcMatrix* pWorldMatrix)
{
	QcSceneNode::RenderSub(render, pWorldMatrix);

	m_oDigitalNode.Render(render, pWorldMatrix);
}

void QcSceneDigital::Animate( double dCurrentTime,f32 fElapsedTime )
{
    QcSceneNode::Animate(dCurrentTime, fElapsedTime);
    m_oDigitalNode.Animate(dCurrentTime, fElapsedTime);
}
