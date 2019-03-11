# CxFSM - Constexpr Finite State Machines

This C++17 library allows finite state machines to be constructed and instantiated at compile time.

# Motivation

Finite state machines occupy an awkward space between program data and program logic. While the goal of an FSM is to direct control flow, their graph-like structure usually leads to them being implemented as data constructs. This library aims to reduce the overhead of crunching data at runtime while at the same time leaveraging the performance benefits to be gained from compile time optimization.

# Usage

See the [examples folder](https://github.com/JasonL663/CxFSM/tree/master/examples) for a brief guide to authoring and running state machines.
