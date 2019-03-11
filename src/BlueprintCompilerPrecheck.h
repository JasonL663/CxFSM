#pragma once

namespace CxFSM
{
	template <class FSM, class Builder = BuilderOf<FSM>>
	struct BlueprintCompilerPrecheck
	{
		static constexpr bool DoPrecheck()
		{
			bool result = false;

			Builder builder{};
			FSM::Blueprint(builder);

			result = true;
			return result;
		}
		static constexpr bool value = DoPrecheck();
	};

	template <class FSM>
	struct BlueprintCompilerPrecheck<FSM, void>
	{
		static constexpr bool value = false;
	};
}