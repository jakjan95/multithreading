#include <iostream>
#include <deque>
#include <mutex>
#include <thread>
#include <string>
#include <fstream>
using namespace std;

template <typename T>
class ThreadsafeQueue {
    deque<T> queue_;
    mutable std::mutex m_;
    // TODO: Make it thread-safe :)

public:
    void push(const T & element) {
        std::lock_guard<std::mutex> l(m_);
        queue_.push_front(element);
    }
    T pop() {
        std::lock_guard<std::mutex> l(m_);
        auto top = queue_.back();
        queue_.pop_back();
        return top;
    }
    bool empty() const {
        std::lock_guard<std::mutex> l(m_);
        return queue_.empty();
    }
};

using StringQueue = ThreadsafeQueue<string>;

void provideData(StringQueue & sq) {
    string txt;
    while (getline(cin, txt))
        sq.push(txt);
}

void saveToFile(StringQueue & sq) {
    ofstream file("/tmp/sth.txt");
    while (file) {
        while (sq.empty()) { /* nop */ }
        file << sq.pop() << endl;
    }
}

void produceText(StringQueue & sq, int number) {
    for (int i = 0; i < number; i++)
        sq.push("This is random text number " + std::to_string(i));
}

int main() {
    StringQueue sq;
    thread userDataProducer(provideData, ref(sq));
    thread textProducer(produceText, ref(sq), 10);
    thread consumer(saveToFile, ref(sq));
    userDataProducer.join();
    textProducer.join();
    consumer.join();
    return 0;
}
