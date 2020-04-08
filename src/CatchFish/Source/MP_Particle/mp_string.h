//-----------------------------------------------------------------------------
// File: mp_string.h
//
// Copyright (c) Astralax. All rights reserved.
// Author: Sedov Alexey
//-----------------------------------------------------------------------------

#ifndef MAGIC_STRING
#define MAGIC_STRING

#include <stdio.h>

#pragma warning ( disable : 4996)

class MP_String
{
protected:
	// eng: The pointer to the string
	char* theString;		
	
	// eng: String length
	int stringLen;
   
public:
	// eng: Constructors
	MP_String();
	MP_String(char s);
	MP_String (const char*);
	MP_String(const MP_String&);

	// eng: Destructor
	virtual ~MP_String(void);

	// eng: String assignment
	MP_String& operator=(const MP_String&);
	MP_String& operator=(char* s);

	// eng: Loads from the stream
	void LoadFromStream(FILE* stream);

	// eng: Writes the string in the stream
	void SaveToStream(FILE* stream);
	
	// eng: Writes the string in the stream as a text
	void SaveToStreamByText(FILE* stream);

	// eng: Compare strings
	bool operator==(const MP_String&);

	bool operator!=(const MP_String&);

	bool operator>(const MP_String&);

	bool operator<(const MP_String&);

	// eng: Index
	char& operator[](int n);

	// eng: Reversal
	MP_String& operator~(void);

	// eng: Append to string
	MP_String operator+(const MP_String& so);
	MP_String operator+(const char c);
	MP_String& operator+=(const MP_String& so);
	MP_String& operator+=(const char c);

	// eng: Return length of string
	int length(void);

	// eng: Converting to upper case
	MP_String& toUpper(void);

	// eng: Converting to lower case
	MP_String& toLower(void);

	// eng: Find character in string 
	int find(char c);

	// eng: Find substring in string
	int find(const char* subs);

	int find(const MP_String& so);

	// eng: Get C string equivalent
	const char* c_str();

	// eng: Return substring from specified position and with specified length
	MP_String substr(int pos,int plen);

	// eng: Return left substring 
	MP_String left(int len);

	// eng: Return right substring 
	MP_String right(int len);

	// eng: Check for empty string
	bool empty();
};

#endif
