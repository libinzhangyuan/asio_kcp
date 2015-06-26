#pragma once
#include "../es_config.h"
#include "binary_istream.h"
#include "binary_ostream.h"

BEGIN_ES_NAMESPACE

	template <class buf_type, check_fuction stream_check = _assert_check>
	class _binary_iostream :
		public _binary_istream<buf_type, stream_check>, public _binary_ostream<buf_type, stream_check>
	{
	public:
		friend class iostreamTester;

		_binary_iostream(buf_type& buf) 
			: _binary_istream<buf_type, stream_check>(buf), 
			  _binary_ostream<buf_type, stream_check>(buf)	{}

		~_binary_iostream(void) {}

	private:
		_binary_iostream(const _binary_iostream& b); // 故意限制
		_binary_iostream& operator=(const _binary_iostream& b); // 故意限制
	};

	//typedef _binary_iostream<binary_buf> binary_iostream;
END_ES_NAMESPACE
