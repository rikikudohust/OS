#include <iostream>
#include <thread>
using namespace std;

int x = 0, y = 1;

void T1() {
    while(1) {
        x = y + 1;
        cout << x << " ";
    }
}

void T2() {
    while(1) {
        y = 2;
        y = y * 2;
    }
}


int main() {
    thread t1(&T1);
    thread t2(&T2);
    t1.join();
    t2.join();
    return 0;
}
