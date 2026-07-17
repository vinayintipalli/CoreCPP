#include <iostream>

class Base {
    public:
        Base() {
            std::cout << "Base constructor \n";
        }
        virtual ~Base() {
            std::cout << "Base destructor \n";
        }

    static void* operator new(size_t size) {
        auto ptr = std::malloc(size);
        std::cout << "Base new size " << size << "  and addr " << ptr << "\n";
        return std::malloc(size);
    }
    
    static void operator delete(void* ptr) {
        std::cout << "Base delete ptr " << ptr << "\n";
        std::free(ptr);
    }
};

class Derived : public Base {
    public:
        Derived() {
            std::cout << "Derived constructor \n";
        }
        ~Derived() {
            std::cout << "Derived destructor \n";
        }

    static void* operator new(size_t size) {
        auto ptr = std::malloc(size);
        std::cout << "Derived new size " << size << "  and addr " << ptr << "\n";
        return std::malloc(size);
    }
    
    static void operator delete(void* ptr) {
        std::cout << "Derived delete ptr " << ptr << "\n";
        std::free(ptr);
    }
};

int main() {
    // Base* b = new Base();

    // delete b;

    Base* d = new Derived();

    delete d;
}