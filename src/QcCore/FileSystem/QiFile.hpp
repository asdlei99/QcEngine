#pragma once

#include "FileSystem\QcStdFile.hpp"
#include "FileSystem\QcWinFile.hpp"
#include "FileSystem\QcMemFile.hpp"

typedef QcWinFile QcFile;

class QcFileBuf : public QcArray<char, 1>
{
public:
	QcFileBuf(){}

	bool readFile(const char* psFile, bool appendZeroChar = false)
	{
		u32 iLen = 0;

		QcFile file;
		if (file.Open(psFile, std::ios::binary | std::ios::in))
		{
			iLen = file.GetSize();
			file.Seek(0, std::ios::beg);
			resize(appendZeroChar ? iLen + 1 : iLen);
			file.Read(pointer(), iLen);
			file.Close();
			if (appendZeroChar)
				pointer()[iLen] = 0;

			return true;
		}
		return false;
	}
};

