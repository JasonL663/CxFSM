#pragma once

#include "BuilderPass0.h"
#include "BuilderPass1.h"
#include "BuilderPass2.h"
#include "FSMState.h"
#include "FSMTransition.h"
#include <cstddef>
#include <stdexcept>

namespace CxFSM
{
	template <class Driver_, size_t N>
	struct Builder
	{
	public:
		using Driver = Driver_;
		using State = FSMState<Driver>;
		using Transition = FSMTransition<Driver>;

	public:
		constexpr Builder() { }
		constexpr Builder(Builder&) = delete;

		FSMStateAccess<Builder> state{ this };
		FSMTransitionAccess<Builder> transition{ this };

	private:
		constexpr State& _state(size_t index);
		constexpr Transition& _transition(size_t index);

	private:
		size_t cur_state = 0;
		BuilderPass0<Driver>* pass0 = nullptr;
		BuilderPass1<Driver>* pass1 = nullptr;
		BuilderPass2<Driver>* pass2 = nullptr;

		State dummy_state{};
		Transition dummy_transition{};

		static constexpr size_t NUM_STATES = N;
	private:
		template <class FSM, bool>
		friend struct BlueprintCompiler;
		friend struct FSMStateAccess<Builder>;
		friend struct FSMTransitionAccess<Builder>;
	};

	template <class Driver, size_t N>
	constexpr FSMState<Driver>& Builder<Driver, N>::_state(size_t index)
	{
		if (index >= N)
			throw std::out_of_range("Index out of range");

		cur_state = index;
		if (pass0 != nullptr)
			return pass0->_state(index);
		if (pass1 != nullptr)
			return pass1->_state(index);
		else if (pass2 != nullptr)
			return pass2->_state(index);
		return dummy_state;
	}

	template <class Driver, size_t N>
	constexpr FSMTransition<Driver>& Builder<Driver, N>::_transition(size_t index)
	{
		if (index >= N)
			throw std::out_of_range("Index out of range");

		if (pass0 != nullptr)
			return pass0->_transition(cur_state, index);
		if (pass1 != nullptr)
			return pass1->_transition(cur_state, index);
		else if (pass2 != nullptr)
			return pass2->_transition(cur_state, index);
		return dummy_transition;
	}
}

namespace CxFSM::detail
{
	template <class T>
	static constexpr bool is_builder_v = false;
	template <class T, size_t N>
	static constexpr bool is_builder_v<Builder<T, N>> = true;

	auto arg_one(...) { return static_cast<void*>(0); }
	template <class RET, class T0>
	auto arg_one(RET(*)(T0)) { return static_cast<std::decay_t<T0>*>(0); }
	template <const auto& Func>
	using arg_one_t = std::remove_pointer_t<decltype(arg_one(Func))>;

	template <class FSM>
	auto BuilderOfImpl(...) { return static_cast<void*>(0); }
	template <class FSM, class Builder = arg_one_t<FSM::Blueprint>>
	auto BuilderOfImpl(std::enable_if_t<is_builder_v<Builder>, int>) { return static_cast<Builder*>(0); }
}

namespace CxFSM
{
	template <class FSM>
	using BuilderOf = std::remove_pointer_t<decltype(detail::BuilderOfImpl<FSM>(0))>;
}