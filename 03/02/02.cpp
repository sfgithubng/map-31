#include <functional>
#include <future>
#include <iostream>
#include <vector>

void handlerFuncImplementation(int32_t& containerElement) {
    containerElement += 10;
};

template<class ForwardIt, class UnaryFunc>
void async_for_each(ForwardIt startIterator,
    ForwardIt endIterator,
    UnaryFunc handlerFunc) {

    auto containerSize = std::distance(endIterator, startIterator);

    if (containerSize < 10) {
        for (auto containerIterator = startIterator; containerIterator != endIterator; ++containerIterator) {
            auto asyncRun = std::async(std::launch::async, handlerFunc, std::ref(*containerIterator));
        }
    } else {
        auto middleIterator = startIterator;
        std::advance(middleIterator, containerSize / 2);
        auto asyncRunResult = std::async(async_for_each<ForwardIt, UnaryFunc>, startIterator, middleIterator, handlerFunc);
        auto asyncRunFirst = std::async(async_for_each<ForwardIt, UnaryFunc>, startIterator, middleIterator, handlerFunc);
        auto asyncRunSecond = std::async(async_for_each<ForwardIt, UnaryFunc>, middleIterator + 1, endIterator, handlerFunc);
    }
}

int32_t main() {
    std::vector<int32_t> vectorToProcess{};
    vectorToProcess.resize(30);
    srand(time(NULL));
    std::cout << "Input vector: ";
    for (auto& vectorElement : vectorToProcess) {
        vectorElement = rand() % 1000;
        std::cout << vectorElement << " ";
    }
    async_for_each(vectorToProcess.begin(), vectorToProcess.end(), handlerFuncImplementation);
    std::cout << std::endl;
    std::cout << "Modified vector: ";
    for (auto& vectorElement : vectorToProcess) {
        std::cout << vectorElement << " ";
    }

    return EXIT_SUCCESS;


}