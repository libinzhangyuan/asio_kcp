#pragma once
#include <vector>
#include <string>
#include "../../src\general\stack_trace\StackTraceDef.h"

namespace StackTrace
{
	class StackFrame;
	class FunctionParam;
	class FuncNameMappingTable;
	class Stack;
	
	/**
	* 现在不支持多线程使用，但是能兼容使用在多线程中，处理方式是，只关注主线程中使用，
	* 其他线程中使用不处理
	**/
	class StackTraceManager
	{
	public:
		friend class InterfaceToolsTester;


		StackTraceManager();
		virtual ~StackTraceManager();
		/**
		* 设置关注的线程，注意现在只支持单线程
		**/
		void SetThreadId( unsigned long threadId );
		const unsigned long& GetThreadId() const;

		/**
		* StackTrace是否生效
		**/
		static bool IsStackTraceCanUse();
		
		/**
		* 获取stackFrame info
		**/
		StackFrameInfo_t GetStackRecords( void );

	public:
		size_t FrameSize(void) const;
		const StackFrame& operator[](const size_t& i) const;

		/**
		* 压入的frame没有参数
		**/
		void PushFrame(const FunctionIndex_t& index);

		/**
		* 往当前栈帧位置压入参数
		**/
		void PushParamToCurFrame(const FunctionParam& param);


		/**
		* 压入的frame有参数
		**/
		void PushFrame(const FunctionIndex_t& index, const FunctionParam& param);

		void PopFrame(const FunctionIndex_t& index);

	public:
		// 注册 " 函数名字" 和 "FunctionIndex_t"
		void Register(const FunctionIndex_t& index, const FunctionName_t& name);

		// 根据index查询function名字
		//   找不到时返回空字符串
		FunctionName_t GetFunctionName(const FunctionIndex_t& index) const;

	private:
		FuncNameMappingTable* m_pFuncNameMappingTable;
		Stack* m_pStack;

		unsigned long m_threadId;
	};
};
