// use store() /  load()
#include <vector>
#include <atomic>
#include <mutex>
#include <thread>
#include <iostream>
using namespace std;


vector<int> generateContainer() {
    vector<int> input = {2, 4, 6, 8, 10, 1, 3, 5, 7, 9};
    vector<int> output;
    vector<thread> threads;

    std::mutex m;
    auto addToOutput = [&](int index) {
        std::lock_guard<std::mutex> lock(m);
        output.push_back(input[index]); };

    for (auto i = 0u; i < input.size(); i++) {
        threads.emplace_back(addToOutput, i);
    }
    for (auto && t : threads) {
        t.join();
    }
    return output;
}

vector<int> generateOtherContainer()
{
    std::atomic<int> start = 5;
    std::atomic<bool> add = true;
    vector<int> output;
    vector<thread> threads;
    std::mutex m;

    auto addToOutput = [&](int index) {
        std::lock_guard<std::mutex> lock(m);
        if (add.load()) {
            output.push_back(start += index);
        } else {
            output.push_back(start -= index);
        }
        add = !add;
    };

    for (int i = 0; i < 10; i++) {
        threads.emplace_back(addToOutput, i);
    }

    for (auto&& t : threads) {
        t.join();
    }
    return output;
}

void powerContainer(vector<int>& input) {
    vector<thread> threads;

    for (auto i = 0u; i < input.size(); i++) {
        threads.emplace_back([&]{ input[i]*=input[i]; });
    }
    for (auto && t : threads) {
        t.join();
    }
}

void printContainer(const vector<int>& c) {
    for (const auto & value : c){
        cout << value << " ";
    }
    cout << endl;
}

int main() {
    auto container1 = generateContainer();
    printContainer(container1);
    powerContainer(container1);
    printContainer(container1);

    auto container2 = generateOtherContainer();
    printContainer(container2);
    powerContainer(container2);
    printContainer(container2);
    return 0;
}

