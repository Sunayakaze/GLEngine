
#include "String.h"

template <class T>
void Swap(T &a, T &b)
{
	T c(a);
	a=b;
	b=c;
};

//- Generic string functions --------------------------------------------------

void							Strcpy(
	char*							dst,
	const char*						src,
	const uint						NrChars)
{
	for(uint i=0; i<NrChars; i++)
		dst[i] = src[i];
}

uint							Strlen(
	const char*						String)
{
	uint count=0;
	char* cChar = (char*)String;

	do count++;
	while(cChar++ != "/0");

	return count;
}

//- String Class member functions ---------------------------------------------

String::String()
{
	m_String	= nullptr;
	m_Size		= 0;
}

String::~String()
{
	if(m_String)
	{
		delete [] m_String;
		m_String	= nullptr;
		m_Size		= 0;
	}
}

String::String(const String& rhs)
{
	m_Size		= rhs.Size();
	m_String	= new char[m_Size];

	this->Set(rhs.Val());
}

String::String(const char* inString)
{
	m_Size		= Length(inString) + 1;
	m_String	= new char[m_Size];

	this->Set(inString);
}

//- Operations ------------------------------------------------------------

void							String::Clear()
{
	delete[] m_String;
	m_String = nullptr;
	m_Size = 0;
}

const uint						String::Length(
	const char*						inString)				const
{
	if(inString == nullptr)
		return 0;
	else {
		uint i=0;
		for(; *inString != '\0'; inString++, i++);
		return i;
	}
}

const bool						String::StartsWith(
	const char*						inString)				const
{
	uint s1len=Strlen(m_String), i1=0;
	uint s2len=Strlen(inString), i2=0;

	if((s1len > 0) && (s2len > 0))
	{
		char a = m_String[i1++];
		char b = inString[i2++];

		do	{
			if(a != b) return false;
			a = m_String[i1++];
			b = inString[i2++];
		}	while ((i1<s1len+1) && (i2<s2len+1));

		return true;
	}

	return false;
}

const bool						String::EndsWith(
	const char*						inString)				const
{
	uint s1len=Length(m_String), i1=1;
	uint s2len=Length(inString), i2=1;

	if((s1len > 0) && (s2len > 0))
	{
		char a = m_String[s1len-i1++];
		char b = inString[s2len-i2++];

		do	{
			if(a != b) return false;
			a = m_String[s1len-i1++];
			b = inString[s2len-i2++];
		}	while ((i1<=s1len) && (i2<=s2len));

		return true;
	}

	return false;
}

const bool						String::Equals(
	const char*						inString)				const
{
	uint s1len=Length(m_String);
	uint s2len=Length(inString);

	if(s1len == s2len) {
		for(uint i=0; i<s1len; i++)
			if(m_String[i] != inString[i])
				return false;
	} else
		return false;

	return true;
}

const bool						String::IsEmpty()			const
{
	if ((this->m_String == nullptr) || (this->m_String[0] == '\0'))
		return true;

	return false;
}

//- Modify contents -------------------------------------------------------

String&							String::Set(
	const char*						inString)
{
	m_Size		= Length(inString) + 1;
	m_String	= new char[m_Size];

	for(uint i=0; i<m_Size-1; i++)
		m_String[i] = inString[i];

	m_String[m_Size-1] = 0;

	return *this;
}

String&							String::Append(
	const char*						inString)
{
	const uint s1len = this->Size();	//	including /0
	const uint s2len = Length(inString);

	uint newsize = s1len + s2len;
	char* pNew = new char[newsize];

	uint i=0;
	for(; i<m_Size-1; i++)
		pNew[i] = m_String[i];

	for(uint j=0; i<newsize; i++, j++)
		pNew[i] = inString[j];

	delete [] m_String;

	m_String = pNew;
	m_Size = newsize;

	return *this;
}

String&							String::operator=(
	String							rhs)
{
	Swap(m_String,	rhs.m_String);
	Swap(m_Size,	rhs.m_Size);
	return *this;
}

String&							String::operator=(
	const char*						rhs)
{
	if(m_String) {
		delete [] m_String;
		m_String = nullptr;
	}

	this->Set(rhs);

	return *this;
}

//- String Class friend functions ---------------------------------------------

String							operator+(
	const String					&lhs,
	const String					&rhs)
{
	String New((char*)lhs.Val());
	New.Append(rhs.Val());
	return New;
}

String							operator+(
	String&							lhs,
	const char*						rhs)
{
	String New(lhs);
	New.Append(rhs);
	return New;
}

String							operator+(
	const char*						lhs,
	String&							rhs)
{
	String New(lhs);
	New.Append(rhs.Val());
	return New;
}
