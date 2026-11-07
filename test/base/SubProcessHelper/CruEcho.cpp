#include <iostream>

int main(int argc, char* argv[]) {
  for (int i = 1; i < argc - 1; ++i) {
    std::cout << argv[i] << '\n';
  }
  std::cout << argv[argc - 1] << std::endl;
  return 0;
}
