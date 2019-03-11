# CxFSM - Compile-Time Expressed Finite State Machines

This C++17 library allows finite state machines to be constructed at compile-time.

# Motivation

Finite state machines occupy an awkward space between program data and program logic. While the goal of an FSM is to direct control flow, their graph-like structure usually leads to them being implemented as data constructs. This library aims to reduce the overhead of crunching data at runtime while at the same time leaveraging the performance benefits to be gained from compile time optimization.

# Usage

A minimally defined state machine:

```c++
struct MyFSM
{
    enum States { ST_First, ST_Second, NUM_STATES };
    using Driver = /* Core FSM Driver */
    using Builder = CxFSM::Builder<Driver, NUM_STATES>;
    static constexpr void Blueprint(Builder& builder)
    {
        /* State Logic */
    }
};
```
#### Drivers
Drivers implement the core logic that dictates control flow between states. CxFSM comes with 2 builtin drivers implementing 2 different FSM paradigms:

- `TimedDriver` - Implements a time based FSM that transitions when a state reaches the end of its duration.
- `MessageDriver` - Implements a more traditional message based FSM.

CxFSM is extensible allowing custom drivers to be implemented as needed by the developer.

#### State Logic
State logic is defined using the builder interface passed into the Blueprint function:

```c++
// State data
builder.state[ST_First].name = "First";
builder.state[ST_First].duration = 1.0f;

// Callbacks
builder.state[ST_Second].OnEnter = [](auto context) { /* callback body */ };
builder.state[ST_Second].OnUpdate = [](auto context) { /* callback body */ };

// Transition from ST_First to ST_Second
builder.state[ST_First];
builder.transition[ST_Second].OnTest = [](Context* ctx) { return /* bool expression */ };
builder.transition[ST_Second].OnTraverse = [](Context* ctx) { /* callback body */ };
```

The data exposed by the `state` and `transition` members will differ depending on the `Driver` type being used.

#### Execution

State machines are instantiated using the `CxFSM::CompiledFSM<T>` template type:

```c++
// Assuming MyFSM was implemented using the TimedDriver type
CxFSM::CompiledFSM<MyFSM> fsm;
fsm.Update(1.0f);
fsm.Update(1.0f);
```

Because invoking the FSM compiler multiple times can lead to expensive spikes in compile time, aim to keep instantiation of the `CxFSM::CompiledFSM<T>` template constrained to as few compilation units as possible. Opt to interact with the FSM via their driver interface wherever possible:

```c++
// Defined in a separate .cpp file
MyFSM::Driver* MyFSM::Make() { return new CxFSM::CompiledFSM<MyFSM>(); }
```
```c++
MyFSM::Driver* fsm = MyFSM::Make();
fsm->Update(1.0f);
fsm->Update(1.0f);
```

#### User Data

Drivers can expose user data in the form of a template type. This type contains user-defined runtime data that is accessible to both the state callback functions (via the `context` parameter) and to the instantiator of the FSM (via the `GetData` function). Both the `TimedDriver` and the `MessageDriver` types expose `UserData` template parameters.

### Example Code

See the [examples folder](https://github.com/JasonL663/CxFSM/tree/master/examples) for a full example of an FSM implementation.
