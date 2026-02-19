#include <iostream>

using namespace std;

int main()
{
    int b = 10;
    int a = 5;
    const int* t1 = &a;
    int const* t2 = &a;

    cout << "address of t1 " << t1 << " New address of t1" << endl;
    t1 = &b;
    cout << t1 << endl;

    cout << "new value of t1" << endl;
    *t2 = 10;

    char a = 'X';
char b = 'Y';

char const* p = &a;  // p now holds the ADDRESS of a (e.g., 0x100)

// 1. Trying to change the DATA (The 'X')
*p = 'Z';


}