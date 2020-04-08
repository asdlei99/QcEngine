#pragma once

#define QmMaxbuf 4096

class QcFileWriter
{
public:
	QcFileWriter(BYTE* buf = NULL, u32 nLen = 0, u32 nExpandBufOnce = 256, bool bFreeBufOnExpand = false) : 
	m_pBuf(buf), m_nLen(nLen), m_nExpandBufOnce(nExpandBufOnce), m_nCurrent(0), m_bFreeBufOnExpand(bFreeBufOnExpand){}

	void Write(const BYTE* buf, u32 nLen)
	{
		if (m_pBuf)
		{
			if (m_nCurrent + nLen > m_nLen)
			{
				if (m_nExpandBufOnce == 0)
				{
					QmLogError("There are no enough memory.");
				}
				else
				{
					Expand(nLen);
				}
			}
			memcpy(m_pBuf + m_nCurrent, buf, nLen);
		}
		m_nCurrent += nLen;
	}

	template<class T>
	void Write(const T& value)
	{
		Write((BYTE*)&value, sizeof(T));
	}

	BYTE* GetBuf() const
	{
		return m_pBuf;
	}
	u64 GetLen() const
	{
		return m_nCurrent;
	}
protected:
	void Expand(u32 newAddLen)
	{
		newAddLen = QcMath::max_(m_nExpandBufOnce, newAddLen);
		BYTE* newBuf = (BYTE*)malloc(m_nLen + newAddLen);
		if (newBuf == NULL)
			QmLogError("There are no enough memory.");

		memcpy(newBuf, m_pBuf, m_nCurrent);
		m_nLen += newAddLen;
		if (m_bFreeBufOnExpand)
			free(m_pBuf);
		m_pBuf = newBuf;
	}
protected:
	BYTE* m_pBuf;
	bool m_bFreeBufOnExpand;
	u32 m_nLen;
	u32 m_nExpandBufOnce;
	u32 m_nCurrent;
};

class QcFileReader
{
public:
	QcFileReader(BYTE* buf, u32 nLen)
		: m_buf(buf), m_nLen(nLen), m_nCurrent(0){}

	void Read(BYTE* buf, u32 nLen)
	{
		memcpy(buf, m_buf, nLen);
		m_buf += nLen;
		m_nCurrent += nLen;
		QmAssert(m_nCurrent <= m_nLen);
	}
	template<class T>
	void Read(T& value)
	{
		Read((BYTE*)&value, sizeof(T));
	}
	u32 GetPos() const {return m_nCurrent;}
	u32 GetLen() const {return m_nLen;}
protected:
	BYTE* m_buf;
	u32 m_nLen;
	u32 m_nCurrent;
};



class QcFileStorageBase : public QcObject
{
public:
	QcFileStorageBase() : m_bDirty(false), m_nLen(0){}
	virtual void Write(QcFileWriter& writer) = 0;
	virtual void Read(QcFileReader& reader) = 0;
	int GetLen() 
	{
		if (m_nLen == 0)
		{
			QcFileWriter writer; 
			Write(writer); 
			m_nLen = (int)writer.GetLen();
		}
		return m_nLen;
	}
	QcCriticalSection& GetCS() {return m_cs;}
	bool IsDirty() const { return m_bDirty;}
	void SetDirty(bool bDirty) { m_bDirty = bDirty;}
protected:
	bool m_bDirty;
	int m_nLen;
	QcCriticalSection m_cs;
};