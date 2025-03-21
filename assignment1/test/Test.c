#include <stdio.h>

int algebraic_identity(int x) {
    int a = x + 0;  // Deve diventare solo 'x'
    int b = x * 1;  // Deve diventare solo 'x'
    return a + b;
}

int main() {
    int result = algebraic_identity(10);
    printf("Risultato: %d\n", result);
    return 0;
}