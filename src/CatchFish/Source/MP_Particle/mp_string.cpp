//-----------------------------------------------------------------------------
// File: mp_string.cpp
//
// Copyright (c) Astralax. All rights reserved.
// Author: Sedov Alexey
//-----------------------------------------------------------------------------

#include "pch.hpp"
#include "mp_string.h"
#include <String.h>
#include <stdlib.h>

// eng: Constructors
MP_String::MP_String()
{
	theString=new char[1];
	*theString=0;
	stringLen=0;
}

MP_String::MP_String(char s)
{
	theString=new char[2];
	theString[0]=s;
	theString[1]='\0';
	stringLen=1;
}

MP_String::MP_String(const char* s)
{
	int len=(int)strlen(s);
	theString=new char [len+1];
	strcpy(theString,s);
	stringLen=len;
	theString[len]=0;
}

MP_String::MP_String(const MP_String& so)
{
	theString=new char[so.stringLen+1];
	strcpy(theString,so.theString);
	theString[so.stringLen]=0;
	stringLen=so.stringLen;
}

// eng: Destructor
MP_String::~MP_String(void)
{
	if (theString)
		delete []theString;
}

// eng: Loads from the stream
void MP_String::LoadFromStream(FILE* stream)
{
	short dln;
    (void )fread((void*)&dln,sizeof(short),1,stream); // длин?имен?
	if (dln!=stringLen)
	{
		delete []theString;
		theString=new char[dln+1];
		stringLen=dln;
	}
	if (dln)
		(void)fread((void*)theString,sizeof(char),dln,stream);
	theString[dln]=0;
}

// eng: Writes the string to the stream
void MP_String::SaveToStream(FILE* stream)
{
	short dln=(short)stringLen;
	// eng: Name's length
	(void )fwrite((void*)&dln,sizeof(short),1,stream); 
    if (dln)
	{
		(void)fwrite((void*)theString,sizeof(char),dln,stream);
	}
}

// eng: Writes the string to the stream as a text
void MP_String::SaveToStreamByText(FILE* stream)
{
	(void)fwrite((void*)theString,sizeof(char),stringLen,stream);
}

// eng: String assignment
MP_String& MP_String::operator=(const MP_String& so)
{
	if (this!=&so)
   	{
		if (theString)
			delete []theString;
		theString=(char*) new char[so.stringLen+1];
		strcpy(theString,so.theString);
		stringLen=so.stringLen;
	}
	return *this;
}

// eng: String assignment
MP_String& MP_String::operator=(char* s)
{
	int len=(int)strlen(s);
	if (theString)
		delete []theString;
	theString=(char*) new char[len+1];
	strcpy(theString,s);
	stringLen=len;
	return *this;
}

// eng: Compare strings
bool MP_String::operator==(const MP_String& so)
{
	int dl1=this->stringLen;
	int dl2=so.stringLen;
	if (dl1!=dl2)
   		return false;
	char* s1=this->theString;
	char* s2=so.theString;
	for (int i=0;i<dl1;i++)
   		if (s1[i]!=s2[i])
      		return false;
	return true;
}

bool MP_String::operator!=(const MP_String& so)
{
	if (strcmp(this->theString,so.theString)==0)
   		return false;
	return true;
}

bool MP_String::operator>(const MP_String& so)
{
	if (strcmp(this->theString,so.theString)>0)
   		return true;
	return false;
}

bool MP_String::operator<(const MP_String& so)
{
	if (strcmp(this->theString,so.theString)<0)
   		return true;
	return false;
}

// eng: Index
char& MP_String::operator[](int n)
{
	if (n<0)
   		return theString[0];
   	else
		if (n>stringLen)
         	return theString[stringLen-1];
        else
           	return theString[n];
}

// eng: Reversal
MP_String& MP_String::operator~(void)
{
	strrev(theString);
	return *this;
}

// eng: Append to string
MP_String MP_String::operator+(const MP_String& so)
{
	int len=stringLen+so.stringLen;
	MP_String temp;
	delete []temp.theString;
	temp.theString=new char[len+1];
	strcpy(temp.theString,this->theString);
	strcat(temp.theString,so.theString);
	temp.stringLen=len;
	return temp;
}

MP_String MP_String::operator+(const char c)
{
	int len=stringLen+1;
	MP_String temp;
	delete []temp.theString;
	temp.theString=new char[len+1];
	strcpy(temp.theString,this->theString);
	*(temp.theString+len-1)=c;
	*(temp.theString+len)='\0';
	temp.stringLen=len;
	return temp;
}

MP_String& MP_String::operator+=(const MP_String& so)
{
	int len=stringLen+so.stringLen;
	char* temp=new char[len+1];
	strcpy(temp,this->theString);
	strcat(temp,so.theString);
	*this=temp;
	delete []temp;
	return *this;
}

MP_String& MP_String::operator+=(const char c)
{
	int len=stringLen+1;
	char* temp=new char[len+1];
	strcpy(temp,this->theString);
	temp[len-1]=c;
	temp[len]='\0';
	*this=temp;
	delete []temp;
	return *this;
}

// eng: Return length of string
int MP_String::length(void)
{
	return stringLen;
}

// eng: Converting to upper case
MP_String& MP_String::toUpper(void)
{
	strupr(theString);
	return *this;
}

// eng: Converting to lower case
MP_String& MP_String::toLower(void)
{
	strlwr(theString);
	return *this;
}

// eng: Find character in string 
int MP_String::find(char c)
{
	char* s=strchr(theString,c);
	if (s==NULL)
   		return -1;
    else
      	return (int)(s-theString);
}

// eng: Find substring in string
int MP_String::find(const char* subs)
{
	char* s=strstr(theString,subs);
	if (s==NULL)
   		return -1;
    else
      	return (int)(s-theString);
}

int MP_String::find(const MP_String& so)
{
	char* s=strstr(theString,so.theString);
	if (s==NULL)
   		return -1;
    else
      	return (int)(s-theString);
}

// eng: Get C string equivalent
const char* MP_String::c_str()
{
	return (const char*)theString;
}

// eng: Return substring from specified position and with specified length
MP_String MP_String::substr(int pos,int plen)
{
	MP_String temp;
	delete []temp.theString;
	temp.theString=new char[plen+1];
	temp.theString[0]=0;
	temp.stringLen=plen;
	for (int i=0;i<plen;i++)
   		temp.theString[i]=theString[pos+i];
	*(temp.theString+plen)='\0';
	return temp;
}

// eng: Return left substring 
MP_String MP_String::left(int len)
{
	return substr(0,len+1);
}

// eng: Return right substring 
MP_String MP_String::right(int len)
{
	return substr(length()-len,len);
}

// eng: Check for empty string
bool MP_String::empty()
{
	return (length()==0);
}
