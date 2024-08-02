
#include <iostream>
using namespace std;

class Animal {
public:
    virtual void makeSound() {  // Virtual function
        cout << "Animal makes a sound" << endl;
    }
};

class Dog : public Animal {
public:
    void makeSound() override {  // Overriding the virtual function
        cout << "Dog barks" << endl;
    }
};

class Cat : public Animal {
public:
    void makeSound() override {  // Overriding the virtual function
        cout << "Cat meows" << endl;
    }
};

int main() {
    Animal* a1 = new Dog();  // Base class pointer pointing to derived class object
    Animal* a2 = new Cat();  // Base class pointer pointing to derived class object

    a1->makeSound();  // Polymorphic call
    a2->makeSound();  // Polymorphic call

    delete a1;
    delete a2;

    return 0;
}
