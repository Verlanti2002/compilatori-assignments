#include<stdio.h>

void test() {
  int a = 5;
  int b = 10;
  int y;
  int i = 0;

  do {
    int z = a + b;    // <- loop-invariant, DOMINA tutte le uscite
    y = z + i;
    printf("%d\n", y);
    i++;
  } while (i < 10);
}

int main(){
  test();
}