#include <iostream>
#include <string>
#include "../include/arena.cpp"
using namespace std;

void test_ctor_one_par(){
    const long long good_size = 1024;
    arena a(good_size);
    cout << "\n";
    const long long bad_size = 100000000000;
    try{
        arena b(bad_size);
    } catch (std::exception& e){
        cout << e.what() << '\n';
    }
}

void test_ctor_two_par(){
    const int good_size = 1 << 10;
    char buf[good_size];
    arena a(buf, good_size);
    try{
        // const long long bad_size = 1e10;  works
        const long long bad_size = 1e11; //  fails
        // char buf[bad_size];             //stack allocation
        {
            char* buf2 = (char*)malloc(bad_size); // on the heap
            arena b(buf2, bad_size);
            free(buf2);
        }
    } catch (std::exception& e){
        cout << e.what() << '\n';
    }
}

struct Car{
    int year;
    string name;
    Car() : year(2000), name("null") {};
    ~Car() {
        cout << "car dtor\n";
    }
};

void car_test(){
    arena a(1024); // empty space
    cout << sizeof(Car) << ' ' <<  alignof(Car) << '\n';
    void* ptr = a.allocate(sizeof(Car), alignof(Car)); // find suitable spot in owned memory
    cout << "car at address " << ptr << '\n';
    Car* car = (Car*)ptr;
    new (car) Car(); // car is ptr to space inside arena, create Car at that address
    car->~Car(); // segfault if i delete the object, because Arena owns it, not me
}

void test_reset() {
    const int size = 1024;
    arena a(size);
    void* ptr = a.allocate(sizeof(Car), alignof(Car));
    cout << "used : " << a.used() << '\n';
    Car* car = (Car*)ptr;
    new (car) Car();
    a.reset();
    cout << "used after reset : " << a.used() << '\n';
}
void test_release() {
    const int size = 1024;
    arena a(size);
    void* ptr = a.allocate(sizeof(Car), alignof(Car));
    cout << "used : " << a.used() << '\n';
    Car* car = (Car*)ptr;
    cout << "ptr: " << car << '\n';
    new (car) Car();
    car->~Car(); // delete object before freeing memory
    a.release(); // now car is a dangling pointer
}

int main(){

    /*
    todo
        delete = operator
        delete copying
        add move semantics
        testing
    */
    return 0;
}