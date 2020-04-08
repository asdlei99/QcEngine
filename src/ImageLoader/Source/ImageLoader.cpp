#include "../libPNG/png.h"
#include "../libPNG/pngstruct.h"
#include "../zlib/zlib.h"
#include "../aes/aes.h"
#include "ImageLoader.h"
#include <iostream>
#include <fstream>
#include <windows.h>
#pragma warning(disable: 4309)

enum Endian {
	BigEndian,
	LittleEndian,
};
static const unsigned int qt_one = 1;
const int gsByteOrder = ((*((unsigned char *) &qt_one) == 0) ? BigEndian : LittleEndian);

class QcFileWriter
{
public:
	QcFileWriter(unsigned long initLen, unsigned long nGrowLen = 1024)
		: m_nLen(initLen)
		, m_nCurrent(0)
		, m_nGrowLen(nGrowLen)
	{
		m_pBuf = new char[initLen];
	}
	~QcFileWriter()
	{
		delete [] m_pBuf;
	}
	void write(const char* buf, unsigned long nLen)
	{
		if (m_nCurrent + nLen > m_nLen)
		{
			Expand(m_nGrowLen > nLen ? m_nGrowLen : nLen);
		}
		memcpy(m_pBuf + m_nCurrent, buf, nLen);
		m_nCurrent += nLen;
	}
	void save(const char* psFile)
	{
		DWORD dwOut = 0;
		HANDLE hFile = CreateFileA(psFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		WriteFile(hFile, m_pBuf, m_nCurrent, &dwOut, NULL);
		CloseHandle(hFile);
	}
	char* getBuf() const
	{
		return m_pBuf;
	}
	unsigned long getLen() const
	{
		return m_nCurrent;
	}
protected:
	void Expand(unsigned long newAddLen)
	{
		m_nLen += newAddLen;
		char* pNew = new char[m_nLen];
		memcpy(pNew, m_pBuf, m_nCurrent);
		delete [] m_pBuf;
		m_pBuf = pNew;
	}
protected:
	char* m_pBuf;
	unsigned long m_nLen;
	unsigned long m_nCurrent;
	unsigned long m_nGrowLen;
};

void PNGAPI user_write_data_fcn(png_structp png_ptr, png_bytep data, png_size_t length)
{
	QcFileWriter* file=(QcFileWriter*)png_ptr->io_ptr;
	file->write((const char*)data,(u32)length);
}

void WritePngImage(const char* data, unsigned long nSourceWidth, unsigned long xPos, unsigned long yPos, unsigned long xLen, unsigned long yLen, const char* psFile)
{
	QcFileWriter file(xLen * yLen * 4);

	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop info_ptr = png_create_info_struct(png_ptr);
	setjmp(png_jmpbuf(png_ptr));
	png_set_write_fn(png_ptr, &file, user_write_data_fcn, NULL);

	png_set_IHDR(png_ptr, info_ptr, xLen, yLen, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	nSourceWidth *= 4;
	data += nSourceWidth * yPos;
	unsigned long xOffset = xPos * 4;
	u8** RowPointers = new png_bytep[yLen];
	for (unsigned long i=0; i<yLen; ++i)
	{
		RowPointers[i]= (u8*)(data + xOffset);
		data += nSourceWidth;
	}

	png_set_rows(png_ptr, info_ptr, RowPointers);
	png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_BGR, NULL);

	delete [] RowPointers;
	file.save(psFile);
	png_destroy_write_struct(&png_ptr, &info_ptr);
}

struct stImageData
{
	const char *pData;
	unsigned int uOffset;
	unsigned int uDataLen;
};

void PngReadInMemory(png_structp png_ptr, png_bytep data, png_size_t length)
{
    stImageData *pImgData = (stImageData*)png_get_io_ptr(png_ptr);
    if (pImgData && pImgData->uOffset+length <= pImgData->uDataLen)
    {
        memcpy(data, pImgData->pData+pImgData->uOffset, length);
        pImgData->uOffset += length;
    }
    else
        png_error(png_ptr, "pngReadCallback failed");
}

bool ReadImage(char *&bits_data, unsigned long &img_wid, unsigned long &img_hei, const char* psFile)
{
	DWORD dwOut = 0;
	HANDLE hFile = CreateFileA(psFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD nLen = GetFileSize(hFile, NULL);
	char* pData = new char[nLen];
	ReadFile(hFile, pData, nLen, &dwOut, NULL);
	CloseHandle(hFile);

	bool bRet = ReadImage(bits_data, img_wid, img_hei, pData, nLen);

	delete [] pData;
	return bRet;
}

bool ReadImage(char *&bits_data, unsigned long &img_wid, unsigned long &img_hei, const char *source, unsigned long len)
{
    bool r = false;
    if (source)
    {
        png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, NULL, NULL);
        png_infop info_ptr = png_create_info_struct(png_ptr);
        setjmp(png_jmpbuf(png_ptr));

        stImageData data = {source, 0, len};

        png_set_read_fn(png_ptr, &data, PngReadInMemory);
		//png_set_sig_bytes(png_ptr, 8); // Tell png that we read the signature
		png_read_info(png_ptr, info_ptr); // Read the info section of the png file

		s32 BitDepth;
		s32 ColorType;
		{
			// Use temporary variables to avoid passing casted pointers
			png_uint_32 w,h;
			// Extract info
			png_get_IHDR(png_ptr, info_ptr,
				&w, &h,
				&BitDepth, &ColorType, NULL, NULL, NULL);
			img_wid=w;
			img_hei=h;
		}
		
		switch(ColorType)
		{
		case PNG_COLOR_TYPE_PALETTE:
			png_set_palette_to_rgb(png_ptr);
			break;
		case PNG_COLOR_TYPE_GRAY:
		case PNG_COLOR_TYPE_GRAY_ALPHA:
			png_set_gray_to_rgb(png_ptr);
			break;
		case PNG_COLOR_TYPE_RGB_ALPHA:
			if (gsByteOrder == BigEndian)
				png_set_swap_alpha(png_ptr);
			else
				png_set_bgr(png_ptr);
			break;
		case PNG_COLOR_TYPE_RGB:
			if (gsByteOrder == LittleEndian)
				png_set_bgr(png_ptr);
			break;
		}
		if (!(ColorType & PNG_COLOR_MASK_ALPHA) && !png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
			png_set_filler(png_ptr, 0xff, gsByteOrder == BigEndian ? PNG_FILLER_BEFORE : PNG_FILLER_AFTER);

		// Convert low bit colors to 8 bit colors
		if (BitDepth < 8)
		{
			png_set_packing(png_ptr);
		}
		else if (BitDepth == 16)
			png_set_strip_16(png_ptr);

		// Update the changes
		png_read_update_info(png_ptr, info_ptr);

		int iPitchWidth = img_wid*4;
		u8* pixeldata = new u8[img_hei*iPitchWidth + img_hei * sizeof(png_bytep)];
		u8** RowPointers = (u8**)(pixeldata + img_hei * iPitchWidth);
		bits_data = (char*)pixeldata;

		for (u32 i=0; i<img_hei; ++i)
		{
			RowPointers[i]=pixeldata;
			pixeldata += iPitchWidth;
		}
		png_read_image(png_ptr, RowPointers);

		png_destroy_read_struct(&png_ptr,&info_ptr, 0); // Clean up memory

		r = true;
    }
    return r;
}

void DestroyBitsData(char *&bits_data)
{
    if (bits_data)
    {
        delete[] bits_data;
        bits_data = NULL;
    }
}

bool CompressData(char *pDest, unsigned long *pDestLen, const char *pSrc, unsigned long SrcLen)
{
#if 0
	z_stream stream = {0};
	int err;

	stream.next_in = (Bytef*)pSrc;
	stream.avail_in = (uInt)SrcLen;

	stream.next_out = (Bytef*)pDest;
	stream.avail_out = (uInt)*pDestLen;

	err = deflateInit2(&stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, MAX_WBITS, 9, Z_DEFAULT_STRATEGY);
	stream.data_type = Z_BINARY;

	err = deflate(&stream, Z_FINISH);

	*pDestLen = stream.total_out;

	err = deflateEnd(&stream);

	return err == Z_OK;
#else
	return compress2((Bytef*)pDest, pDestLen, (Bytef*)pSrc, SrcLen, Z_DEFAULT_COMPRESSION) == Z_OK;
#endif
}
bool UncompressData(char *pDest, unsigned long *pDestLen, const char *pSrc, unsigned long SrcLen)
{
	 return uncompress((Bytef*)pDest, pDestLen, (Bytef*)pSrc, SrcLen) == Z_OK;
}
