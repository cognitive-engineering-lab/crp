#include <iostream>
#include <string>

class Device {
public:
    virtual void powerOn() = 0;
    virtual void powerOff() = 0;

    virtual void resetDevice() {
        std::cout << "Resetting device..." << std::endl;
        powerOff();
        powerOn();
    }

    virtual ~Device() {}
};

class Printer : public Device {
    bool powered = false;
public:
    void powerOn() override {
        this.powered = true;
        std::cout << "Printer is powered on." << std::endl;
    }

    void powerOff() override {
        this.powered = false;
        std::cout << "Printer is powered off." << std::endl;
    }
};

int main() {
    Printer myPrinter;
    myPrinter.resetDevice();
}
