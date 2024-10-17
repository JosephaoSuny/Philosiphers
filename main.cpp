#include <array>
#include <iostream>
#include <thread>
#include <mutex>
#include <random>
#include <algorithm>
#include <syncstream>

// Wait for all threads to be started before running
bool should_run = false;

void philosopher_loop(std::mutex* l_chopstick, std::mutex* r_chopstick, int name) {
    const int rand = static_cast<int>(std::random_device{}()) % 100;
    int hunger = std::clamp(rand, 25, 100);

    while (hunger != 0) {
        if (!should_run) continue;

        // If hungry pickup chopstick and eat until full
        if (hunger < 50) {
            if (l_chopstick->try_lock()) {
                if (r_chopstick->try_lock()) {
                    while (hunger < 100) hunger++;

                    l_chopstick->unlock();
                    r_chopstick->unlock();

                    continue;
                }
                l_chopstick->unlock();
            }
        }

        hunger--;

        std::osyncstream synced_out(std::cout);

        synced_out << "Philosopher " << name << " is " << hunger << " % hungry\n";
    }
}

int main() {
    // Make an array of "chopsticks" (mutexes)
    std::array chopsticks = {std::mutex(), std::mutex(), std::mutex(), std::mutex(), std::mutex()};
    // Make an array of "philosophers" (threads)
    std::array<std::thread, 5> threads = {  };

    for (int i = 0; i < 5; i++) {
        // The chopstick to the left and right of the current philosopher
        const auto left = &chopsticks[i];
        const auto right = &chopsticks[i == 4 ? 0 : i + 1];

        // The running thread
        auto thread = std::thread(philosopher_loop, left, right, i);

        threads[i] = std::move(thread);
    }

    // Now that the loop is over, all should run
    should_run = true;

    // Wait ten seconds so I can talk
    std::this_thread::sleep_for(std::chrono::seconds(10));

    // Join threads
    for (int i = 0; i < 5; i++) threads[i].join();

    return 0;
}
