#pragma once

class QcEncryptFileBuf : public QcArray<char, 1>
{
public:
	QcEncryptFileBuf(){}
	int Load(const char* psFile, bool appendZeroChar = false);
};