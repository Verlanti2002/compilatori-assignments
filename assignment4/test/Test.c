#include <stdio.h>
#include <stdbool.h>

const int N = 10;

void test_all_properties_ok() {
    int a[N], b[N], c[N], d[N];

    for (int i = 0; i < N; ++i) {
        c[i] = a[i] + b[i];
    }

    for (int i = 0; i < N; ++i) {
        d[i] = c[i] * 2;
    }
}

void test_not_adjacent() {
    int a[N], b[N], c[N], d[N];

    for (int i = 0; i < N; ++i) {
        c[i] = a[i] + b[i];
    }

    printf("Separazione tra i loop\n");  // rompe l'adiacenza

    for (int i = 0; i < N; ++i) {
        d[i] = c[i] * 2;
    }
}

void test_control_flow_not_equivalent() {
    int a[N], b[N];
    bool flag = true;

    if(flag) {
        for(int i = 0; i < N; i++) {
            a[i] = i;
        }
    }
    
    for(int i = 0; i < N; i++) {
        b[i] = a[i] * 2;
    }
}

void test_different_trip_count() {
    int a[N], b[N], c[N], d[N];

    for (int i = 0; i < N; ++i) {  // trip count N
        c[i] = a[i] + b[i];
    }

    for (int i = 1; i < N; ++i) {  // trip count N - 10
        d[i] = c[i] * 2;
    }
}

void test_negative_dependence() {
    int a[N], c[N];

    for (int i = 0; i < N; ++i) {
        c[i] = a[i];
    }

    for (int i = 0; i < N; ++i) {
        c[i] = c[i+3] + 1;  // scrive in i+3: dipendenza negativa
    }
}