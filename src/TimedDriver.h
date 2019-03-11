#pragma once

#include "FSMDriver.h"
#include <cstddef>
#include <string_view>

namespace CxFSM
{
	template <class UserData = EmptyData>
	class TimedDriver
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
		constexpr TimedDriver() { };

		float GetTime() const { return time; }
		size_t GetStateID() const { return state; }
		UserData& GetData() { return data; }
		const UserData& GetData() const { return data; }

		virtual const size_t GetNumStates() const = 0;
		virtual const State& GetState() const = 0;
		virtual const State& GetState(size_t state_id) const = 0;

		virtual void Update(float delta) = 0;
		virtual void GoToState(size_t state_id) = 0;

	protected:
		float time = 0.0f;
		size_t state = 0;
		UserData data = {};
		Context context = { this, &data };
	};

	template <class UserData>
	struct TimedDriver<UserData>::Context
	{
		TimedDriver<UserData>* fsm;
		UserData* data;
		float delta;
	};

	template <class UserData>
	struct TimedDriver<UserData>::State
	{
		constexpr State() = default;

		std::string_view name = "";
		float duration = 0.0f;
		Callback<void> OnEnter = nullptr;
		Callback<void> OnUpdate = nullptr;
		Callback<void> OnExit = nullptr;
	};

	template <class UserData>
	struct TimedDriver<UserData>::Transition
	{
		constexpr Transition() = default;

		Callback<bool> OnTest = nullptr;
		Callback<void> OnTraverse = nullptr;
	};
}

#include "TimedDriver.inl"
