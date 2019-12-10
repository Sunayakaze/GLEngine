
#pragma once

typedef unsigned int					uint;

class String
{
	char*								m_String;
	unsigned int						m_Size;

public:

	//- Constructors ----------------------------------------------------------

	String();

	~String();

	String(const String& rhs);

	String(const char* inString);

	//- Operations ------------------------------------------------------------

	void							Clear();

	const uint						Length(
		const char*						inString)				const;

	const bool						StartsWith(
		const char*						inString)				const;

	const bool						EndsWith(
		const char*						inString)				const;

	const bool						Equals(
		const char*						inString)				const;

	const bool						IsEmpty()					const;

	//- Member Access ---------------------------------------------------------

	const char*						Val()						const
	{
		return m_String;
	}

	const uint						Size()						const
	{
		return m_Size;
	}

	const char						GetChar(
		uint							Index)					const
	{
		if(Index < m_Size)
			return m_String[Index];
		else
			return 0;
	}

	//- Modify contents -------------------------------------------------------

	String&							Set(
		const char*						inString);

	String&							Append(
		const char*						inString);

	String&							operator=(
		String							rhs);

	String&							operator=(
		const char*						rhs);

	String friend					operator+(
		const String&,
		const String&);

	String friend					operator+(
		String&,
		const char*);

	String friend					operator+(
		const char*,
		String&);
};

void Strcpy(char*, const char*, const uint);
uint Strlen(const char*);
