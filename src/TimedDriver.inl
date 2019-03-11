#pragma once

#include <cstddef>

namespace CxFSM
{
	template <class UserData>
	template <const auto& Blueprint>
	class TimedDriver<UserData>::Impl : public TimedDriver<UserData>
	{
		template <size_t I>
		static constexpr auto TSEQ = detail::transition_sequence<Blueprint, I>();
		static constexpr auto SSEQ = detail::state_sequence<Blueprint>();

	public:
		constexpr Impl() { }

		virtual const size_t GetNumStates() const override
		{
			return Blueprint.states.size();
		}

		virtual const State& GetState() const override
		{
			return Blueprint.states[state];
		}

		virtual const State& GetState(size_t state_id) const override
		{
			return Blueprint.states[state_id];
		}

		virtual void Update(float delta) override
		{
			time += delta;

			context.fsm = this;
			context.data = &data;
			context.delta = delta;

			OnUpdate(SSEQ);

			int32_t next = OnTest(SSEQ);
			while (next != -1)
			{
				OnExit(SSEQ);
				OnTraverse(SSEQ, next);
				state = next;
				time = 0.0f;
				OnEnter(SSEQ);
				next = OnTest(SSEQ);
			}
		}

		virtual void GoToState(size_t state_id) override
		{
			if (state_id == state)
				return;

			OnExit(SSEQ);
			state = state_id;
			time = 0.0f;
			OnEnter(SSEQ);
		}

	private:
		template <size_t... I>
		void OnUpdate(std::index_sequence<I...>)
		{
			static constexpr auto& S = Blueprint.states;
			(void)((state == I && S[I].OnUpdate != nullptr && (S[I].OnUpdate(&context), true)) || ...);
		}

		template <size_t... I>
		void OnEnter(std::index_sequence<I...>)
		{
			static constexpr auto& S = Blueprint.states;
			(void)((state == I && S[I].OnEnter != nullptr && (S[I].OnEnter(&context), true)) || ...);
		}

		template <size_t... I>
		void OnExit(std::index_sequence<I...>)
		{
			static constexpr auto& S = Blueprint.states;
			(void)((state == I && S[I].OnExit != nullptr && (S[I].OnExit(&context), true)) || ...);
		}

		template <size_t... I>
		size_t OnTest(std::index_sequence<I...>)
		{
			size_t outIndex = -1;
			(void)((state == I && (OnTestImpl(TSEQ<I>, outIndex), true)) || ...);
			return outIndex;
		}

		template <size_t... I>
		void OnTestImpl(std::index_sequence<I...>, size_t& outIndex)
		{
			static constexpr auto& T = Blueprint.transitions;
			static constexpr auto& D = Blueprint.destinations;

			(void)((T[I].OnTest != nullptr && T[I].OnTest(&context) && (outIndex = D[I], true)) || ...);
		}

		template <size_t... I>
		void OnTraverse(std::index_sequence<I...>, size_t next)
		{
			(void)((state == I && (OnTraverseImpl(TSEQ<I>, next), true)) || ...);
		}

		template <size_t... I>
		void OnTraverseImpl(std::index_sequence<I...>, size_t next)
		{
			static constexpr auto& T = Blueprint.transitions;
			static constexpr auto& D = Blueprint.destinations;

			(void)((D[I] == next && T[I].OnTraverse != nullptr && (T[I].OnTraverse(&context), true)) || ...);
		}
	};
}