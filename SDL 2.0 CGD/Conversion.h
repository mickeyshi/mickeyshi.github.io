#pragma once
#include <string>
#if _MSC_VER >= 1100
    #include <cstdlib>
    #define Absolute(val) std::abs(val)
#else
    #include <stdlib.h>
    #define Absolute(val) abs(val)
#endif

inline bool ToBool(const std::string &str);
inline bool ToBool(char c);
inline bool ToBool(int i);
inline int ToInt(bool b);
inline short ToShort(const std::string &str);
inline int ToInt(const std::string &str);
inline unsigned int ToUint(const std::string &str);
inline long ToLong(const std::string &str);
inline double ToDouble(const std::string &str);
inline float ToFloat(const std::string &str);
inline std::string ToString(bool b);
inline std::string ToString(char c);
inline std::string ToString(short s);
inline std::string ToString(unsigned short us);
inline std::string ToString(int i);
inline std::string ToString(unsigned u);
inline std::string ToString(float f);
inline std::string ToString(double d);
inline char ToChar(const std::string &str, unsigned index = 0);
inline char ToChar(bool b);
double ToRadians(double degrees);
float ToRadians(float degrees);
double ToDegrees(double radians);
float ToDegrees(float radians);

//"0" or "false" - false
//everything else true
bool ToBool(const std::string &str)
{
	return !(str == "0" || str == "false");
}
//'0' or Any nondigit character - false
//Any other integer - true
bool ToBool(char c)
{
	return c != '0' && isdigit(static_cast<unsigned char>(c));
}
//0 - false
//Any other integer - true
bool ToBool(int i)
{
	return i != 0;
}
//false - 0
//true - 1
int ToInt(bool b)
{
	if (b)
		return 1;
	return 0;
}
short ToShort(const std::string &str)
{
	return static_cast<short>(std::stoi(str));
}
int ToInt(const std::string &str)
{
	return std::stoi(str);
}
unsigned int ToUint(const std::string &str)
{
	return std::stoul(str);
}
long ToLong(const std::string &str)
{
	return std::stol(str);
}
double ToDouble(const std::string &str)
{
	return std::stod(str);
}
float ToFloat(const std::string &str)
{
	return std::stof(str);
}
//false - "false"
//true - "true"
std::string ToString(bool b)
{
	if (b)
		return "true";
	return "false";
}
//VS-2010 compatible
std::string ToString(char c)
{
	return std::string(1, c);
}
//VS-2010 compatible
std::string ToString(short s)
{
	return std::to_string(static_cast<long long>(s));
}
//VS-2010 compatible
std::string ToString(unsigned short us)
{
	return std::to_string(static_cast<unsigned long long>(us));
}
//VS-2010 compatible
std::string ToString(int i)
{
	return std::to_string(static_cast<long long>(i));
}
//VS-2010 compatible
std::string ToString(unsigned u)
{
	return std::to_string(static_cast<unsigned long long>(u));
}
//VS-2010 compatible
std::string ToString(float f)
{
	return std::to_string(static_cast<long double>(f));
}
//VS-2010 compatible
std::string ToString(double d)
{
	return std::to_string(static_cast<long double>(d));
}
//default index is 0: first character
char ToChar(const std::string &str, unsigned index)
{
	return str[index];
}
//false - '0'
//true - '1'
char ToChar(bool b)
{
	if (b)
		return '1';
	return '0';
}


