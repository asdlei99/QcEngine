#include "pch.hpp"
#include "QcPacketPerformer.hpp"

int main(int argc, char *argv[])
{

	std::locale::global(std::locale(""));

	const char* pPara = "";
	if (argc == 2)
	{
		pPara = argv[1];
	}

	char buf[MAX_PATH] = {0};
	GetModuleFileNameA(NULL, buf, MAX_PATH);
	QcString fileName;
	QcString path;
	QcFileSystem::SplitPath(QcString(buf), path, fileName);
	SetCurrentDirectoryA(path.c_str());

	QcPacketPerformer::GetInstance()->Excute(pPara);
    
	return 0;
}
