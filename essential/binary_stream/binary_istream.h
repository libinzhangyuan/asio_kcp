#pragma once
#include "../es_config.h"

#include <assert.h>

//#include "boost/type_traits/is_fundamental.hpp"
//#include "boost/type_traits/is_pod.hpp"
//#include "boost/mpl/assert.hpp"

#include "binary_stream_def.h"
#include "check_function.h"

BEGIN_ES_NAMESPACE

	template <class buf_type, check_fuction istream_check = _assert_check>
	class _binary_istream
	{
	public:
		friend class istreamTester;
		friend class iostreamTester;
		friend class istreamTester_appointed_buf;

		_binary_istream(const buf_type &buf) : m_ReadBuf(buf), m_ReadPos(0) {}
		virtual ~_binary_istream(void) {}

	public:

    ::std::string Unpack(int sizeToUnpack);

		// 从流中读出数据, 读取位置由m_ReadPos指定
		// 任何情况下都会将pBuf清空(按照所给的bufSizeInBytes大小),再写数据. 
		// 当读取失败时(剩余数据不够所想要的大小, 或用户指定的buf比想读取的字节数小), throw异常 or assert
		//
		void Unpack(char* pBuf, int bufSizeInBytes, int sizeToCpy);

		// 这个接口只允许POD类型 (int, char, DWORD等, 以及只包含这些基本类型的结构体或类) 才能调用.
		// 对于非POD类型,请特例化之.
		template <class unpack_type>
			unpack_type Unpack(void);

		::std::string UnpackString(void) { // 最前面4个字节以int存储长度,后面紧跟内容.
	   		::std::string dst;
				UnpackString(dst);
				return dst;
		}

		::std::wstring UnpackWString(void)	{ // 最前面4个字节以int存储长度,后面紧跟内容.
				::std::string dst;
				UnpackString(dst);
				return ::std::wstring(reinterpret_cast<const wchar_t *>(dst.c_str()), dst.size() / sizeof(wchar_t));
		}

		//template <>
		//	buf_type Unpack(void)	{
		//		::std::string rstStr = Unpack<::std::string>(); // 是将buf当作::std::string进行存储的.
		//		return buf_type(rstStr.c_str(), rstStr.size());
		//	}

	public:

        size_t LeftBytes(void) const {return EndPos() - GetReadPos();}
		int GetReadPos(void) const;

		// 设置读取点. 必须是buf的有效内容范围内,否则抛异常.
		void SetReadPos(int readPos);

		// 从当前游标位置移动游标. offset < 0表示往左移动, >0表示往右移动.
		// 移动到非法位置时,会抛出异常
		void MoveReadPos(int offset);

		// 返回有效数据之后第一个位置 : 如 buf = "123" 则 EndPos() == 3.  
		//     当readPos == endPos时，表示读取到末尾了(不能再unpack了)。
		int EndPos(void) const;

	protected:
		_binary_istream(const _binary_istream& b) {}// 故意限制

	private:
		_binary_istream& operator=(const _binary_istream&); // 故意限制



		void UnpackString(::std::string& dstStr);

	private:
		// 内部接口: 从流中读出数据, 读取位置由用户指定(readPos), 或当前m_ReadPos位置. 
		// 不会改变流中的读取游标位置, 即不会影响调用Unpack的结果.
		// 任何情况下都会将pBuf清空(按照所给的bufSizeInBytes大小),再写数据. 
		// 当读取失败时(剩余数据不够所想要的大小, 或用户指定的buf比想读取的字节数小), throw异常 or assert
		//
		void Read(char* pBuf, int bufSizeInBytes, int readPos, int sizeToRead) const;

		template <class read_type>
			read_type Read(int readPos) const;

		void Read(char* pBuf, int bufSizeInBytes, int sizeToRead) const { // 从当前m_ReadPos读
			Read(pBuf, bufSizeInBytes, GetReadPos(), sizeToRead);
		}

		template <class read_type>
			read_type Read() const { // 从当前m_ReadPos读
				return Read<read_type>(m_ReadPos);
			}


	private:
		const buf_type& m_ReadBuf; // 知道关系.
		int m_ReadPos;
	};

	//class binary_buf;
	//typedef _binary_istream<binary_buf, throw> net_istream;



	// **********************************************************************************************
	// **********************************************************************************************
	// 下面是实现
	// **********************************************************************************************
	// **********************************************************************************************
	//
	template <class buf_type, check_fuction istream_check>
        ::std::string _binary_istream<buf_type, istream_check>::Unpack(int sizeToUnpack)
        {
            if (sizeToUnpack < 1024) 
            {
                char buf[1024] = "";
                Unpack(buf, sizeToUnpack, sizeToUnpack);
                return ::std::string(buf);
            }
            else
            {
                char* buf = new char[sizeToUnpack + 1];
                Unpack(buf, sizeToUnpack, sizeToUnpack);
                ::std::string ret(buf);
                delete [] buf;
                return ret;
            }
        }

	template <class buf_type, check_fuction istream_check>
		void _binary_istream<buf_type, istream_check>::Unpack(char* pBuf, int bufSizeInBytes, int sizeToCpy)
		{
			Read(pBuf, bufSizeInBytes, sizeToCpy);
			MoveReadPos(sizeToCpy);
		}

	template <class buf_type, check_fuction istream_check>
		template <class unpack_type>
			unpack_type _binary_istream<buf_type, istream_check>::Unpack(void)
			{
				// 下面这句限制只有POD类型, 但它没有限制到指针
				//BOOST_MPL_ASSERT_MSG(boost::is_pod<unpack_type>::value, obj_type_is_not_pod_type, (unpack_type));

				// 下面这句限制指针.
				//BOOST_MPL_ASSERT_MSG(!(boost::is_pointer<unpack_type>::value), obj_type_is_not_pod_type, (unpack_type));

				// to do : 对于结构体或类里面使用的指针,还没有限制到.
				// mpl_assert(pointer in struct or class);

				unpack_type buf;
				Unpack(reinterpret_cast<char*>(&buf), sizeof(unpack_type), sizeof(unpack_type));
				return buf;
			}


	template <class buf_type, check_fuction istream_check>
		int _binary_istream<buf_type, istream_check>::GetReadPos(void) const
		{
			return m_ReadPos;
		}


	template <class buf_type, check_fuction istream_check>	
		void _binary_istream<buf_type, istream_check>::SetReadPos(int readPos)
		{
			do_check(istream_check, readPos >= 0 && readPos <= EndPos(), "can't set readPos after context bound!");
			m_ReadPos = readPos;
		}

	template <class buf_type, check_fuction istream_check>	
		void _binary_istream<buf_type, istream_check>::MoveReadPos(int offset)
		{
			int newPos = GetReadPos() + offset; // 省略了加法溢出int范围的检查
			SetReadPos(newPos);
		}

	template <class buf_type, check_fuction istream_check>	
		int _binary_istream<buf_type, istream_check>::EndPos(void) const 
		{
			return static_cast<int>(m_ReadBuf.size());
		} 

	template <class buf_type, check_fuction istream_check>
		void _binary_istream<buf_type, istream_check>::UnpackString(::std::string& dstStr)
		{
			dstStr.clear();

			// 先做检查(避免移动游标后出错,需要还原游标的麻烦事情)
			//
			{
				int dstStrLen = Read<int>(); // 读出字符串长度
				int bufLeftLen = EndPos() - GetReadPos() - sizeof(int);
				do_check(istream_check, (dstStrLen >= 0) && (dstStrLen <= bufLeftLen), "distroyed string!");
			}

			// 开始取出数据,并填充string
			try	{
				int dstStrLen = Unpack<int>();
				dstStr.reserve(dstStrLen);
				for (int i = 0; i < dstStrLen; i++)
					dstStr.append(1, Unpack<char>()); 
			}
			catch (bin_stream_exception&) {
				assert(false); // 前面已经保证了长度了的,这儿不应该抛异常
			}
		}

	template <class buf_type, check_fuction istream_check>
		void _binary_istream<buf_type, istream_check>::Read(char* pBuf, int bufSizeInBytes, int readPos, int sizeToRead) const
		{
			assert(pBuf != NULL); // 故意保留原始的assert, 因为这种情况肯定是写错代码了.
			do_check(istream_check, bufSizeInBytes >= 0, "");

			memset(pBuf, 0, bufSizeInBytes);

			do_check(istream_check, (readPos >= 0) && (readPos <= EndPos()), "readPos exceed the bound of buf");
			do_check(istream_check, (sizeToRead >= 0) && ((readPos + sizeToRead) <= EndPos()), "read exceed the bound of buf!");
			do_check(istream_check, sizeToRead <= bufSizeInBytes, "dst buf is smaller than cpySize");

			const char* pReadBuf = m_ReadBuf.c_str();
			memcpy(pBuf, &pReadBuf[readPos], sizeToRead);
		}

	template <class buf_type, check_fuction istream_check>
		template <class read_type>
		read_type _binary_istream<buf_type, istream_check>::Read(int readPos) const
		{
			// 下面这句限制只有原生类型(int,char,DWORD等)才能调用Read接口.
			//BOOST_MPL_ASSERT_MSG(boost::is_fundamental<read_type>::value, obj_type_is_not_original_type, (read_type));

			read_type buf;
			Read(reinterpret_cast<char*>(&buf), sizeof(read_type), readPos, sizeof(read_type));
			return buf;
		}

END_ES_NAMESPACE







