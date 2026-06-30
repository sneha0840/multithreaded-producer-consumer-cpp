#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <chrono>

// Shared task queue between producer and consumer threads
std::queue<int> taskQueue;
std::mutex queueMutex;              // protects taskQueue from concurrent access
std::condition_variable cv;         // lets threads signal each other
bool productionDone = false;        // flag to tell consumers when producers are finished

const int NUM_PRODUCERS = 2;
const int NUM_CONSUMERS = 3;
const int TASKS_PER_PRODUCER = 5;

// Producer: generates tasks and pushes them into the shared queue
void producer(int id) {
    for (int i = 1; i <= TASKS_PER_PRODUCER; ++i) {
        int task = id * 100 + i; // unique task id per producer
        {
            std::lock_guard<std::mutex> lock(queueMutex); // lock before touching shared queue
            taskQueue.push(task);
            std::cout << "[Producer " << id << "] produced task " << task << std::endl;
        } // lock released here automatically
        cv.notify_one(); // wake up a waiting consumer
        std::this_thread::sleep_for(std::chrono::milliseconds(200)); // simulate work
    }
}

// Consumer: waits for tasks and processes them
void consumer(int id) {
    while (true) {
        std::unique_lock<std::mutex> lock(queueMutex);
        // Wait until there's a task OR production is done
        cv.wait(lock, [] { return !taskQueue.empty() || productionDone; });

        if (!taskQueue.empty()) {
            int task = taskQueue.front();
            taskQueue.pop();
            lock.unlock(); // unlock before doing "work" so other threads aren't blocked

            std::cout << "    [Consumer " << id << "] processing task " << task << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(300)); // simulate processing
        } else if (productionDone) {
            break; // no more tasks and producers are done -> exit
        }
    }
}

int main() {
    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    // Launch producer threads
    for (int i = 1; i <= NUM_PRODUCERS; ++i)
        producers.emplace_back(producer, i);

    // Launch consumer threads
    for (int i = 1; i <= NUM_CONSUMERS; ++i)
        consumers.emplace_back(consumer, i);

    // Wait for all producers to finish
    for (auto &p : producers) p.join();

    // Signal consumers that no more tasks will be produced
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        productionDone = true;
    }
    cv.notify_all(); // wake up all consumers so they can check the flag and exit

    // Wait for all consumers to finish
    for (auto &c : consumers) c.join();

    std::cout << "All tasks completed." << std::endl;
    return 0;
}
