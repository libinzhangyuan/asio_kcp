#pragma once
#include "../es_config.h"
#include <assert.h>

//#include "boost/type_traits/is_fundamental.hpp"
//#include "boost/type_traits/is_pod.hpp"
//#include "boost/mpl/assert.hpp"

#include "binary_stream_def.h"
#include "../check_function.h"


BEGIN_ES_NAMESPACE

	template <class buf_type, check_fuction ostream_check = _assert_check>
	class _binary_ostream
	{
	public:
		friend class ostreamTester;
		friend class iostreamTester;
		friend class ostreamTester_appointed_buf;

		_binary_ostream(buf_type &buf) : m_WriteBuf(buf), m_WritePos((int)buf.size()) {}
		virtual ~_binary_ostream(void) {}

	public:

		// 将数据压入流中, 位置由writePos指定.
		// 如果未设置WritePos, 默认状态WritePos就是串尾, 即从buf中已有内容的后面开始写.
		// 返回当前的writePos.
		//
		int Pack(const char* pBuf, int size);

		// 这个接口只允许POD类型 (int, char, DWORD等, 以及只包含这些基本类型的结构体或类) 才能调用.
		// 对于非POD类型,请特例化之.
		template <class pack_type>
			int Pack(const pack_type& buf);

		int PackString(const ::std::string& str) {
			return _PackString(str);
		}

		int PackWString(const ::std::wstring& wstr) {
			::std::string str(reinterpret_cast<const char *>(wstr.c_str()), wstr.size() * sizeof(wchar_t));
			return _PackString(str);
		}

	public:
		int GetWritePos(void);

		// 设置写入点. 必须是buf的有效内容范围内,否则抛异常.
		void SetWritePos(int writePos);

		// 从当前游标位置移动游标. offset < 0表示往左移动, >0表示往右移动.
		// 移动到非法位置时,会抛出异常
		void MoveWritePos(int offset);

		// 返回有效数据之后第一个位置 : 如 buf = "123" 则 EndPos() == 3
		int EndPos(void) const {return BufLen();}

	protected:
		_binary_ostream(const _binary_ostream&); // 故意限制

	private:
		_binary_ostream& operator=(const _binary_ostream&); // 故意限制


		int BufLen(void) const {return static_cast<int>(m_WriteBuf.size());}

		void ResizeBuf(int newSize);

		int _PackString(const ::std::string& buf);

	private:
		buf_type& m_WriteBuf; // 知道关系。
		int m_WritePos;
	};

	//class binary_buf;
	//typedef _binary_ostream<binary_buf> binary_ostream;



	// **********************************************************************************************
	// **********************************************************************************************
	// 下面是实现
	// **********************************************************************************************
	// **********************************************************************************************
	//
	template<class buf_type, check_fuction ostream_check>
		template <class pack_type>
			int _binary_ostream<buf_type, ostream_check>::Pack(const pack_type& buf)
			{
				// 下面这句限制只有POD类型, 但它没有限制到指针
				//BOOST_MPL_ASSERT_MSG(boost::is_pod<pack_type>::value, obj_type_is_not_pod_type, (pack_type));

				// 下面这句限制指针.
				//BOOST_MPL_ASSERT_MSG(!(boost::is_pointer<pack_type>::value), obj_type_is_not_pod_type, (pack_type));

				// to do : 对于结构体或类里面使用的指针,还没有限制到.
				// mpl_assert(pointer in struct or class);

				return Pack(reinterpret_cast<const char*>(&buf), sizeof(pack_type));
			}


	template<class buf_type, check_fuction ostream_check>
		int _binary_ostream<buf_type, ostream_check>::Pack( const char* pBuf, int size )
		{
			assert(pBuf != NULL);
			do_check(ostream_check, size >= 0, "call pack, but size < 0");

			int BufNeedLen =  GetWritePos() + size;
			if (BufNeedLen > BufLen())
				ResizeBuf(BufNeedLen);

			m_WriteBuf.replace(GetWritePos(), pBuf, size);
			SetWritePos(GetWritePos() + size);

			return GetWritePos();
		}

	template<class buf_type, check_fuction ostream_check>	
		int _binary_ostream<buf_type, ostream_check>::_PackString( const ::std::string& str )
		{
			Pack<int>((int)str.size());
			return Pack(str.c_str(), (int)str.size());
		}

	template<class buf_type, check_fuction ostream_check>
		void _binary_ostream<buf_type, ostream_check>::ResizeBuf( int newSize )
		{
			m_WriteBuf.resize(newSize);
		}


	template<class buf_type, check_fuction ostream_check>
		int _binary_ostream<buf_type, ostream_check>::GetWritePos( void )
		{
			return m_WritePos;
		}


	template<class buf_type, check_fuction ostream_check>
		void _binary_ostream<buf_type, ostream_check>::SetWritePos( int writePos )
		{
			do_check(ostream_check, writePos >= 0 && writePos <= EndPos(), "can't set writePos after context bound!");
			m_WritePos = writePos;
		}

	template<class buf_type, check_fuction ostream_check>
		void _binary_ostream<buf_type, ostream_check>::MoveWritePos( int offset )
		{
			int newPos = GetWritePos() + offset; // 省略了溢出检查
			SetWritePos(newPos);
		}

END_ES_NAMESPACE
