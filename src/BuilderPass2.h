#pragma once

#include "FSMState.h"
#include "FSMTransition.h"
#include <cstddef>
#include <stdexcept>

namespace CxFSM
{
	template <class States, class Transitions>
	struct BuilderPass2Result
	{
		constexpr BuilderPass2Result() = default;

		States states{};
		Transitions transitions{};
	};

	template <class Driver>
	struct BuilderPass2
	{
		constexpr BuilderPass2() = default;

		constexpr FSMState<Driver>& _state(size_t index);
		constexpr FSMTransition<Driver>& _transition(size_t state, size_t index);

		size_t state_count = 0;
		FSMState<Driver>* states = nullptr;
		FSMTransition<Driver>* transitions = nullptr;
		
		const size_t* transition_start_indices = nullptr;
		const size_t* transition_counts = nullptr;
		const size_t* transition_index_buffer = nullptr;

		FSMTransition<Driver> dummy_transition;
	};

	template <class Driver>
	constexpr FSMState<Driver>& BuilderPass2<Driver>::_state(size_t index)
	{
		return states[index];
	}

	template <class Driver>
	constexpr FSMTransition<Driver>& BuilderPass2<Driver>::_transition(size_t state, size_t index)
	{
		size_t start = transition_start_indices[state];
		size_t end = start + transition_counts[state];
		for (size_t i = start; i < end; ++i)
			if (transition_index_buffer[i] == index)
				return transitions[i];

		if (bool b = true) // gcc constexpr throw guard
			throw std::runtime_error("Could not find transition in index buffer.");

		return dummy_transition;
	}
}