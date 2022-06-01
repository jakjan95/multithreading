#include <chrono>
#include <mutex>
#include <vector>
#include <thread>
#include <iostream>
#include <sstream> 
#include <random>


using namespace std;

class Prize {
    string winnerId{"unknown"};
    std::random_device dev{};
    std::mt19937 rng{dev()};  
    std::uniform_int_distribution<int> dist{10, 50};
    std::once_flag flag;
    
public:
    void setWinner() {
        auto id = this_thread::get_id();
        auto sleepDuration = dist(rng);
        stringstream msg;
        msg << "Called " << __FUNCTION__ << "(" << id << "). Chasing time: " << sleepDuration << "ms\n";
        cout << msg.str(); // single operation on stream is atomic

        std::this_thread::sleep_for(std::chrono::milliseconds(sleepDuration));
        
        std::call_once(
            flag, [&id, this]() {
                stringstream ssId;
                ssId << id;
                winnerId = ssId.str();
            });
    }
      
    void showWinner() {
        cout << ("And the winner is... " + winnerId + '\n');
        // this is single stream operation, which is atomic - no need for mutex
    }
};

int main() {
    Prize millionBucks;
    vector<thread> contestants;

    for (int i = 0; i < 10; i++)
        contestants.emplace_back(&Prize::setWinner, &millionBucks);

    for (auto && c: contestants)
        c.join();

    millionBucks.showWinner();
    return 0;
}
