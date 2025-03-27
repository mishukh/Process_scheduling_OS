# CPU Scheduling process simulate and analysis  in C++

## How it looks like 
![image](https://github.com/user-attachments/assets/38778cf7-d7eb-4c75-8b59-c44ff6786454)
![image](https://github.com/user-attachments/assets/5248ec60-2446-4b6d-b90a-5cff8953daa0)
![image](https://github.com/user-attachments/assets/0067f260-ac81-4525-8ef1-fa9161a34e40)
![image](https://github.com/user-attachments/assets/3ae580d3-e3ce-4333-8e73-d25e653248e6)



## Project Description
This project is a comprehensive CPU scheduling simulator that implements and visualizes multiple CPU scheduling algorithms. It allows users to compare the performance of different scheduling strategies through metrics like average waiting time, CPU utilization, and throughput.

The simulator supports three operation modes:
- **Trace Mode**: Visualizes the execution timeline of processes for each algorithm
- **Statistics Mode**: Displays performance metrics and statistics
- **Compare Mode**: Performs comparative analysis across all selected algorithms

## Goals
- Implement and visualize common CPU scheduling algorithms
- Provide interactive comparison between different scheduling strategies
- Help understand the tradeoffs between various scheduling approaches
- Visualize process execution and waiting times

## Features
- Support for multiple scheduling algorithms (FCFS, Round Robin, SPN, SRT, HRRN, Feedback Queues, Aging)
- Colored visualization option for better readability
- Detailed performance metrics calculation
- Comparative analysis to identify the best algorithm for a given workload
- Efficient process management and scheduling simulation

## Data Structures and Algorithms

### Scheduling Algorithms Implemented
1. **First Come First Serve (FCFS)**: Processes are executed in the order they arrive
2. **Round Robin (RR)**: Time-sharing system using a fixed time quantum
3. **Shortest Process Next (SPN)**: Non-preemptive scheduling based on service time
4. **Shortest Remaining Time (SRT)**: Preemptive version of SPN
5. **Highest Response Ratio Next (HRRN)**: Non-preemptive scheduling based on response ratio
6. **Feedback Queue (FB-1)**: Multi-level feedback queue with single quantum
7. **Feedback Queue (FB-2i)**: Multi-level feedback queue with increasing quantum
8. **Aging Priority**: Priority-based scheduling with aging mechanism

### Key Data Structures
- **Priority Queues**: Used in SPN, SRT, and Feedback Queue algorithms for efficient process selection
- **Vectors & Tuples**: Store process information and execution timelines
- **Unordered Maps**: Track remaining service times and process indices
- **2D Vectors**: Represent the execution timeline for visualization

## C++ Implementation Details
- **Modular Design**: Separated into main logic, parsing, and visualization components
- **STL Usage**: Extensive use of STL containers and algorithms
- **Object-Oriented Approach**: Structured design with clear separation of concerns
- **Dynamic Memory Management**: Efficient handling of process data
- **Console-Based Visualization**: ANSI color codes for enhanced visualization
- **Algorithm Comparison**: Metrics-based comparison infrastructure

## Performance Metrics
The simulator calculates and compares several key performance metrics:
- **Average Waiting Time**: Average time processes spend waiting in the ready queue
- **CPU Utilization**: Percentage of time the CPU is busy
- **Throughput**: Number of processes completed per unit of time
- **Turnaround Time**: Total time taken from submission to completion
- **Normalized Turnaround Time**: Ratio of turnaround time to service time

## How to Use
1. Compile the program with a C++ compiler supporting C++11 or later
2. Run the executable
3. Follow the prompts to:
   - Select operation mode (trace/stats/compare)
   - Choose visualization type if in trace mode
   - Enter scheduling algorithms (comma-separated)
   - Enter simulation time and number of processes
   - Input process details (name, arrival time, service time)
4. Review the results based on your selected mode

### Example Input Format
```
Enter operation mode: trace
Choose visualization type: colored
Enter scheduling algorithms: 1,2-4,3
Enter total simulation time: 20
Enter the number of processes: 3

Enter process details:
P1,0,5
P2,2,3
P3,3,7
```

## Learnings
This project demonstrates:
- Implementation of various CPU scheduling algorithms
- Performance analysis and comparison of scheduling strategies
- Tradeoffs between response time, throughput, and CPU utilization
- Effective use of priority queues and other data structures for scheduler implementation
- Visualization techniques for process execution timelines


