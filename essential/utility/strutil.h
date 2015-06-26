#pragma once
#include "../es_config.h"
#include <string>
#include <vector>

BEGIN_ES_NAMESPACE

	std::wstring& CutSpace(std::wstring& ret);
	std::string& CutSpace(std::string& ret);

	std::string CutAllSpace( const std::string& srcStr );
	// 获取文件的后缀名
	std::string GetFileSuffix( const std::string& fileName );

	// 获取文件无后缀名的部分 
	//    输入 fsdf.txt  输出 fsdf
	std::string GetFileWithoutSuffix(const std::string& fileName);

	// 获取文件的路径名.
	std::string GetFillPath_ByFullPathName(const std::string& fullPathName);

	// 获取 无路径信息的文件名
	std::string GetFileNameWithoutPath(const std::string& fullPathName);

	//函数说明
	//功能：字符串转为整数值[字符串可为2-16]任意进制数值字符串
	//返回：长整型数值
	//参数：strData 为将要转换的字符串
	//参数：jz 为字符串的进制
	long StrToData(const ::std::string& strData, int jz);


	// 功能： "任意字符串" 与 "c风格的只含有可读字符的字符串(CStyleStr)" 相互转换. 
	//
	// "任意字符串"	: 字符串中间可以有任意字符，甚至NULL.
	//
	// CStyleStr	: 字符串中间没有NULL,没有回车换行； 不可见字符进行了转义. 
	// ....... 比如 ：123\(00)\r\n6786\t\(0d)\(0a)
	// 
	// 需要转义的字符范围：0 <= c < 0x20 或 c == 0x7F . 
	// ....... 其中: 0x0d->\r, 0x0a->\n, , 0x09->\t; 其余以"\(十六进制数字)"的方式转义表示
	// ....... "\"符号作为转义指示符号, 其本身需要转义,以"\\"方式表示
	//
	::std::string ConvertToCStyleStr(const ::std::string& _Str); // 测试在BinaryStreamTester下 bufTester
	::std::string ConvertFromCStyleStr(const ::std::string& _CStyleStr);

    ::std::string ToHexDumpText(const std::string& _Str, size_t width);

	// 将字符 '0'-'9'  'a'-'f'  'A'-'F' 转为所对应的数字.  A->10  B->11 f->15
	int ToHexDigit( char c );

	int CompairNoCase(const ::std::string& lhs, const ::std::string& rhs);
	::std::string  ToLower(const ::std::string& str);

END_ES_NAMESPACE
