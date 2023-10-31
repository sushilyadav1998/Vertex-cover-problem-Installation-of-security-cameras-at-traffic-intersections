#include <iostream>

include namespace std;
template <class T>

void swap(T& a, T &b) {

    std::cout << "Before swapping\na = " << a << "and b = " << b << std::endl;
    T temp = a;
    a = b;
    b = temp; 

    std::cout << "After Swapping\n";
    std::cout << "a = " << a << " and " << b << " = " << b << std::endl;
}

int main(){
    int a = 1;
    int b = 2;
    swap(a, b);  
    return 0;
}

