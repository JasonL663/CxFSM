#pragma once

#include "FSMState.h"
#include "FSMTransition.h"
#include <array>
#include <cstddef>

namespace CxFSM
{
	template <size_t N, size_t M>
	struct BuilderPass1Result
	{
		constexpr BuilderPass1Result() = default;

		size_t transition_count = 0;
		std::array<size_t, N> transition_counts{};
		std::array<size_t, M> transition_index_buffer{};
	};

	template <class Driver>
	struct BuilderPass1
	{
		constexpr BuilderPass1() = default;

		constexpr FSMState<Driver>& _state(size_t index);
		constexpr FSMTransition<Driver>& _transition(size_t state, size_t index);

		size_t state_count = 0;
		size_t* transition_count = nullptr;
		size_t* transition_counts = nullptr;
		size_t* transition_index_buffer = nullptr;
		const size_t* transition_start_indices = nullptr;
		const size_t* transition_upper_bounds = nullptr;

		FSMState<Driver> dummy_state{};
		FSMTransition<Driver> dummy_transition{};
	};

	template <class Driver>
	constexpr FSMState<Driver>& BuilderPass1<Driver>::_state(size_t index)
	{
		return dummy_state;
	}

	template <class Driver>
	constexpr FSMTransition<Driver>& BuilderPass1<Driver>::_transition(size_t state, size_t index)
	{
		size_t start = transition_start_indices[state];
		size_t end = start + transition_upper_bounds[state];
		for (size_t i = start; i < end && transition_index_buffer[i] != index; ++i)
			if (transition_index_buffer[i] == -1) {
				transition_index_buffer[i] = index;
				transition_counts[state]++;
				(*transition_count)++;
				break;
			}

		return dummy_transition;
	}
}