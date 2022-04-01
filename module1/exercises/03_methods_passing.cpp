#include <thread>
#include <iostream>
#include <string>

class Car {
    int production_year;
    std::string model_name;
public:
    void setData(int year, const std::string & model) {
        production_year = year;
        model_name = model;
    }
    void print() {
        std::cout << model_name << " " << production_year << std::endl; 
    }
};

int main() {
    Car toyota;
    // set year to 2015, model to "Corolla" in a thread
    std::thread t(&Car::setData, &toyota, 2015, "Corolla");
    t.join();
    toyota.print();
    return 0;
}

