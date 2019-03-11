#pragma once

#include "FSMState.h"
#include "FSMTransition.h"
#include <array>
#include <cstddef>

namespace CxFSM
{
	template <size_t N>
	struct BuilderPass0Result
	{
		constexpr BuilderPass0Result() = default;

		size_t transitions_upper_bound = 0;
		std::array<size_t, N> upper_bounds_by_state{};
	};

	template <class Driver>
	struct BuilderPass0
	{
		constexpr BuilderPass0() = default;

		constexpr FSMState<Driver>& _state(size_t index);
		constexpr FSMTransition<Driver>& _transition(size_t state, size_t index);

		size_t* transitions_upper_bound = nullptr;
		size_t* upper_bounds_by_state = nullptr;

		size_t last_state = static_cast<size_t>(-1);
		size_t last_transition = static_cast<size_t>(-1);

		FSMState<Driver> dummy_state{};
		FSMTransition<Driver> dummy_transition{};
	};

	template <class Driver>
	constexpr FSMState<Driver>& BuilderPass0<Driver>::_state(size_t index)
	{
		if (index != last_state) {
			last_state = index;
			last_transition = -1;
		}

		return dummy_state;
	}

	template <class Driver>
	constexpr FSMTransition<Driver>& BuilderPass0<Driver>::_transition(size_t state, size_t index)
	{
		if (last_transition != index) {
			upper_bounds_by_state[state]++;
			(*transitions_upper_bound)++;
			last_transition = index;
		}
		return dummy_transition;
	}
}