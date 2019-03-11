#pragma once

namespace CxFSM
{
	struct NullDriver
	{
		struct State { };
		struct Transition { };
		template <const auto&>
		struct Impl;
	};

	struct EmptyData { };
}