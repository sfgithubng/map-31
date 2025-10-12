#include <algorithm>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

constexpr std::string clearScreenStringSeq = "\x1B[2J\x1B[H";
constexpr int32_t DefaultInt32MemberVar{0};
constexpr std::string DefaultStringMemberVar{"0"};

const std::vector<int32_t> validMenuOptions{
    1,
    2,
    3,
    10
};

class Data {
private:
    int32_t int32MemberVar;
    std::string stringMemberVar;
public:
    std::mutex editionMutex{};

    Data() : Data(DefaultInt32MemberVar, DefaultStringMemberVar) {};

    Data (int32_t constrInt32MemberVar, std::string ConstrStringMemberVar) :
        int32MemberVar(constrInt32MemberVar), stringMemberVar(ConstrStringMemberVar) {
    }

    ~Data() = default;

    Data(const Data& otherData) : Data(otherData.int32MemberVar, otherData.stringMemberVar) {};

    Data(Data&& otherData) = delete;

    Data& operator=(const Data& otherData) {
        if (this != &otherData) {
            int32MemberVar = otherData.int32MemberVar;
            stringMemberVar = otherData.stringMemberVar;
        }
        return *this;
    }

    Data& operator=(Data&& otherData) = delete;

    std::string fetchDataMembers() {
        std::string formattedDataMembers = "int32MemberVar = " +
            std::to_string(int32MemberVar) +
            ", stringMemberVar = " +
            stringMemberVar;

        return formattedDataMembers;
    }

};

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

int32_t getMenuOption() {
    std::cout << "Please choose the implementation to swap values:" << std::endl;
    std::cout << "1. std::lock." << std::endl;
    std::cout << "2. std::scoped_lock." << std::endl;
    std::cout << "3. std::unique_lock." << std::endl;
    std::cout << "10. Exit program." << std::endl;
    int32_t userMainMenuOption{100};
    while (std::find(validMenuOptions.begin(), validMenuOptions.end(), userMainMenuOption) == validMenuOptions.end()) {
        userMainMenuOption = getPositiveInputInt32Var("Choose a menu option to proceed: ");
}
    return userMainMenuOption;
}

void swapStdLock(Data& dataInstance1, Data& dataInstance2) {
    std::lock(dataInstance1.editionMutex, dataInstance2.editionMutex);
    std::lock_guard<std::mutex> dataInstance1Lock(dataInstance1.editionMutex, std::adopt_lock);
    std::lock_guard<std::mutex> dataInstance2Lock(dataInstance2.editionMutex, std::adopt_lock);
    auto tmpDataInstance1 = Data(dataInstance1);
    dataInstance1 = dataInstance2;
    dataInstance2 = tmpDataInstance1;
}

void swapScopedLock(Data& dataInstance1, Data& dataInstance2) {
    std::scoped_lock dataInstancesLock(dataInstance1.editionMutex, dataInstance2.editionMutex);
    auto tmpDataInstance1 = Data(dataInstance1);
    dataInstance1 = dataInstance2;
    dataInstance2 = tmpDataInstance1;
}

void swapUniqueLock(Data& dataInstance1, Data& dataInstance2) {
    std::unique_lock dataInstance1Lock(dataInstance1.editionMutex, std::defer_lock);
    std::unique_lock dataInstance2Lock(dataInstance2.editionMutex, std::defer_lock);
    std::lock(dataInstance1Lock, dataInstance2Lock);
    auto tmpDataInstance1 = Data(dataInstance1);
    dataInstance1 = dataInstance2;
    dataInstance2 = tmpDataInstance1;
}

int32_t main() {
    int32_t userMenuOption{0};
    while(userMenuOption != 10) {
        auto dataInstance1 = Data(123, "123");
        auto dataInstance2 = Data(456, "456");
        std::cout << clearScreenStringSeq;
        userMenuOption = getMenuOption();
        std::cout << "Data instance 1 values before swap: " << dataInstance1.fetchDataMembers() << std::endl;
        std::cout << "Data instance 2 values before swap: " << dataInstance2.fetchDataMembers() << std::endl;
        auto swapThread = std::thread();
        switch (userMenuOption) {
            case 1:
                swapThread = std::thread(swapStdLock, std::ref(dataInstance1), std::ref(dataInstance2));
                break;
            case 2:
                swapThread = std::thread(swapScopedLock,std::ref(dataInstance1), std::ref(dataInstance2));
                break;
            case 3:
                swapThread = std::thread(swapUniqueLock, std::ref(dataInstance1), std::ref(dataInstance2));
                break;
        }
        swapThread.join();
        std::cout << "Data instance 1 values after swap: " << dataInstance1.fetchDataMembers() << std::endl;
        std::cout << "Data instance 2 values after swap: " << dataInstance2.fetchDataMembers() << std::endl;
        std::cout << "Press any key to repeat" << std::endl;
        std::string dummyString{};
        std::getline(std::cin, dummyString);
    }

    return EXIT_SUCCESS;
}