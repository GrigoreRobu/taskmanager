# TaskManager

A lightweight, header-only C++ thread pool for running tasks asynchronously across multiple threads.
Yes i know using the implemented SafeQueue is suboptimal.

## Features

- Header-only — just include `TaskManager.h`, no linking required
- Supports any callable (functions, lambdas, functors) via `Enqueue`
- Returns `std::future` so you can retrieve results from async tasks
- Thread-safe task queue using `std::mutex`
- Clean shutdown — destructor waits for all queued tasks to finish

## Usage

```cpp
#include "TaskManager.h"

int multiply(int a, int b) {
    return a * b;
}

int main() {
    ThreadPool pool(4); // 4 worker threads

    // Fire-and-forget task
    pool.Enqueue([] {
        std::cout << "Background task running..." << std::endl;
    });

    // Task with a return value
    auto result = pool.Enqueue(multiply, 10, 5);
    std::cout << "Result: " << result.get() << std::endl;

    return 0;
}
```

## Building

Requires C++17 or later.

```bash
g++ -std=c++17 -pthread main.cpp -o main
```

## How it works

`ThreadPool` spawns N worker threads on construction. Each thread waits on a condition variable until a task is available in the internal `SafeQueue`. `Enqueue` wraps any callable into a `std::packaged_task`, pushes it onto the queue, and returns a `std::future` for the result. On destruction, the pool sets a stop flag, wakes all threads, and joins them.