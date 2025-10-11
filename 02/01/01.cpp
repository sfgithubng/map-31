#include <atomic>
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>

std::atomic<int32_t> currentClientAmount{};
bool allClientsAdded{false};

int32_t getPositiveInputInt32Var(const std::string& questionText) {
    std::string userInputString{};
    std::string invalidInputMessage{"The number must be a positive integer number. Please try again."};
    int32_t userInputInt{};
    while (userInputString.empty()) {
        std::cout << questionText;
        std::getline(std::cin, userInputString);
        try {
            userInputInt = std::stoll(userInputString);
            if (userInputInt <= 0) {
                std::cout << invalidInputMessage << std::endl;
                userInputString = "";
            }
        } catch (...) {
            std::cout << invalidInputMessage << std::endl;
            userInputString = "";
        }
    }
    return userInputInt;
}

void addClientsToTheQueue(int32_t maxClientAmount) {
    auto queueAdditionTimer = std::chrono::seconds(1);
    for (int32_t client = 0;  client < maxClientAmount; client++) {
        std::this_thread::sleep_for(queueAdditionTimer);
        currentClientAmount.store(++currentClientAmount, std::memory_order_seq_cst);
        std::cout << "Added client to the queue. Current queue length: " << currentClientAmount.load() << std::endl;
    }
    allClientsAdded = true;
    std::cout << "Done adding clients to the queue. Total clients added: " << maxClientAmount << std::endl;
}

void serveClientsInTheQueue() {
    auto queueProcessingTimer = std::chrono::seconds(2);
    bool skipQueueProcessing = false;
    bool stopQueueProcessing = false;
    while (true) {
        std::this_thread::sleep_for(queueProcessingTimer);
        if (currentClientAmount.load() == 0) {
            if (allClientsAdded) {
                stopQueueProcessing = true;
            } else {
                skipQueueProcessing = true;
            }
        }
        if (skipQueueProcessing) {
            skipQueueProcessing = false;
            continue;
        }
        if (stopQueueProcessing) {
            std::cout << "Processed all clients in the queue" << std::endl;
            break;
        }
        currentClientAmount.store(--currentClientAmount, std::memory_order_seq_cst);
        std::cout << "Processed client in the queue. Current queue length: " << currentClientAmount.load() << std::endl;
    }
}

int32_t main() {
    auto maxClientAmount = getPositiveInputInt32Var("Please enter maximum amount of clients: ");

    std::vector<std::thread> workerThreads{};
    workerThreads.push_back(std::thread(addClientsToTheQueue, maxClientAmount));
    workerThreads.push_back(std::thread(serveClientsInTheQueue));

    for (auto& workerThread : workerThreads) {
        if (workerThread.joinable()) {
            workerThread.join();
        }
    }

    return EXIT_SUCCESS;
}