#pragma once

#include "Common\QcStrToValue.hpp"

class QcXmlNodeIterator
{
public:
	QcXmlNodeIterator(xml_node<char>* pNode = NULL) : m_pNode(pNode){}

	operator bool() const
	{
		return m_pNode != NULL;
	}

//read Attribute	
	template<typename T>
	T Attribute(const char* psName, const T& defaultValue) const
	{
		xml_attribute<char>* pAttribute = AttributeNode(psName);
		if (pAttribute)
			return QcStrToValue(pAttribute->value());
		return defaultValue;
	}
	const char* Attribute(const char* psName) const
	{
		return Attribute(psName, (const char*)(""));
	}
//read value
	template<typename T>
	T Value(const T& defaultValue) const
	{
		QcStrToValue value(m_pNode->value());
		if (!value)
			return value;
		return defaultValue;
	}
	const char* Value() const
	{
		return m_pNode->value();
	}
//read SubValue
	template<typename T>
	T SubValue(const char* psName, const T& defaultValue) const
	{
		QcXmlNodeIterator node = FirstNode(psName);
		if (node)
			return node.Value(defaultValue);
		return defaultValue;
	}
	const char* SubValue(const char* psName) const
	{
		return SubValue(psName, (const char*)(""));
	}
	const char* name() const { return m_pNode->name(); }

//Iterator
	QcXmlNodeIterator FirstNode(const char* psName = NULL) const
	{
		return QcXmlNodeIterator(m_pNode->first_node(psName,0,false));
	}
	QcXmlNodeIterator NextSibling(const char* psName = NULL) const
	{
		return QcXmlNodeIterator(m_pNode->next_sibling(psName,0,false));
	}
	xml_attribute<char>* AttributeNode(const char* psName = NULL) const
	{
		if (m_pNode)
			return m_pNode->first_attribute(psName, 0, false);
		return NULL;
	}

	//Add Attribute
	template<typename T>
	void AddAttribute(const char* psName, const T& value)
	{
		char sBuf[QsValueSizeHelper<T>::Value];
		ValueToStr(value, sBuf);
		AddAttribute(psName, strlen(psName), sBuf, strlen(sBuf));
	}
	void AddAttribute(const char* psName, const QcString& value)
	{
		AddAttribute(psName, strlen(psName), value.c_str(), value.size());
	}
	void AddAttribute(const char* psName, const char* value)
	{
		AddAttribute(psName, strlen(psName), value, strlen(value));
	}

	//Add Node
	template<typename T>
	QcXmlNodeIterator AddNode(const char* psName, const T& value, rapidxml::node_type nodeType = rapidxml::node_element)
	{
		char sBuf[QsValueSizeHelper<T>::Value];
		ValueToStr(value, sBuf);
		return AddNode(psName, strlen(psName), sBuf, strlen(sBuf));
	}
	QcXmlNodeIterator AddNode(const char* psName, const QcString& value, rapidxml::node_type nodeType = rapidxml::node_element)
	{
		return AddNode(psName, strlen(psName), value.c_str(), value.size(), nodeType);
	}
	QcXmlNodeIterator AddNode(const char* psName, const char* value, rapidxml::node_type nodeType = rapidxml::node_element)
	{
		return AddNode(psName, strlen(psName), value, strlen(value), nodeType);
	}
protected:
	void AddAttribute(const char* psName, int nLen, const char* psValue, int nValueLen)
	{
		xml_document<char>* pDocument = document();
		char* pNewName = pDocument->allocate_string(psName, nLen);
		char* pNewValue = pDocument->allocate_string(psValue, nValueLen);
		xml_attribute<char>* pAttri = pDocument->allocate_attribute(pNewName, pNewValue, nLen, nValueLen);
		m_pNode->append_attribute(pAttri);
	}
	QcXmlNodeIterator AddNode(const char* psName, int nLen, const char* psValue, int nValueLen, rapidxml::node_type nodeType)
	{
		xml_document<char>* pDocument = document();
		char* pNewName = pDocument->allocate_string(psName, nLen);
		char* pNewValue = pDocument->allocate_string(psValue, nValueLen);
		xml_node<char>* pNode = pDocument->allocate_node(node_element, pNewName, pNewValue, nLen, nValueLen);
		m_pNode->append_node(pNode);
		return QcXmlNodeIterator(pNode);
	}
	xml_document<char>* document()
	{
		return m_pNode->document();
	}
private:
	xml_node<char>* m_pNode;
};