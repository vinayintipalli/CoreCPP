#include "math_utils.hpp"
#include <cstdio>

int main() {
    int result = square(5);   // calls a function whose BODY isn't in this file
    printf("%d\n", result);
    return 0;
}