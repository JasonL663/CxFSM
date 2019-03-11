#pragma once

#include "FSMState.h"
#include "FSMTransition.h"
#include <array>
#include <cstddef>

namespace CxFSM
{
	template <class Driver_, size_t N, size_t M>
	struct Blueprint
	{
		using Driver = Driver_;
		using State = FSMState<Driver>;
		using Transition = FSMTransition<Driver>;

		constexpr Blueprint() = default;

		std::array<State, N> states{};
		std::array<Transition, M> transitions{};
		std::array<size_t, N> transition_counts{};
		std::array<size_t, M> destinations{};
	};
}

namespace CxFSM::detail
{
	template <size_t START, size_t... I>
	constexpr auto make_index_range(std::index_sequence<I...>)
	{
		return std::index_sequence<START + I...>();
	}

	template <size_t START, size_t COUNT>
	constexpr auto make_index_range()
	{
		return make_index_range<START>(std::make_index_sequence<COUNT>());
	}

	template <const auto& Blueprint>
	static constexpr size_t transition_index(size_t state)
	{
		size_t result = 0;
		for (size_t i = 0; i < state; ++i)
			result += Blueprint.transition_counts[i];
		return result;
	}

	template <const auto& Blueprint, size_t State>
	static constexpr auto transition_sequence()
	{
		constexpr size_t start = transition_index<Blueprint>(State);
		constexpr size_t count = Blueprint.transition_counts[State];

		return make_index_range<start, count>();
	}

	template <const auto& Blueprint>
	static constexpr auto state_sequence()
	{
		return std::make_index_sequence<Blueprint.states.size()>();
	}
}
