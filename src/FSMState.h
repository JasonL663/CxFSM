#pragma once

#include <cstddef>
#include <type_traits>

namespace CxFSM::detail
{
	struct EmptyState { };
	template <class Driver, class State = typename Driver::State>
	auto FSMStateOf(int) { return static_cast<State*>(0); }
	template <class Driver>
	auto FSMStateOf(...) { return static_cast<EmptyState*>(0); }
}

namespace CxFSM
{
	template <class Driver>
	using FSMState = std::remove_pointer_t<decltype(detail::FSMStateOf<Driver>(0))>;

	template <class Builder>
	struct FSMStateAccess
	{
	public:
		constexpr FSMStateAccess(Builder* builder) : builder(builder) { }
		constexpr typename Builder::State& operator[](size_t index) { return builder->_state(index); }

	private:
		Builder* builder = nullptr;
	};
}