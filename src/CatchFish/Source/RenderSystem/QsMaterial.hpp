#pragma once

#include "RenderSystem\QcColor.hpp"

struct QsMaterial
{
	QsMaterial()
	{
		Init();
	}
	QsMaterial(const QsMaterial& sMaterial)
	{
		*this = sMaterial;
	}
	void Init()
	{
		m_ambientColor.set(0xffffffff);
		m_diffuseColor.set(0xffffffff);
		m_emissiveColor.set(0xffffffff);
		m_specularColor.set(0xffffffff);
		m_fShininess = 5.f;
	}
	QsMaterial& operator=(const QsMaterial& sMaterial)
	{
		if (this == &sMaterial)
			return *this;

		m_ambientColor = sMaterial.m_ambientColor;
		m_diffuseColor = sMaterial.m_diffuseColor;
		m_emissiveColor = sMaterial.m_emissiveColor;
		m_specularColor = sMaterial.m_specularColor;
		m_fShininess = sMaterial.m_fShininess;

		return *this;
	}
	bool operator==(const QsMaterial& sMaterial) const
	{
		return m_ambientColor == sMaterial.m_ambientColor && 
		m_diffuseColor == sMaterial.m_diffuseColor && 
		m_emissiveColor == sMaterial.m_emissiveColor &&
		m_specularColor == sMaterial.m_specularColor &&
		QcMath::equals(m_fShininess, sMaterial.m_fShininess);
	}
	bool operator!=(const QsMaterial& sMaterial) const
	{
		return !(*this == sMaterial);
	}

	D3DMATERIAL9& ToD3DMaterial(D3DMATERIAL9& material) const
	{
		m_ambientColor.ToD3DColorValue(material.Ambient);
		m_diffuseColor.ToD3DColorValue(material.Diffuse);
		m_emissiveColor.ToD3DColorValue(material.Emissive);
		m_specularColor.ToD3DColorValue(material.Specular);
		material.Power = m_fShininess;
		return material;
	}

	QcColor m_ambientColor;
	QcColor m_diffuseColor;
	QcColor m_emissiveColor;
	QcColor m_specularColor;
	f32 m_fShininess;
};