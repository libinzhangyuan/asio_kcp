#pragma once
#include "StackTraceManager.h"
#include "../../include/utility/fakeSingleton.h"

namespace StackTrace
{
	typedef  Essential::fakeSingleton<StackTraceManager>  StackTraceManagerIns;
};

