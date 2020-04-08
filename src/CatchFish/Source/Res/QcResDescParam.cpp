#include "pch.hpp"
#include "QcResDescParam.hpp"

ImplRuntimeClass2(QcResDescParam, 100);

void QcResDescParam::LoadParam(const QcXmlNodeIterator& node)
{
	QcXmlNodeIterator element = node.FirstNode();
	while (element)
	{
		if (QfStrNoCaseCmp(element.name(), "Point") == 0)
		{
			xml_attribute<char>* pAttribute = element.AttributeNode();
			while (pAttribute)
			{
				QcPoint pt = QcStrToValue(pAttribute->value());
				SetProperty(pAttribute->name(), pt);

				pAttribute = pAttribute->next_attribute();
			}
		}
		else if (QfStrNoCaseCmp(element.name(), "Float") == 0)
		{
			xml_attribute<char>* pAttribute = element.AttributeNode();
			while (pAttribute)
			{
				f32 fValue = QcStrToValue(pAttribute->value());
				SetProperty(pAttribute->name(), fValue);

				pAttribute = pAttribute->next_attribute();
			}
		}
		else if (QfStrNoCaseCmp(element.name(), "Int") == 0)
		{
			xml_attribute<char>* pAttribute = element.AttributeNode();
			while (pAttribute)
			{
				s32 iValue = QcStrToValue(pAttribute->value());
				SetProperty(pAttribute->name(), iValue);

				pAttribute = pAttribute->next_attribute();
			}
		}
		else if (QfStrNoCaseCmp(element.name(), "String") == 0)
		{
			xml_attribute<char>* pAttribute = element.AttributeNode();
			while (pAttribute)
			{
				SetProperty(pAttribute->name(), QcString(pAttribute->value()));
				pAttribute = pAttribute->next_attribute();
			}
		}
		else if (QfStrNoCaseCmp(element.name(), "StringList") == 0)
		{
			QcStringList stringList;
			xml_attribute<char>* pAttribute = element.AttributeNode();
			while (pAttribute)
			{
				stringList.clear();
				QfSplitString(pAttribute->value(), stringList, ';');
				SetProperty(pAttribute->name(), stringList);
				pAttribute = pAttribute->next_attribute();
			}
		}
		else if (QfStrNoCaseCmp(element.name(), "Image") == 0)
		{
			xml_attribute<char>* pAttribute = element.AttributeNode();
			while (pAttribute)
			{
				QcResImageDescPtr pImageDesc(new QcResImageDesc());
				pImageDesc->LoadDesc(pAttribute->value());
				pImageDesc = QmResMgr->LoadRes2(pImageDesc);
				SetProperty(pAttribute->name(), QcString(pAttribute->value()));
				pAttribute = pAttribute->next_attribute();
			}
		}
		element = element.NextSibling();
	}
}