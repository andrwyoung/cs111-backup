#include <signal.h>
#include <stdlib.h>
int main(int argc, char *argv[]) {
  raise(atoi(argv[1]));
  return 0;
}
