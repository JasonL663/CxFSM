#pragma once

#include <cstddef>
#include <type_traits>

namespace CxFSM::detail
{
	struct EmptyTransition { };
	template <class Driver, class Transition = typename Driver::Transition>
	auto FSMTransitionOf(int) { return static_cast<Transition*>(0); }
	template <class Driver>
	auto FSMTransitionOf(...) { return static_cast<EmptyTransition*>(0); }
}

namespace CxFSM
{
	template <class Driver>
	using FSMTransition = std::remove_pointer_t<decltype(detail::FSMTransitionOf<Driver>(0))>;

	template <class Builder>
	struct FSMTransitionAccess
	{
	public:
		constexpr FSMTransitionAccess(Builder* builder) : builder(builder) { }
		constexpr typename Builder::Transition& operator[](size_t index) { return builder->_transition(index); }

	private:
		Builder* builder = nullptr;
	};
}