<div align="center">

# CPU Scheduler Analyzer

### C++ Simulation of SPN, HRRN, and Feedback Queue Scheduling

[![C++](https://img.shields.io/badge/C%2B%2B-17-00599C?logo=cplusplus&logoColor=white)](https://isocpp.org/)
[![Make](https://img.shields.io/badge/Build-Make-427819)]()

</div>

## Overview

This command-line simulator models and compares CPU scheduling policies over a process workload. It produces scheduling traces and per-process statistics for three strategies: shortest process next, highest response ratio next, and multi-level feedback queues with aging.

## Scheduling Policies

| Policy | Description |
| --- | --- |
| SPN | Selects the shortest available process next |
| HRRN | Selects by highest response ratio to account for waiting time |
| FB-2i | Uses feedback queues with exponentially increasing quanta and aging |

## Build and Run

```bash
make
./scheduler
```

The `Makefile` compiles `src.cpp` with `g++`. Name the executable `scheduler` when building manually with `g++ src.cpp -o scheduler`.

## Output

The simulator reports each policy's scheduling timeline along with waiting, turnaround, and response-time statistics for the submitted process set.

## Repository Structure

```text
.
├── src.cpp       # Scheduling simulation and reporting
├── Makefile       # Build instructions
└── Makefile        # Build configuration
```

## Author

**Youssef Mohamed Attia**  
[GitHub](https://github.com/YoussefMohAttia) · [LinkedIn](https://www.linkedin.com/in/youssefmohattia/)
