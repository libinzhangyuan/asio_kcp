#pragma once
#include "../es_config.h"

#include <string>
#include <ostream>
#include <istream>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../utility/strutil.h"
class archive_normal;

BEGIN_ES_NAMESPACE

	class _binary_buf
	{
	public:
		friend class iostreamTester;
		friend class istreamTester;
		friend class ostreamTester;
		friend class binary_buf_Tester;
		friend class bufTester;

		typedef archive_normal artype; // 对NASA系统，设置为一般序列化模式

		friend ::std::ostream& operator << (::std::ostream& os, const _binary_buf& buf); // 序列化
		friend ::std::istream& operator >> (::std::istream& is, _binary_buf& buf);		 // 反序列化

		_binary_buf(void) {;}
		_binary_buf(const char *_Ptr, size_t _Count) : m_Str(_Ptr, _Count) {;}
		_binary_buf(const _binary_buf& b) : m_Str(b.m_Str) {;}

		inline _binary_buf& operator = (const _binary_buf& b);
		inline _binary_buf& operator+=(const _binary_buf& _Right); // append _Right
		inline _binary_buf& operator+=(char _Ch); // append 1 * _Ch

		inline bool operator == (_binary_buf& b) const;
		inline bool operator != (_binary_buf& b) const;
		inline bool operator > (_binary_buf& b) const;
		inline bool operator < (_binary_buf& b) const;
		inline bool operator <= (_binary_buf& b) const;
		inline bool operator >= (_binary_buf& b) const;

		inline bool LoadFromFile( const ::std::string& fullPathName );

		// 从一段内存中读入数据.
		// 头四个字节(size_t)是定义大小的
		// 返回pStr中被读取了多少位置(offset)
		inline size_t LoadFromString_FrontIsSize(const char* pStr);

        inline std::string to_std_string(void) const;
        inline std::string to_cstyle_str(void) const;
        inline std::string to_hex_dump_str(void) const;

		inline size_t size(void) const;
		inline void resize(size_t _Newsize);

		inline size_t capacity() const;
		inline void reserve(size_t _Newcap = 0);

		inline const char* c_str() const;
		inline char& operator[](size_t _Off);
		inline const char& operator[](size_t _Off) const;


		inline _binary_buf& set(const char* _Ptr, size_t _Count);
		inline _binary_buf& set(const ::std::string& _Str);




		// append _Right
		inline _binary_buf& append(const _binary_buf& _Right); 

		// append [_Ptr, _Ptr + _Count)
		inline _binary_buf& append(const char *_Ptr, size_t _Count);

		// append _Right [_Roff, _Roff + _Count)
		inline _binary_buf& append(const _binary_buf& _Right, size_t _Roff, size_t _Count);

		// append _Count * _Ch
		inline _binary_buf& append(size_t _Count, char _Ch);


		// replace [_Off, _Off + _N0) with [_Ptr, _Ptr + _Count)
		inline _binary_buf& replace(size_t _Off, size_t _N0, const char *_Ptr, size_t _Count);

		// replace [_Off, _Off + _Count) with [_Ptr, _Ptr + _Count)
		inline _binary_buf& replace(size_t _Off, const char *_Ptr, size_t _Count);
	 

		// insert _Right at _Off
		inline _binary_buf& insert(size_t _Off, const _binary_buf& _Right);

		// insert _Right [_Roff, _Roff + _Count) at _Off
		inline _binary_buf& insert(size_t _Off,
			const _binary_buf& _Right, size_t _Roff, size_t _Count);

		// insert [_Ptr, _Ptr + _Count) at _Off
		inline _binary_buf& insert(size_t _Off, const char *_Ptr, size_t _Count);

		// insert _Count * _Ch at _Off
		inline _binary_buf& insert(size_t _Off, size_t _Count, char _Ch);

		// erase elements [_Off, _Off + _Count)
		inline _binary_buf& erase(size_t _Off = 0, size_t _Count = ::std::string::npos);


		inline void clear(void);

	private:

		// 下面的接口故意限制使用,只能够内部使用!
		//
		_binary_buf(char* p) : m_Str(p) {};
		_binary_buf(const ::std::string& str) : m_Str(str) {};
		inline _binary_buf& append(const char *_Ptr);
		inline _binary_buf& operator+=(const char *_Ptr);
		inline _binary_buf& insert(size_t _Off, const char *_Ptr);

	private:
		::std::string m_Str;
	};

	// 序列化
	inline ::std::ostream& operator << (::std::ostream& os, const _binary_buf& buf)
	{
		::std::string cStyleStr = ConvertToCStyleStr(buf.m_Str);
		os << cStyleStr;		
		return os;
	}

	inline ::std::istream& operator >> (::std::istream& is, _binary_buf& buf)
	{
		buf.clear();

		::std::string cStyleStr;
		::std::getline(is, cStyleStr);
		buf.m_Str = ConvertFromCStyleStr(cStyleStr);
		return is;
	}

    inline ::std::string _binary_buf::to_std_string(void) const
    {
        return m_Str;
    }

    inline ::std::string _binary_buf::to_cstyle_str(void) const
    {
        return ConvertToCStyleStr(m_Str);
    }

    inline ::std::string _binary_buf::to_hex_dump_str(void) const
    {
        return ToHexDumpText(m_Str, 32);
    }

	inline _binary_buf& _binary_buf::operator = (const _binary_buf& b) 
	{
		// m_Str进行了自拷贝检查了的
		m_Str = b.m_Str; 
		return *this;
	} 


	inline _binary_buf& _binary_buf::operator+=(const _binary_buf& _Right) // append _Right
	{	
		m_Str.append(_Right.m_Str);
		return *this;
	}

	inline _binary_buf& _binary_buf::operator+=(char _Ch) // append 1 * _Ch
	{	
		m_Str.append((size_t)1, _Ch);
		return *this;
	}

	inline _binary_buf& _binary_buf::operator+=(const char *_Ptr)
	{
		m_Str.append(_Ptr);
		return *this;
	}

	inline bool _binary_buf::operator == (_binary_buf& b) const
	{
		return m_Str == b.m_Str;
	}

	inline bool _binary_buf::operator != (_binary_buf& b) const
	{
		return m_Str != b.m_Str;
	}

	inline bool _binary_buf::operator > (_binary_buf& b) const
	{
		return m_Str > b.m_Str;
	}

	inline bool _binary_buf::operator < (_binary_buf& b) const
	{
		return m_Str < b.m_Str;
	}

	inline bool _binary_buf::operator <= (_binary_buf& b) const
	{
		return m_Str <= b.m_Str;
	}

	inline bool _binary_buf::operator >= (_binary_buf& b) const
	{
		return m_Str >= b.m_Str;
	}

	inline size_t _binary_buf::size(void) const 
	{
		return m_Str.size();
	}

	inline void _binary_buf::resize(size_t _Newsize) 
	{
		return m_Str.resize(_Newsize);
	}

	inline size_t _binary_buf::capacity() const
	{
		return m_Str.capacity();
	}

	inline void _binary_buf::reserve(size_t _Newcap /* = 0 */) 
	{
		return m_Str.reserve(_Newcap);
	}

	inline const char* _binary_buf::c_str() const 
	{
		return m_Str.c_str();
	}

	inline char& _binary_buf::operator[](size_t _Off) 
	{
		return m_Str[_Off];
	}

	inline const char& _binary_buf::operator[](size_t _Off) const 
	{
		return m_Str[_Off];
	}

	inline _binary_buf& _binary_buf::set(const char* _Ptr, size_t _Count)
	{
		m_Str = ::std::string(_Ptr, _Count);
		return *this;
	}

	inline _binary_buf& _binary_buf::set(const ::std::string& _Str)
	{
		m_Str = _Str;
		return *this;
	}


	inline _binary_buf& _binary_buf::append(const _binary_buf& _Right)
	{
		m_Str.append(_Right.m_Str);
		return *this;
	}

	inline _binary_buf& _binary_buf::append(const _binary_buf& _Right,	size_t _Roff, size_t _Count) // append _Right [_Roff, _Roff + _Count)
	{
		m_Str.append(_Right.m_Str, _Roff, _Count);
        return *this;
	}

	inline _binary_buf& _binary_buf::append(const char *_Ptr, size_t _Count) 
	{
		m_Str.append(_Ptr, _Count);
		return *this;
	}

	inline _binary_buf& _binary_buf::append(size_t _Count, char _Ch) 
	{
		m_Str.append(_Count, _Ch);
		return *this;
	}

	inline _binary_buf& _binary_buf::append(const char *_Ptr) 
	{
		m_Str.append(_Ptr);
		return *this;
	}


	inline _binary_buf& _binary_buf::replace(size_t _Off,
			size_t _N0, const char *_Ptr, size_t _Count) 
	{
		m_Str.replace(_Off, _N0, _Ptr, _Count);
		return *this;
	}

	inline _binary_buf& _binary_buf::replace(size_t _Off, const char *_Ptr, size_t _Count)
	{
		m_Str.replace(_Off, _Count, _Ptr, _Count);
		return *this;
	}


	inline _binary_buf& _binary_buf::insert(size_t _Off, const _binary_buf& _Right)
	{	// insert _Right at _Off
		m_Str.insert(_Off, _Right.m_Str);
		return *this;
	}

	inline _binary_buf& _binary_buf::insert(size_t _Off,
			const _binary_buf& _Right, size_t _Roff, size_t _Count)
	{
		m_Str.insert(_Off, _Right.m_Str, _Roff, _Count);
		return *this;
	}

	inline _binary_buf& _binary_buf::insert(size_t _Off,
		  const char *_Ptr, size_t _Count)
	{
		m_Str.insert(_Off, _Ptr, _Count);
		return *this;
	}

	inline _binary_buf& _binary_buf::insert(size_t _Off, const char *_Ptr)
	{
		m_Str.insert(_Off, _Ptr);
		return *this;
	}

	inline _binary_buf& _binary_buf::insert(size_t _Off, size_t _Count, char _Ch)
	{
		m_Str.insert(_Off, _Count, _Ch);
		return *this;
	}

	inline _binary_buf& _binary_buf::erase(size_t _Off /* = 0 */, size_t _Count /* = size_t(0) */)
	{
		m_Str.erase(_Off, _Count);
        return *this;
	}

	inline void _binary_buf::clear(void) 
	{
		m_Str.clear();
	}

	inline bool _binary_buf::LoadFromFile( const ::std::string& fullPathName ) 
	{
		clear();

		FILE* f = fopen(fullPathName.c_str(), "rb");
		if (f == NULL)
			return false;

		for (;;)
		{
			char readBuf[1024 * 10] = "";
			int readCount = (int)fread(readBuf, 1, sizeof(readBuf), f);
			if (readCount == 0)
				break;

			append(readBuf, readCount);
		}

		fclose(f);
		return true;
	}

	inline size_t _binary_buf::LoadFromString_FrontIsSize(const char* pStr)
	{
        assert(pStr != NULL);

		size_t nSize = 0;
		memcpy(&nSize, pStr, sizeof(size_t));

		m_Str.assign(pStr + sizeof(size_t), nSize);
		return nSize + sizeof(size_t);
	}


END_ES_NAMESPACE
