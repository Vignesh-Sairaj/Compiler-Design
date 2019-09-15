#include <stdio.h>
#include <stdlib.h>

int add(int a, int b);

int main (void) {

    /*

    This is a comment to ignore

    */

    char str1[] = "Ignore this \"string\" with escape sequences\n";
    char null = '\0';

    int a10 = 0;
    int b = 1;

    printf("a+b = %d\n", add(a, b));
    return 0;
}


int add(int a, int b) {
    int c = a + b;
    return c;
}
