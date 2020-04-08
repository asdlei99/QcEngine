#pragma once

typedef boost::function<void(const QcXmlNodeIterator& rootNode, void* pParam)> QcXmlNodeParseFn;
#define QmBindParseFn(fun, obj) boost::bind(fun, obj, _1, _2)
class QcEncryptXmlLoader : public QcObjectEx
{
public:
	virtual bool LoadFromFile(const char* psFile, void* pParam = 0, const QcXmlNodeParseFn& fn = QcXmlNodeParseFn());
	virtual void Parse(const QcXmlNodeIterator& pRootNode, void* pParam){}
};