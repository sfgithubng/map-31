#include <iostream>
#include <chrono>
#include <format>
#include <mutex>
#include <random>
#include <thread>

constexpr std::string percentageOutput = "{:>3}";
constexpr std::string threadIDOutput = "{:0>3}";
constexpr std::string timeOutput = "{:>7}";
constexpr std::string clearScreenStringSeq = "\x1B[2J\x1B[H";
const double outputWidth = 80;
const double completedProgress = 100;
const double oneSymbolPercentage = completedProgress / outputWidth;
const auto nonInitializedThread = std::thread::id{0};
std::mutex mutexObj;

struct threadSpec {
    std::thread::id systemThreadID;
    int32_t threadNumber;
    int32_t threadProgress;
    int32_t threadWorkTimeMilliSec;
};
std::vector<threadSpec> runningThreads;

int32_t getPositiveInputInt32Var(const std::string questionText) {
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

void displayProgress(std::vector<threadSpec>* runningThreads) {
    bool allThreadsDone{false};
    auto sleepTime = std::chrono::milliseconds(1000);
    while ((*runningThreads).size() == 0) {
        std::this_thread::sleep_for(sleepTime);
        std::cout << clearScreenStringSeq;
        std::cout << "No threads have been initialized yet, please wait." << std::endl;
    }
    while (!allThreadsDone) {
        std::this_thread::sleep_for(sleepTime);
        std::cout << clearScreenStringSeq;
        allThreadsDone = true;
        std::lock_guard<std::mutex> lockObj(mutexObj);
        for (const auto& runningThread : (*runningThreads)) {
            if (runningThread.threadProgress != completedProgress) {
                allThreadsDone = false;
                if (runningThread.systemThreadID == nonInitializedThread) {
                    continue;
                }
            }
            std::string threadNumberToPrint = std::format(threadIDOutput, runningThread.threadNumber);
            std::cout << "Thread #" << threadNumberToPrint << ": " << runningThread.systemThreadID << " [";
            double printedProgress = 0;
            while (printedProgress < completedProgress) {
                if (printedProgress <= runningThread.threadProgress) {
                    std::cout << "#";
                } else {
                    std::cout << " ";
                }
                printedProgress = printedProgress + oneSymbolPercentage;
            }
            std::string completedPercent = std::format(percentageOutput, std::to_string(runningThread.threadProgress)) + "%";
            std::string timeTaken = std::format(timeOutput, std::to_string(runningThread.threadWorkTimeMilliSec)) + "ms";
            std::cout << "] done: " << completedPercent << ", working for: " << timeTaken << std::endl;
        }
    }
    std::cout << "All threads done!" << std::endl;
}

void updateThreadProgress(const int32_t threadNumber) {
    srand(time(NULL));
    const auto processingStartTime{std::chrono::steady_clock::now()};
    double threadProgress = 0;
    while (threadProgress < completedProgress) {
        auto threadProgressSleepTime = std::chrono::milliseconds(rand() % 2000);
        std::this_thread::sleep_for(threadProgressSleepTime);
        auto threadProgressIncrement = rand() % 10;
        if (threadProgress + threadProgressIncrement < 100) {
            threadProgress += threadProgressIncrement;
        } else {
            threadProgress = 100;
        }
        std::lock_guard<std::mutex> lockObj(mutexObj);
        runningThreads.at(threadNumber).systemThreadID = std::this_thread::get_id();
        runningThreads.at(threadNumber).threadNumber = threadNumber;
        runningThreads.at(threadNumber).threadProgress = threadProgress;
        auto currentProcessingTime{std::chrono::steady_clock::now()};
        const std::chrono::duration<double> processingTime{currentProcessingTime - processingStartTime};
        const auto processingTimeMilliSec = round(processingTime.count()*1000);
        runningThreads.at(threadNumber).threadWorkTimeMilliSec = processingTimeMilliSec;
    }
}


int32_t main() {
    std::cout << clearScreenStringSeq;
    auto numThreads = getPositiveInputInt32Var("Please enter number of threads: ");

    auto* runningThreadsPtr = &runningThreads;
    auto displayProgressThread = std::thread(displayProgress, runningThreadsPtr);


    std::vector<std::thread> workerThreads{};
    for (int32_t threadNumber = 0; threadNumber < numThreads; ++threadNumber) {
        std::lock_guard<std::mutex> lockObj(mutexObj);
        runningThreads.push_back({nonInitializedThread, 0, 0});
        workerThreads.push_back(std::thread(updateThreadProgress, threadNumber));
    }
    for (auto& workerThread : workerThreads) {
        if (workerThread.joinable()) {
            workerThread.join();
        }
    }
    displayProgressThread.join();

    return EXIT_SUCCESS;
}