#pragma once
#include "../es_config.h"
#include <assert.h>

#include "check_function.h"

BEGIN_ES_NAMESPACE

	class bin_buf_exception : public ::std::exception
	{
	public:
		bin_buf_exception(const char *const& _What) : ::std::exception(_What) {}
	};


	// 指定内存空间的buf. 其size是固定的. 只能写size范围内的数据,否则抛出异常. 
	//
	template <check_fuction buf_check = _assert_check>
	class _appointed_buf
	{
	public:
		friend class bufTester;
		friend class istreamTester_appointed_buf;

		_appointed_buf(char *_Ptr, size_t bufSize) 
			: m_Ptr(_Ptr), m_BufSize(bufSize)
		{
			assert(bufSize <= INT_MAX); // 这儿判断使用int作为bufSize转型传进来构造时的可能错误：int为负数，转为size_t后，会变成非常大的正数。
		}

		template <class left_buf_type> // 用来拷贝过来的buf，需要实现两个接口: c_str()  size().
		_appointed_buf& operator = (left_buf_type& b);

		size_t size(void) const {return m_BufSize;}

		void resize(size_t /*_Newsize*/){do_check(buf_check, false, "不能改变_appointed_buf的size!");} // 这个接口不能调用的.
		const char* c_str() const {return m_Ptr;}
		char& operator[](size_t _Off){
			do_check(buf_check, _Off < m_BufSize, "_appointed_buf::operator[] 所给的index大于bufsize!");
			return m_Ptr[_Off];
		}

		const char& operator[](size_t _Off) const{
			do_check(buf_check, _Off < m_BufSize, "_appointed_buf::operator[] 所给的index大于bufsize!");
			return m_Ptr[_Off];
		}


		// replace [_Off, _Off + _Count) with [_Ptr, _Ptr + _Count)
		_appointed_buf& replace(size_t _Off, const char *_Ptr, size_t _Count){
			assert(_Ptr != NULL); // 故意使用assert , 这种情况肯定是代码写错了.

			do_check(buf_check, _Off < m_BufSize, "_appointed_buf::replace 所给的覆盖位置在buf的有效范围之外!");
			do_check(buf_check, _Count >= 0, "_appointed_buf::replace 想覆盖的长度小于0!");
			do_check(buf_check, _Count <= m_BufSize, "_appointed_buf::replace 指定覆盖的长度太大!"); // 这儿能够帮忙判断 int类型负数，传进来后，变成非常大的正数的问题。
			do_check(buf_check, _Off + _Count <= m_BufSize, "_appointed_buf::replace 所想覆盖的范围超出了buf的有效范围!");

			const size_t leftSize = m_BufSize - _Off;
			memcpy_s(&m_Ptr[_Off], leftSize, _Ptr, _Count);
			return *this;
		}


		// 只清空内容，不改变大小的
		void clear(void){
			memset(m_Ptr, NULL, m_BufSize);
		}



	private:
		// 需要时，再实现这个接口
		_appointed_buf(_appointed_buf& b);

		// 下面的接口故意限制使用
		//
		_appointed_buf();
		_appointed_buf(char* p);



	private:
		char* const m_Ptr; // 知道关系. 由用户通过构造函数指定一个现有的内存空间.
		const size_t m_BufSize;
	};

	template <check_fuction buf_check>
		template <class left_buf_type>
		_appointed_buf<buf_check>& _appointed_buf<buf_check>::operator = (left_buf_type& b)
		{
			if (this == &b)
				return *this;

			do_check(buf_check, m_BufSize >= b.m_BufSize, "");
			assert(m_Ptr != NULL); // 发生这个肯定是代码写错了.
			assert(b.m_BufSize >= 0);

			memset(m_Ptr, NULL, m_BufSize);
			memcpy_s(m_Ptr, m_BufSize, b.m_Ptr, b.m_BufSize);
			return *this;
		}

END_ES_NAMESPACE
