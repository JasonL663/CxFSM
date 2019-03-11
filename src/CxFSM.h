#pragma once

#include "Builder.h"
#include "Blueprint.h"
#include "BlueprintCompiler.h"
#include "MessageDriver.h"
#include "TimedDriver.h"

namespace CxFSM
{
	template <class FSM>
	static constexpr auto BlueprintOf = BlueprintCompiler<FSM>::Compile();
	template <class FSM>
	using CompiledFSM = typename BlueprintCompiler<FSM>::Driver::template Impl<BlueprintOf<FSM>>;
}