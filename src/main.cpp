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
    Car() : year(2000), name("null") {cout << "car ctor\n";};
    ~Car() {
        cout << "car destructed\n";
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

    Car* car = new(a.allocate(sizeof(Car), alignof(Car))) Car();

    car->~Car(); //delete car myself
    a.reset(); // does not delete existing cars

    //trying to allocate new cars to place with old cars
    Car* car2 = new(a.allocate(sizeof(Car), alignof(Car))) Car();

    cout << "cars at: ";
    cout << car << ' ' << car2 << '\n'; // car2 is lower than car1 on 40 bytes (one car)
    //first 40 bytes were overwritten, other bytes - abandoned

    car2->~Car();
}
void test_release() {
    const int size = 1024;
    arena a(size);
    void* ptr = a.allocate(sizeof(Car), alignof(Car));
    Car* car = (Car*)ptr;
    cout << "ptr: " << car << '\n';
    new (car) Car();
    car->~Car(); // delete object before freeing memory
    a.release(); // now car is a dangling pointer
}

void nestedArenaProblem() {
    size_t parent_size = 1024;
    size_t child_size = 256;
    arena parent(parent_size);
    char* child_buf = (char*) parent.allocate(child_size, alignof(arena));

    char* parent_arr = parent.getBegin() + child_size;
    for (int i = 0; i < 100; ++i) {
        parent_arr[i] = 5;
    }

    arena child(child_buf, parent_size); //more than parent allocated

    int* corrupting_arr = (int*) child.allocate(300, alignof(int));

    bool good = true;
    for (int i = 0; i < 100; ++i) {
        good &= parent_arr[i] == 5;
    }
    cout << "parent arr before allocation is " << (good ? "" : "not ") << "good\n";

    for (int i = 0; i < 75; ++i) { // 300 bytes = 75 int
        corrupting_arr[i] = 3;
    }

    for (int i = 0; i < 100; ++i) {
        if (parent_arr[i] != 5) {
            cout << "corruption at " << i << "\n";
        }
    }
}

void testMoveCtor() {
    size_t size = 80;
    arena a(size);

    int* buf = (int*) a.allocate(20 * sizeof(int), alignof(int));

    cout << "arena A before moving: \n";

    cout << "size " << a.size() << ", ";
    cout << "used " << a.used() << ", ";
    cout << "isOwner " << a.is_owner() << "\n";

    arena b = std::move(a); 

    cout << "arena A after moving: \n";

    cout << "size " << a.size() << ", ";
    cout << "used " << a.used() << ", ";
    cout << "isOwner " << a.is_owner() << "\n";

    cout << "arena B after moving: \n";

    cout << "size " << b.size() << ", ";
    cout << "used " << b.used() << ", ";
    cout << "isOwner " << b.is_owner() << "\n";
}

void testMoveOperator() {
    size_t size = 80;
    arena a(size);

    int* buf = (int*) a.allocate(20 * sizeof(int), alignof(int));

    arena b(size);

    cout << "arena A before moving: \n";

    cout << "size " << a.size() << ", ";
    cout << "used " << a.used() << ", ";
    cout << "isOwner " << a.is_owner() << "\n";

    b = std::move(a); 

    cout << "arena A after moving: \n";

    cout << "size " << a.size() << ", ";
    cout << "used " << a.used() << ", ";
    cout << "isOwner " << a.is_owner() << "\n";

    cout << "arena B after moving: \n";

    cout << "size " << b.size() << ", ";
    cout << "used " << b.used() << ", ";
    cout << "isOwner " << b.is_owner() << "\n";
    //arena does not own memory
}


int main(){
    testMoveOperator();
    //how to automatically desturct objects?
    /*
    todo
        delete = operator
        delete copying
        add move semantics
        testing
    */
    return 0;
}