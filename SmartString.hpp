#pragma once

#include <string>
#include <memory>
#include <cstdarg>
#include <time.h>

#pragma warning(disable: 6385 6273 4267 4018 4477 4172)

#ifdef _WIN32

#define SMART_STRING_AUTHOR  "Daniel Berg, 14 Year old Prodigy."
#define SMART_STRING_VERSION 1.0.0
#define SMART_STRING_MAX_BUF 4096
#define SMART_STRING_CHARS   "ABCDEFGHIJKNLMOPQRSTUVWXYZabcdefghijknlmopqrstuvwxyz"
#define SMART_STRING_CHARSW  L"ABCDEFGHIJKNLMOPQRSTUVWXYZabcdefghijknlmopqrstuvwxyz"

class SmartString
{
private:

	std::string m_iString;
	int         m_iSize;
	char        m_iXorKey[3] = { 'A','B','C' };

public:

	SmartString(const char* Format, ...)
	{
		char buf[SMART_STRING_MAX_BUF];

		va_list args;
		va_start(args, Format);
		vsnprintf_s(buf, SMART_STRING_MAX_BUF, Format, args);

		m_iString = buf;
		m_iSize = m_iString.length();
	}

	inline SmartString* CurrentInstance()
	{
		return this;
	}

	inline std::string Get()
	{
		return m_iString;
	}

	inline std::string Append(const char* Format, ...)
	{
		char buf[SMART_STRING_MAX_BUF];

		va_list args;
		va_start(args, Format);
		vsnprintf_s(buf, SMART_STRING_MAX_BUF, Format, args);

		m_iString.append(buf);
	}

	inline void Set(char Character, int Index)
	{
		m_iString.at(Index) = Character;
	}

	inline std::wstring ToWString()
	{
		return std::wstring(m_iString.begin(), m_iString.end());
	}

	inline const wchar_t* ToWChar()
	{
		std::wstring wstr = ToWString();
		const wchar_t* ptr = wstr.c_str();
		return ptr;
	}

	inline int Length() const
	{
		return m_iSize;
	}

	inline int Size() const
	{
		return m_iSize;
	}

	inline size_t SizeOf() const
	{
		return sizeof(m_iString);
	}

	inline char GetChar(int index) const
	{
		return m_iString.at(index);
	}

	inline bool IsNull() const
	{
		return (&m_iString != nullptr && m_iSize > 0);
	}

	std::string XOR(std::string input)
	{
		std::string result = input;
		for (unsigned int i = 0; i < input.size(); i++)
			result[i] = input[i] ^ m_iXorKey[i % (sizeof(m_iXorKey) / sizeof(char))];

		return result;
	}

	std::string Random(int length)
	{
		std::string result;
		static const char bet[] = { SMART_STRING_CHARS };
		srand((unsigned)time(0));

		for (unsigned int i = 0; i < length; i++)
			result += bet[rand() % (sizeof(bet) - 1)];

		return result;
	}

	std::wstring RandomW(int length)
	{
		std::wstring result;
		static const wchar_t bet[] = { SMART_STRING_CHARSW };
		srand((unsigned)time(0));

		for (unsigned int i = 0; i < length; i++)
			result += bet[rand() % (sizeof(bet) - 1)];

		return result;
	}

	char* Addr()
	{
		char buf[sizeof(int) * 4];
		sprintf_s(buf, "0x%llx", &m_iString);
		return buf;
	}

	void* Ptr()
	{
		return (void*)&m_iString;
	}

	std::string operator+(const char* Format)
	{
		m_iString.append(Format);
	}

	std::string operator()(const char* Format, ...)
	{
		char buf[SMART_STRING_MAX_BUF];

		va_list args;
		va_start(args, Format);
		vsnprintf_s(buf, SMART_STRING_MAX_BUF, Format, args);

		m_iString.append(buf);
	}

	/* We can't forget our nullterminator on the end of our string :), unless i learned something wrong haha */

	bool operator < (const char* Format)
	{
		return m_iString.length() < strlen(Format) + '\0';
	}

	bool operator > (const char* Format)
	{
		return m_iString.length() > strlen(Format) + '\0';
	}

	~SmartString()
	{
		m_iString.clear();
		memset(&m_iSize, 0, sizeof(m_iSize));
	}
};

#define SMART_STRING_INSTANCE(Name, String, ...) SmartString Instance(String, __VA_ARGS__)

#else
#error "Your OS is not supported."
#endif