#pragma once

#include "../../src/general/stack_trace/FuncNameRegisterStub.h"
#include "../../src/general/stack_trace/StackTraceStub.h"
#include "../../src/general/stack_trace/FunctionParam.h"
#include "StackTraceIns.h"


#include "../config.h"
#include "../compile_config.h"


#ifdef ENABLE_STACK_TRACE
/**
* 必须在代码中使用了STACK_TRACE_BIND_INS，下面这个宏才能正常工作
**/
#define STACK_TRACE																	\
		static StackTrace::FuncNameRegisterStub registerStub(__FUNCTION__);				\
		StackTrace::StackTraceStub traceStub(registerStub.GetFuncIndex());	 		\
		traceStub.SetArgument

/**
* 绑定一个Stack对象到单件上以及绑定一个FuncNameMappingTable对象到单件上
**/
#define STACK_TRACE_BIND_INS( __StackTraceManagerInstance__  )			\
	StackTrace::StackTraceManagerIns::Set( __StackTraceManagerInstance__ );


#else // ENABLE_STACK_TRACE
	#define STACK_TRACE __noop
	#define STACK_TRACE_BIND_INS( __StackTraceManagerInstance__ )
#endif // ENABLE_STACK_TRACE





//1. 合并BIND_STACK_INSTANCE 和 BIND_FNMT_INSTANCE  ok
//2. 宏开关. 要检查release下，开启ENABLE_STACK_TRACE时运行正常.
//3. 移植到adapter(visitor, world, stub) func(ai)
//4. 全线狂加STACK_TRACE


//4.5 不开启ENABLE_STACK_TRACE时运行正常
//5. assert_check兼容未移植StackTrace系统的情况的代码需要重新审视.
//6. 给个移植说明.
//7. 远期目标： 考虑做STACK_TRACE2 和 STACK_TRACE_INIT2   and 3 and 4 ...
