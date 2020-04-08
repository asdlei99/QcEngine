#include "Pch.hpp"
#include "QcEncryptXmlLoader.hpp"
#include "QcEncryptFileBuf.hpp"

bool QcEncryptXmlLoader::LoadFromFile(const char* psFile, void* pParam, const QcXmlNodeParseFn& fn)
{
	bool bRet = false;
	QcEncryptFileBuf byteArray;
	if (byteArray.Load(psFile, true))
	{
		QcXmlLoader loader;
		QcXmlNodeIterator rootNode = loader.LoadFromBuf(byteArray.pointer());
		if (rootNode)
		{
			if (fn)
				fn(rootNode, pParam);
			else
				Parse(rootNode, pParam);
		}
	}
	return bRet;
}