#include <future>
#include <iostream>
#include <random>
#include <vector>

void findVectorMinElementWithPromise(const std::vector<int32_t>& vectorToSearch,
    const int32_t& startIndex,
    std::promise<int32_t> searchPromise) {
    int minElementIndex = startIndex;
    for (int32_t elementIndex = startIndex; elementIndex < vectorToSearch.size(); elementIndex++) {
        if (vectorToSearch[elementIndex] < vectorToSearch[minElementIndex]) {
            minElementIndex = elementIndex;
        }
    }
    searchPromise.set_value(minElementIndex);
}

void sortVector(std::vector<int32_t>& vectorToSort) {

    for (int32_t elementIndex = 0; elementIndex < vectorToSort.size(); elementIndex++) {
        std::promise<int32_t> searchPromise;
        std::future<int32_t> searchFuture;
        searchFuture = searchPromise.get_future();
        auto asyncRun = std::async(findVectorMinElementWithPromise,
            std::cref(vectorToSort),
            std::cref(elementIndex),
            std::move(searchPromise));
        searchFuture.wait();
        auto minElementIndex = searchFuture.get();
        if (minElementIndex > elementIndex) {
            std::swap(vectorToSort[elementIndex], vectorToSort[minElementIndex]);
        }
    }
}

int32_t main() {
    std::vector<int32_t> vectorToSort{};
    vectorToSort.resize(30);
    srand(time(NULL));
    std::cout << "Input vector: ";
    for (auto& vectorElement : vectorToSort) {
        vectorElement = rand() % 1000;
        std::cout << vectorElement << " ";
    }
    sortVector(vectorToSort);
    std::cout << std::endl;
    std::cout << "Sorted vector: ";
    for (auto& vectorElement : vectorToSort) {
        std::cout << vectorElement << " ";
    }

    return EXIT_SUCCESS;
}