#ifndef ImageLoader_h__
#define ImageLoader_h__

#define  QmWinFileOn
#define QmAESEncrypt
#define QmMinEncryLen 16
#define PackName "Temp.sys"
#define PackDataExtension ".bat"

extern void WritePngImage(const char* data, unsigned long nSourceWidth, unsigned long xPos, unsigned long yPos, unsigned long xLen, unsigned long yLen, const char* psFile);
extern bool ReadImage(char *&bits_data, unsigned long &img_wid, unsigned long &img_hei, const char* psFile);
extern bool ReadImage(char *&bits_data, unsigned long &img_wid, unsigned long &img_hei, const char *source, unsigned long len);
extern void DestroyBitsData(char *&bits_data);

extern bool CompressData(char *pDest, unsigned long *pDestLen, const char *pSrc, unsigned long SrcLen);
extern bool UncompressData(char *pDest, unsigned long *pDestLen, const char *pSrc, unsigned long SrcLen);

struct QsARGB
{
	unsigned char b;
	unsigned char g;
	unsigned char r;
	unsigned char a;
};

typedef union
{
	QsARGB m_argb;
	unsigned __int32 m_value;
}QuARGB;

#endif // ImageLoader_h__