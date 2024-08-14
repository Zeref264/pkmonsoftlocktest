#include <iostream>
#include <vector>
#include <random>
#include <thread>
#include <mutex>
#include <atomic>

const int NUM_THREADS = 16;  // Adjust based on your CPU's core count

std::mutex mtx;
std::atomic<int> globalMaxOnes(0);
std::atomic<long long> globalRolls(0);

void simulateRolls(long long maxRollsPerThread) {
    std::vector<int> items = {1, 2, 3, 4};
    std::vector<int> numbers(4, 0);
    int localMaxOnes = 0;
    long long localRolls = 0;

    // Initialize the random number generator for each thread
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 3);

    while (numbers[0] < 177 && localRolls < maxRollsPerThread) {
        numbers = {0, 0, 0, 0};
        for (int i = 0; i < 231; ++i) {
            int roll = items[dis(gen)];
            numbers[roll - 1]++;
        }
        localRolls++;
        if (numbers[0] > localMaxOnes) {
            localMaxOnes = numbers[0];
        }
    }
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (localMaxOnes > globalMaxOnes) {
            globalMaxOnes = localMaxOnes;
        }
    }
    globalRolls += localRolls;
}

int main() {
    long long totalMaxRolls = 1000000000;
    long long maxRollsPerThread = totalMaxRolls / NUM_THREADS;

    std::vector<std::thread> threads;

    // Create and start threads
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back(simulateRolls, maxRollsPerThread);
    }

    // Wait for all threads to complete
    for (auto& th : threads) {
        th.join();
    }

    std::cout << "Highest Ones Roll: " << globalMaxOnes.load() << std::endl;
    std::cout << "Number of Roll Sessions: " << globalRolls.load() << std::endl;

    return 0;
}
