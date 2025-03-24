#include <stdio.h>

int algebraic_identity(int x) {
    int a = x + 0;  // Deve diventare solo x
    int b = x * 1;  // Deve diventare solo x
    return a + b;
}

int strength_reduction(int x) {
    int a = x * 8;   // Dovrebbe diventare x << 3
    int b = x * 15;  // Dovrebbe diventare (x << 4) - x
    int c = x * 17;  // Dovrebbe diventare (x << 4) + x
    int d = 4 * x;   // Dovrebbe diventare x << 2
    int e = 17 * x;  // Dovrebbe diventare (x << 4) + x
    return a + b + c + d + e;
}

int strength_reduction2(int x) {
    int c = x / 8;  // Deve diventare x >> 3
    int d = x / 15; // Deve diventare (x >> 4) - x
    int e = x / 17; // Deve diventare (x >> 4) + x
    return c + d + e;
}

int multi_instruction(int b, int e) {
    int a = b + 1;
    int c = a - 1; 
    int d = e - 1;
    int f = d + 1;
    return c + f; // Dovrebbe ritornare direttamente b ed e
}

int main() {
    /* Algebraic Identity */
    int result = algebraic_identity(10);
    printf("Risultato: %d\n", result);

    /* Strength Reduction MUL */
    int x = 10;
    result = strength_reduction(x);
    printf("Risultato per x = %d: %d\n", x, result);

    /* Strength Reduction DIV */
    result = strength_reduction2(x);
    printf("Risultato per x = %d: %d\n", x, result);

    /* Multi Instruction */
    result = multi_instruction(x, x);
    printf("Risultato per x = %d: %d\n", x, result);

    return 0;
}