
#include "levenshtein.h"
#include<iostream>

int main( int, char ** ) {
    std::string a, b;
    std::cout << "Enter first string: ";
    std::cin >> a;
    std::cout << "Enter second string: ";
    std::cin >> b;

    std::vector<char> av(a.begin(), a.end()), bv(b.begin(), b.end());
    int d = levenshtein( av, bv );
    std::cout << "Distance is " << d << std::endl;
    return 0;
}
