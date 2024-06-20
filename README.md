# Real-Time CPU Scheduling Implementation

## Project Overview

This project involves the implementation of two algorithms for simulating real-time systems: Rate-Monotonic (rate) and Earliest-Deadline-First (edf). The total simulation time and the periodic tasks to be scheduled will be described in an input file formatted as follows:

```
[TOTAL TIME]
[TASK NAME 1] [PERIOD 1] [CPU BURST 1]
[TASK NAME 2] [PERIOD 2] [CPU BURST 2]
...
[TASK NAME n] [PERIOD n] [CPU BURST n]
```

## Important Deadlines and Submission Guidelines

1. **Deadline**: Submissions after the specified deadline (8:00 am on 11.04) will not be accepted. Students with an official extension have until 8:00 am on 15.04.
2. **File Naming**:
    - **Implementation File**: The implementation file must be named `real.c`.
    - **Makefile**: Ensure the Makefile is named exactly `Makefile`.
    - **Directory Naming**: Save both files in a directory named after the initials of your email in lowercase. For example, if your email is `gst@cesar.school`, the directory should be named `gst`.
    - **Archive**: The directory should be compressed into a `.tar` file named after the initials of your email. For example, `gst.tar`.
3. **Submission Method**: Submit the `.tar` file via the Google Classroom form for the activity.
4. **File Contents**: The directory should only contain `real.c` and `Makefile`. Any additional files will result in a zero grade.
5. **Originality**: Any detected plagiarism will result in a zero grade.
6. **Compilation**: Submissions that fail to compile using the `make` command as specified will receive a zero grade.

## Implementation Details

### Real-Time Scheduling Requirements

- **Programming Language**: The program must be implemented in C and executable on Linux, Unix, or macOS systems.
- **Compilation**: Use `Makefile` to compile the program. The `make` command will receive the algorithm to be compiled (`rate` or `edf`), resulting in an executable named accordingly. For example:
  - `make rate` compiles for Rate-Monotonic and produces the executable `rate`.
  - `make edf` compiles for Earliest-Deadline-First and produces the executable `edf`.
- **Execution**: Run the executable using the command `./rate [namefile]` or `./edf [namefile]`, where `[namefile]` is the name of the input file containing the total simulation time and the list of periodic tasks.
- **Input File**: The input file will not have a blank line at the end. Each line will have a single space between elements, with no trailing spaces.
- **Task Execution**: All tasks described in the input file arrive at the system simultaneously at time 0.
- **Validation**: The implementation will be validated using GCC 13.2.0, ensuring strict adherence to the specified output format and text.
- **Priority Handling**: In case of a tie by priority, tasks will be executed based on the FCFS algorithm.
- **Task Completion**: If a task A finishes exactly when a higher-priority task B arrives, task A is considered completed.
- **Process Handling**: If a process A arrives exactly when the simulation ends, it is considered KILLED and does not need to be indicated as having executed 0 time units.
- **Numeric Values**: Burst and Period values are always integers.
- **Lost Tasks**: If a task is lost at time T but does not execute any units, it does not need to be shown as lost using 0 units.
- **Deadlines**: The deadline for calculating the priority of tasks for both rate and edf is the given period.

### Output File

- **Output File**: The output file should be named `algorithm.out`, where `algorithm` is the scheduling method used (`rate` or `edf`), and must follow the exact formatting provided in the Classroom activity. Pay close attention to spaces and the absence of an extra line at the end of the file.

#### Example

Given an input file `input.txt` with the following content:

```
165
T1 50 25
T2 80 35
```

This indicates that the execution will last 165 time units (t.u.) with two tasks arriving simultaneously:
- Task T1 arrives every 50 t.u. and requires 25 t.u. to execute.
- Task T2 arrives every 80 t.u. and requires 35 t.u. to execute.

Running `./rate input.txt` should create the file `rate.out` with the following content:

```
EXECUTION BY RATE
[T1] for 25 units - F
[T2] for 25 units - H
[T1] for 25 units - F
[T2] for 5 units - L
[T2] for 20 units - H
[T1] for 25 units - F
[T2] for 15 units - F
idle for 10 units
[T1] for 15 units - K

LOST DEADLINES
[T1] 0
[T2] 1

COMPLETE EXECUTION
[T1] 3
[T2] 1

KILLED
[T1] 1
[T2] 1
```

Explanation:
- `EXECUTION BY RATE`: The scheduling algorithm used.
- `[T1] for 25 units - F`: Task T1 executed for 25 t.u. and finished (F).
- `[T2] for 25 units - H`: Task T2 executed for 25 t.u. and is on hold (H).
- `[T1] for 25 units - F`: Task T1 executed for 25 t.u. and finished (F).
- `[T2] for 5 units - L`: Task T2 executed for 5 t.u. but missed the deadline and was lost (L).
- `[T2] for 20 units - H`: Task T2 executed for 20 t.u. and is on hold (H).
- `[T1] for 25 units - F`: Task T1 executed for 25 t.u. and finished (F).
- `[T2] for 15 units - F`: Task T2 executed for the remaining 15 t.u. and finished (F).
- `idle for 10 units`: No tasks to execute for 10 t.u.
- `[T1] for 15 units - K`: Task T1 executed for 15 t.u. and was killed (K) as time expired.
- `LOST DEADLINES`: Indicating how many deadlines were missed by each task.
- `COMPLETE EXECUTION`: Number of tasks that completed during the total time.
- `KILLED`: Number of tasks removed as time expired.

**Note**: Ensure there are no trailing spaces at the end of each line, only one space separating elements in each line, and no blank line at the end of the file.

### Scoring

- **Test 1**: 20%
- **Test 2**: 20%
- **Test 3**: 30%
- **Test 4**: 30%

Make sure to follow all implementation and submission guidelines strictly to avoid disqualification. Good luck!
