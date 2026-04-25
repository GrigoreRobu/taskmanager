#include "TaskManager.h"
#include <iostream>

int multiply(int a, int b) {
    return a * b;
}

int main() {
    ThreadPool pool(4);

    pool.Enqueue([] { std::cout << "Background task running..." << std::endl; });

    auto ticket = pool.Enqueue(multiply, 10, 5);

    std::cout << "The result is: " << ticket.get() << std::endl; 

    return 0;
}