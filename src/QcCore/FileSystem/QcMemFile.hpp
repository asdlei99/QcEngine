#pragma once



class QcMemFile
{
	enum
	{
		eNormal,
		eFreeMem,
		eCopyMem,
	};
public:
	inline QcMemFile(u32 nBufExpandLen = 256) 
	: Buffer(NULL)
	, Len(0)
	, MaxPos(0)
	, Pos(0)
	, NBufExpandLen(nBufExpandLen)
	, MemFlag(eNormal){}

	inline ~QcMemFile()
	{
		Close();
	}

	inline void OpenFromMem(void* memory, long len, int memFlag = eNormal)
	{
		Close();

		if (eCopyMem == memFlag)
		{
			Buffer = new char[len];
			memcpy(Buffer, memory, len);
			MemFlag = eFreeMem;
		}
		else
		{
			Buffer = memory;
			MemFlag = memFlag;
		}
		Pos = 0;
		MaxPos = 0;
		Len = len;
	}
	inline char* GetBuffer() {return (char*)Buffer;}
	inline long GetMaxSize() const {return MaxPos; }
	inline long GetPos() const { return Pos;}
	inline long GetSize() const { return Len;}
	inline void Seek(long lDistanceToMove, std::ios_base::seekdir _Way)
	{
		switch(_Way)
		{
		case std::ios::beg:
			break;
		case std::ios::cur:
			lDistanceToMove += Pos;
			break;
		case std::ios::end:
			lDistanceToMove = Len + lDistanceToMove;
			break;
		}
		if (lDistanceToMove >=0 && lDistanceToMove < Len)
			Pos = lDistanceToMove;
	}
	inline s32 Read(void* buffer, u32 sizeToRead)
	{
		s32 amount = static_cast<s32>(sizeToRead);
		if (Pos + amount > Len)
			amount -= Pos + amount - Len;

		if (amount <= 0)
			return 0;

		char* p = (char*)Buffer;
		memcpy(buffer, p + Pos, amount);

		Pos += amount;
		MaxPos = max(MaxPos, Pos);
		return amount;
	}
	inline s32 Write(const void* buffer, u32 sizeToWrite)
	{
		s32 amount = static_cast<s32>(sizeToWrite);
		if (Pos + amount > Len)
		{
			if (NBufExpandLen > 0)
			{
				Expend(max(amount, NBufExpandLen));
			}
			else
				amount -= Pos + amount - Len;
		}

		if (amount <= 0)
			return 0;

		char* p = (char*)Buffer;
		memcpy(p + Pos, buffer, amount);

		Pos += amount;
		MaxPos = max(MaxPos, Pos);
		return amount;
	}
	inline void Close()
	{
		FreeMem();
		Buffer = NULL;
		Len = 0;
		Pos = 0;
		MemFlag = eNormal;
	}


	template<class T>
	void Write(const T& value)
	{
		//QmStatic_Assert(std::is_pod<T>::value);
		Write((char*)&value, sizeof(T));
	}
	template<>
	void Write(const QcString& str)
	{
		Write(str.c_str(), str.size() + 1);
	}
	template<class T>
	s32 Read(T& value)
	{
		//QmStatic_Assert(std::is_pod<T>::value);
		return Read((char*)&value, sizeof(T));
	}
	template<>
	s32 Read(QcString& str)
	{
		char* pStr = (char*)Buffer + Pos;
		str = pStr;
		Pos += str.size() + 1;
		return str.size() + 1;
	}
protected:
	void FreeMem()
	{
		if (MemFlag & eFreeMem)
			delete [] (char*)Buffer;
	}
	void Expend(int nExpendLen)
	{
		void* pNewBuf = new char[Len + nExpendLen];
		memcpy(pNewBuf, Buffer, Len);
		FreeMem();
		Len += nExpendLen;
		MemFlag = eFreeMem;
		Buffer = pNewBuf;
	}
protected:
	void *Buffer;
	long Len;
	long Pos;
	long NBufExpandLen;
	long MaxPos;
	int MemFlag;
};