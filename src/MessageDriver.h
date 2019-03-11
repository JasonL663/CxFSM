#pragma once

#include "FSMDriver.h"
#include <cstddef>
#include <string_view>

namespace CxFSM
{
	template <class Message = EmptyData, class UserData = EmptyData>
	class MessageDriver
	{
	public:
		struct Context;
		struct State;
		struct Transition;

		template <const auto& Blueprint>
		class Impl;
		template <class Ret, class... Args>
		using Callback = Ret(*)(Context*, Args...);
	public:
		constexpr MessageDriver() { };

		float GetTime() const { return time; }
		size_t GetStateID() const { return state; }
		UserData& GetData() { return data; }
		const UserData& GetData() const { return data; }

		virtual const size_t GetNumStates() const = 0;
		virtual const State& GetState() const = 0;
		virtual const State& GetState(size_t state_id) const = 0;

		virtual void Update(Message msg) = 0;
		virtual void GoToState(size_t state_id) = 0;

	protected:
		float time = 0.0f;
		size_t state = 0;
		UserData data = {};
		Context context = { this, nullptr };
	};

	template <class MessageType, class UserData>
	struct MessageDriver<MessageType, UserData>::Context
	{
		MessageDriver<MessageType>* fsm;
		MessageType* msg;
	};

	template <class MessageType, class UserData>
	struct MessageDriver<MessageType, UserData>::State
	{
		constexpr State() = default;

		std::string_view name = "";
		Callback<void> OnEnter = nullptr;
		Callback<void> OnMessage = nullptr;
		Callback<void> OnExit = nullptr;
	};

	template <class MessageType, class UserData>
	struct MessageDriver<MessageType, UserData>::Transition
	{
		constexpr Transition() = default;

		Callback<bool> OnTest = nullptr;
		Callback<void> OnTraverse = nullptr;
	};
}

#include "MessageDriver.inl"
