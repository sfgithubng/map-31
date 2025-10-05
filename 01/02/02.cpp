#include <chrono>
#include <format>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

constexpr std::string columnHeaderOutputFormat = "{:^25}";
constexpr std::string columnProcesingTimeOutputFormat = "{:.4f}{}";

void populateVectorWithRandomData(std::vector<int32_t>& vectorToPopulate) {
    std::mt19937 randGenerator;
    std::uniform_int_distribution<int32_t> random1KDistr(0, 1000);
    for (auto& vectorElement : vectorToPopulate) {
        vectorElement = random1KDistr(randGenerator);
    };
}

void sumVectorsWorker(const std::vector<int32_t>& vectorA,
                   const std::vector<int32_t>& vectorB,
                   std::vector<int32_t>& vectorResult,
                   const int32_t& chunkStartIndex,
                   const int32_t& chunkEndIndex) {
    for (int32_t elementIndex = chunkStartIndex; elementIndex <= chunkEndIndex; elementIndex++) {
        vectorResult.at(elementIndex) = vectorA.at(elementIndex) + vectorB.at(elementIndex);
    }
}

std::vector<int32_t> sumVectorsWithThreading(const std::vector<int32_t>& vectorA, const std::vector<int32_t>& vectorB, int32_t threadingFactor) {
    if (vectorA.size() != vectorB.size()) {
        throw std::invalid_argument("Vectors supplied to sum function aren't of the same length");
    }
    int32_t vectorChunkSize = std::round(vectorA.size() / threadingFactor);
    int32_t chunkStartIndex = 0;
    int32_t chunkEndIndex = 0;
    std::vector<int32_t> vectorChunkStartIndexes;
    std::vector<int32_t> vectorResult{};
    vectorResult.resize(vectorA.size());
    std::vector<std::thread> workerThreads{};
    while (chunkStartIndex + vectorChunkSize < vectorA.size()) {
        vectorChunkStartIndexes.push_back(chunkStartIndex);
        chunkEndIndex = chunkStartIndex + vectorChunkSize;
        workerThreads.push_back(std::thread(sumVectorsWorker,
            std::cref(vectorA),
            std::cref(vectorB),
            std::ref(vectorResult),
            chunkStartIndex,
            chunkEndIndex)
        );
        chunkStartIndex = chunkEndIndex + 1;
    }
    workerThreads.push_back(std::thread(sumVectorsWorker,
        std::cref(vectorA),
        std::cref(vectorB),
        std::ref(vectorResult),
        chunkStartIndex, vectorA.size() - 1)
    );
    for (auto& workerThread : workerThreads) {
        if (workerThread.joinable()) {
            workerThread.join();
        }
    }

    return vectorResult;
}

int32_t main() {
    uint32_t cpuCoreCount = std::thread::hardware_concurrency();
    std::cout << "CPU cores and hyper-threads available: " << cpuCoreCount << std::endl;

    std::vector<std::vector<int32_t>> vectorsToSum{};
    std::vector<int32_t> vectorLengths {1000, 10000, 100000, 1000000, 10000000};
    std::cout << std::format(columnHeaderOutputFormat, "Threads \\ Vector lengts");
    for (const auto vectorLength : vectorLengths) {
        std::cout << "|" << std::format(columnHeaderOutputFormat, vectorLength);
        vectorsToSum.push_back(std::vector<int32_t>{});
        vectorsToSum.back().resize(vectorLength);
        populateVectorWithRandomData(vectorsToSum.back());
    }
    std::cout << std::endl;

    std::vector<int32_t> concurrencyFactors {1, 2, 4, 8, 16};
    for (const auto concurrencyFactor : concurrencyFactors) {
        std::cout << std::format(columnHeaderOutputFormat, "Thread count: " + std::to_string(concurrencyFactor));
        for (const auto vectorToSum : vectorsToSum) {
            const auto processingStartTime{std::chrono::steady_clock::now()};
            auto vectorsSum = sumVectorsWithThreading(vectorToSum, vectorToSum, concurrencyFactor);
            const auto processingFinishTime{std::chrono::steady_clock::now()};
            const std::chrono::duration<double> processingTime{processingFinishTime - processingStartTime};
            const auto processingTimeMilliSec = processingTime.count()*1000;
            const auto processingTimeForOutput = std::format(columnProcesingTimeOutputFormat, processingTimeMilliSec, "ms");
            std::cout << "|" << std::format(columnHeaderOutputFormat, processingTimeForOutput);
        }
        std::cout << std::endl;
    }

}
