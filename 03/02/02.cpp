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

    for (auto containerIterator = startIterator; containerIterator != endIterator; containerIterator++) {
        auto asyncRun = std::async(std::launch::async, handlerFunc, std::ref(*containerIterator));
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