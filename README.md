# CPU-Scheduling-Simulation
This program implements three classic CPU scheduling algorithms—First-Come/First-Served (FCFS), Shortest-Job-First (SJF), and Round-Robin (RR)—and prints a summary table of each process’s start time, finish time, turnaround time, waiting time, and response time. For RR, it also prints a timeline of quantum slices.

---

## Overview

This C program prompts the user to select one of three scheduling algorithms (FCFS, SJF, or RR), then reads a list of processes (PID, arrival time, burst time), schedules them accordingly, and prints:

- A detailed table showing, for each process:
  - **PID**  
  - **Arrival time**  
  - **Burst time**  
  - **Start time**  
  - **Finish time**  
  - **Turnaround time** (Finish − Arrival)  
  - **Waiting time** (Turnaround − Burst)  
  - **Response time** (Start − Arrival)  

- Average turnaround time, average waiting time, and average response time across all processes.  
- If Round-Robin is chosen, a timeline of quantum slices in the form `[t0–t1] Pid | [t1–t2] Pid | …`.

---



