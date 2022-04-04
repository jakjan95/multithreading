#include <mutex>
#include <vector>
#include <iostream>
#include <thread>
#include <shared_mutex>

using namespace std;

// Use shared and / or regular locks
// Use the appropriate lock managers
// Compile with TSan

std::shared_mutex sharedMutexNumbers;
std::mutex countMutex;

vector<int> numbers = {};

int getNextValue() {
    static int i = 0;
    return i+=10;
}

void read(int index)
{
    for (int i = 0; i < 10; i++) {
        std::shared_lock<std::shared_mutex> lock(sharedMutexNumbers);
        int value = numbers[index];
        lock.unlock();
        std::lock_guard<std::mutex> countLock(countMutex);
        cout << value << " ";
    }
}

void write()
{
    for (int i = 0; i < 3; i++) {
        std::lock_guard<std::shared_mutex> lock(sharedMutexNumbers);
        int newValue = getNextValue();
        numbers.emplace_back(newValue);
    }
}

int main() {
    vector<thread> writers;
    for(int i = 0; i < 10; i++)
        writers.emplace_back(write);

    cout << "Writers produced: ";

    std::shared_lock<std::shared_mutex> lock(sharedMutexNumbers);
    for (const auto& n : numbers) {
        cout << n << " ";
    }
    cout << endl;
    lock.unlock();

    cout << "Readers consumed: ";
    vector<thread> readers;
    for(int i = 0; i < 10; i++)
        readers.emplace_back(read, i);

    for(auto && writer : writers)
        writer.join();
    for(auto && reader : readers)
        reader.join();

    cout << endl;
    return 0;
}
