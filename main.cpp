#include <array>
#include <iostream>
#include <thread>
#include <mutex>
#include <random>
#include <algorithm>

struct Chopstick {
    std::mutex* lock = new std::mutex();
};

int main() {
    std::array chopsticks = {Chopstick(), Chopstick(), Chopstick(), Chopstick(), Chopstick()};
    auto* threads = new std::thread[5];

    for (int i = 0; i < 5; i++) {
        auto left_idx = i;
        auto right_idx = i == 4 ? 0 : i + 1;

        auto thread = std::thread([chopsticks](const int l_idx, const int r_idx) {
            const int rand = static_cast<int>(random()) % 100;
            int hunger = std::clamp(rand, 25, 100);

            std::cout << "Initial Hunger" << hunger << '\n';

            while (hunger != 0) {
                if (hunger < 50) {
                    if (chopsticks[l_idx].lock->try_lock()) {
                        if (chopsticks[r_idx].lock->try_lock()) {
                            while (hunger < 100) hunger++;

                            chopsticks[r_idx].lock->unlock();
                            chopsticks[l_idx].lock->unlock();
                        } else {
                            chopsticks[l_idx].lock->unlock();
                            hunger--;
                        }
                    } else {
                        hunger--;
                    }
                } else {
                    hunger--;
                }

                printf("Philosopher %d is %d%% hungry\n", l_idx, hunger);
                fflush(stdout);
            }

            exit(1);
        }, left_idx, right_idx);

        threads[i] = std::move(thread);
    }

    for (int i = 0; i < 5; i++) threads[i].join();

    free(threads);

    return 0;
}
