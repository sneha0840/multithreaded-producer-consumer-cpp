# Multithreaded Producer-Consumer Task Queue (C++)

A small C++ project demonstrating concurrent programming fundamentals: multiple producer threads generate tasks into a shared queue, while multiple consumer threads process them concurrently and safely.

## Concepts demonstrated
- Multi-threading using std::thread
- Synchronization using std::mutex
- Thread signaling using std::condition_variable
- Producer-consumer design pattern

## How to run
g++ -std=c++17 -pthread main.cpp -o main
./main
