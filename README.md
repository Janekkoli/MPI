# MIMPI

A compact, C-based implementation of a small subset of the MPI standard, built as part of the second laboratory assignment for the MIMUW PW course.

MIMPI provides a minimal process-management and communication layer that lets multiple program instances exchange messages through custom channels, simulate parallel execution, and support basic collective operations such as barriers, broadcasting, and reduction.

## Overview

This project is split into two core components:

- `mimpirun`: a launcher that starts multiple copies of a target program and configures the communication environment.
- `mimpi`: a lightweight MPI-like library implementing message passing and collective operations.

The implementation uses custom channel primitives instead of raw OS networking, making it suitable for educational and experimental parallel-programming exercises.

## Features

- Process spawning and per-rank identification
- Point-to-point communication via `MIMPI_Send` and `MIMPI_Recv`
- Global synchronization with `MIMPI_Barrier`
- Broadcast and reduction operations
- Deadlock detection support for certain circular wait patterns
- Custom channel-based interprocess communication layer

## Project structure

- `mimpirun.c` — runner that launches parallel processes
- `mimpi.c` — MPI-like library implementation
- `mimpi.h` — public API for the library
- `channel.c` / `channel.h` — interprocess communication primitives
- `mimpi_common.c` / `mimpi_common.h` — shared helpers and utilities
- `examples/` — sample programs that exercise the library
- `tests/` — self-checking programs and effectiveness scenarios
- `assignment.md` — task specification and requirements

## Build

To build the runner and example binaries:

```bash
make
```

This compiles:

- the main `mimpirun` executable
- example programs under `examples_build/`
- test programs from the `tests/` directory

## Run

```bash
./mimpirun <n> <program> [args...]
```

Example:

```bash
./mimpirun 4 ./examples_build/hello
```

This launches `n` instances of the specified executable and provides them with the MIMPI communication environment.

## Examples

The repository includes several example programs demonstrating communication patterns and edge cases, including:

- `hello`
- `send_recv`
- `broadcast`
- `reduction`
- `barrier`
- `deadlock`
- `pipe_closed`
- `obstruction`

## Testing

The project includes self-checking tests in `tests/` and effectiveness benchmarks under `tests/effectiveness/` and `tests/effectiveness_optimal/`.

To clean the build artifacts:

```bash
make clean
```

## License

This project is distributed under the license included in the repository.

## Notes

This repository is primarily an academic implementation exercise, so the focus is on correctness, synchronization semantics, and communication behavior rather than full production-grade MPI compatibility.

For the detailed specification, see [assignment.md](assignment.md).