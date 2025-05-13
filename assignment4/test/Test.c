#include <stdio.h>

const int N = 100;

void loop_fusion(int *a, int *b, int *c) {
    // Primo ciclo: inizializza a[]
    for (int i = 0; i < N; ++i) {
        a[i] = b[i] + 1;
    }

    // Secondo ciclo: inizializza c[] usando a[]
    for (int i = 0; i < N; ++i) {
        c[i] = a[i] * 2;
    }
}

int main() {
    int a[N], b[N], c[N];

    // Inizializza b[]
    for (int i = 0; i < N; ++i) {
        b[i] = i;
    }

    loop_fusion(a, b, c);
    
    return 0;
}