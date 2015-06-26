#pragma once
#include <string>
#include "../../../Essential/src/general/stack_trace/StackTraceDef.h"
namespace StackTrace
{
	namespace StackReporter
	{		
		/**
		* 以文本方式展示StackRecords
		**/
		void ShowStackRecordsInTxt( const StackFrameInfo_t& stackInfo );
	}
}