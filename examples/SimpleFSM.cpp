#include <CxFSM/CxFSM.h>
#include <iostream>

struct SimpleFSM
{
	// Helper declarations
	enum States { ST_First, ST_Second, ST_Third, ST_COUNT };
	using Driver = CxFSM::TimedDriver<>;
	using Builder = CxFSM::Builder<Driver, ST_COUNT>;
	using Context = Driver::Context;

	static constexpr void Blueprint(Builder& b)
	{
		// Define some state data
		b.state[ST_First].name = "First";
		b.state[ST_First].duration = 1.0f;
		b.state[ST_Second].name = "Second";
		b.state[ST_Second].duration = 1.0f;
		b.state[ST_Third].name = "Third";
		b.state[ST_Third].duration = 1.0f;

		// Define some callbacks
		b.state[ST_First].OnUpdate = [](Context* ctx) { std::cout << "First - OnUpdate" << std::endl; };
		b.state[ST_First].OnExit = [](Context* ctx) { std::cout << "First - OnExit" << std::endl; };
		b.state[ST_Second].OnEnter = [](Context* ctx) { std::cout << "Second - OnEnter" << std::endl; };
		b.state[ST_Second].OnUpdate = [](Context* ctx) { std::cout << "Second - OnUpdate" << std::endl; };
		b.state[ST_Second].OnExit = [](Context* ctx) { std::cout << "Second - OnExit" << std::endl; };
		b.state[ST_Third].OnEnter = [](Context* ctx) { std::cout << "Third - OnEnter" << std::endl; };

		// Define transitions for exiting ST_First
		b.state[ST_First];
		b.transition[ST_Second].OnTest = [](Context* ctx) { return ctx->fsm->GetTime() >= ctx->fsm->GetState().duration; };
		b.transition[ST_Second].OnTraverse = [](Context* ctx) { std::cout << "First -> Second" << std::endl; };

		// Define transitions for exiting ST_Second
		b.state[ST_Second];
		b.transition[ST_Third].OnTest = [](Context* ctx) { return ctx->fsm->GetTime() >= ctx->fsm->GetState().duration; };
		b.transition[ST_Third].OnTraverse = [](Context* ctx) { std::cout << "Second -> Third" << std::endl; };
	}
};

int main()
{
	CxFSM::CompiledFSM<SimpleFSM> fsm;
	fsm.Update(0.5f);
	fsm.Update(0.5f);
	fsm.Update(0.5f);
	fsm.Update(0.5f);

	return 0;
}