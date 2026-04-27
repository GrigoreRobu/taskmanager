#include "TaskManager.h"
#include <iostream>
#include <chrono>

int multiply(int a, int b) {
    return a * b;
}

int main() {
    ThreadPool pool(12);
    auto start = std::chrono::high_resolution_clock::now();

    for(int i = 0; i<1000000;i++){
        auto ticket = pool.Enqueue(multiply, 10, i);
    }
    std::cout <<"ceva"<<std::endl;
    auto end = std::chrono::high_resolution_clock::now();

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Total time: " << ms << " ms\n";
    return 0;
}