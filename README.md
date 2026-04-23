# Thread Manager with Task Queue

A multithreaded task scheduling system in C++ that manages a worker thread pool with a request-based task distribution mechanism, accompanied by a Python-based real-time thread monitoring GUI.

---

## Table of Contents

- [Overview](#overview)
- [Architecture](#architecture)
- [Components](#components)

---

## Overview

This project implements a **producer-consumer pattern** with a centralized task queue and a scheduler thread that distributes tasks to worker threads on demand. Workers request tasks when idle, and the scheduler assigns them from the queue.

---

## Architecture
```mermaid
stateDiagram
[*] --> Created: create_thread_manager

    Created --> Stopped: is_running = 0
    
    Stopped --> Running: start_manager()
    
    state Running {
        [*] --> MainRunning
        
        state MainRunning {
            [*] --> AddingTasks
            AddingTasks --> Waiting: sleep()
            Waiting --> AddingTasks: add_task()
        }
        
        state SchedulerRunning {
            [*] --> WaitingForRequests
            WaitingForRequests --> ProcessingRequest: cond1 signal
            ProcessingRequest --> AssigningTask: pop_request + pop_from_queue
            AssigningTask --> SignalingWorker: task assigned
            SignalingWorker --> WaitingForRequests: done
        }
        
        state WorkerRunning {
            [*] --> SendingRequest
            SendingRequest --> WaitingForTask: cond_wait
            WaitingForTask --> ExecutingTask: cond_signal + has_task
            ExecutingTask --> TaskComplete: task() returns
            TaskComplete --> SendingRequest: loop
        }
    }
    
    Running --> Terminating: stop_manager()
    Terminating --> [*]: destroy_manager()
```
## Components
- **ThreadManager** - Main controller managing worker threads, queues, and scheduler
- **Task Queue** - FIFO queue storing tasks (`TaskFunc` function pointers)
- **Request Queue** - Queue storing pending task requests from worker threads
- **Scheduler Thread** - Processes worker requests and assigns tasks from main queue
- **Worker Threads** - Execute computational tasks and request new work when idle