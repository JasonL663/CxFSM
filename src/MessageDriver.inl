#pragma once

#include <cstddef>

namespace CxFSM
{
	template <class Message, class UserData>
	template <const auto& Blueprint>
	class MessageDriver<Message, UserData>::Impl : public MessageDriver<Message, UserData>
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

		virtual void Update(Message msg) override
		{
			context.fsm = this;
			context.data = &data;
			context.msg = &msg;

			OnMessage(SSEQ);

			int32_t next = OnTest(SSEQ);
			if (next != -1)
			{
				OnExit(SSEQ);
				OnTraverse(SSEQ, next);
				state = next;
				OnEnter(SSEQ);
			}
		}

		virtual void GoToState(size_t state_id) override
		{
			if (state_id == state)
				return;

			OnExit(SSEQ);
			state = state_id;
			OnEnter(SSEQ);
		}

	private:
		template <size_t... I>
		void OnMessage(std::index_sequence<I...>)
		{
			static constexpr auto& S = Blueprint.states;
			(void)((state == I && S[I].OnMessage != nullptr && (S[I].OnMessage(&context), true)) || ...);
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
