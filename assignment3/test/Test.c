#include<stdio.h>

void test() {
  int a = 5;
  int b = 10;
  int i = 0;
  int z = 0;
  int y;

  do {
    int k = a + b;         // loop-invariant, domina tutte le uscite
    if (i > 5) {
      z = a + b;           // loop-invariant, ma non domina tutte le uscite
    }

    y = z + i;             // dipende da valore che cambia
    i++;
  } while (i < 10);
}

int main(){
  test();
}