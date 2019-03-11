#pragma once

#include "Blueprint.h"
#include "BlueprintCompilerPrecheck.h"
#include "Builder.h"
#include "FSMDriver.h"
#include "FSMState.h"
#include "FSMTransition.h"
#include <cstddef>

namespace CxFSM
{
	template <class FSM, bool = BlueprintCompilerPrecheck<FSM>::value>
	struct BlueprintCompiler
	{
		using Builder = BuilderOf<FSM>;
		using Driver = typename Builder::Driver;
		using State = FSMState<Driver>;
		using Transition = FSMTransition<Driver>;

		static constexpr auto Pass0()
		{
			constexpr size_t NUM_STATES = Builder::NUM_STATES;
			BuilderPass0Result<NUM_STATES> result{};
			BuilderPass0<Driver> pass{};
			pass.transitions_upper_bound = &result.transitions_upper_bound;
			pass.upper_bounds_by_state = result.upper_bounds_by_state.data();

			Builder builder{};
			builder.pass0 = &pass;
			
			FSM::Blueprint(builder);

			return result;
		}
		static constexpr auto PASS0 = Pass0();
		
		static constexpr auto Pass1()
		{
			constexpr size_t NUM_STATES = Builder::NUM_STATES;

			BuilderPass1Result<NUM_STATES, PASS0.transitions_upper_bound> result{};
			for (size_t i = 0; i < result.transition_index_buffer.size(); ++i)
				result.transition_index_buffer[i] = -1;

			std::array<size_t, NUM_STATES> transition_start_indices{};
			for (size_t i = 1; i < transition_start_indices.size(); ++i)
				transition_start_indices[i] = transition_start_indices[i - 1] + PASS0.upper_bounds_by_state[i - 1];

			BuilderPass1<Driver> pass{};
			pass.state_count = NUM_STATES;
			pass.transition_count = &result.transition_count;
			pass.transition_index_buffer = result.transition_index_buffer.data();
			pass.transition_counts = result.transition_counts.data();
			pass.transition_start_indices = transition_start_indices.data();
			pass.transition_upper_bounds = PASS0.upper_bounds_by_state.data();

			Builder builder{};
			builder.pass1 = &pass;

			FSM::Blueprint(builder);

			// Compact the lists in the buffer into a single contiguous list
			size_t next = 0;
			for (size_t i = 0; i < result.transition_index_buffer.size(); ++i) {
				if (result.transition_index_buffer[i] != -1 && i != next) {
					result.transition_index_buffer[next] = result.transition_index_buffer[i];
					result.transition_index_buffer[i] = -1;
					next++;
				} else if (result.transition_index_buffer[next] != -1) {
					next++;
				}
			}

			return result;
		}
		static constexpr auto PASS1 = Pass1();
		
		static constexpr auto Pass2()
		{
			constexpr size_t NUM_STATES = Builder::NUM_STATES;
			constexpr size_t NUM_TRANSITIONS = PASS1.transition_count;
			
			using States = std::array<State, NUM_STATES>;
			using Transitions = std::array<Transition, NUM_TRANSITIONS>;
			BuilderPass2Result<States, Transitions> result{};

			std::array<size_t, NUM_STATES> transition_start_indices{};
			for (size_t i = 1; i < transition_start_indices.size(); ++i)
				transition_start_indices[i] = transition_start_indices[i - 1] + PASS1.transition_counts[i - 1];
			
			BuilderPass2<Driver> pass{};
			pass.state_count = result.states.size();
			pass.states = result.states.data();
			pass.transitions = result.transitions.data();
			pass.transition_start_indices = transition_start_indices.data();
			pass.transition_counts = PASS1.transition_counts.data();
			pass.transition_index_buffer = PASS1.transition_index_buffer.data();

			Builder builder{};
			builder.pass2 = &pass;

			FSM::Blueprint(builder);

			return result;
		}
		static constexpr auto PASS2 = Pass2();
		
		static constexpr auto Compile()
		{
			constexpr size_t NUM_STATES = PASS2.states.size();
			constexpr size_t NUM_TRANSITIONS = PASS2.transitions.size();

			Blueprint<Driver, NUM_STATES, NUM_TRANSITIONS> blueprint;
			blueprint.states = PASS2.states;
			blueprint.transitions = PASS2.transitions;
			blueprint.transition_counts = PASS1.transition_counts;
			for (size_t i = 0; i < NUM_TRANSITIONS; ++i)
				blueprint.destinations[i] = PASS1.transition_index_buffer[i];

			return blueprint;
		}
	};

	template <class FSM>
	struct BlueprintCompiler<FSM, false>
	{
		using Driver = NullDriver;
		using Builder = CxFSM::Builder<Driver, 0>;
		using State = FSMState<Driver>;
		using Transition = FSMTransition<Driver>;

		static constexpr auto Compile()
		{
			return Blueprint<NullDriver, 0, 0>();
		}
	};
}