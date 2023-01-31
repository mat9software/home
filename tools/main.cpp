#ifdef PROGRAM_CREATE_CPP
#include "create_cpp.h"
#endif

#ifdef PROGRAM_HELLO
#include "hello.h"
#endif

int main(int argc, char** argv) {
  return run_from_args(argc, argv);
}
