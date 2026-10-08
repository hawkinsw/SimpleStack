#include <dwarf.hpp>
#include <iostream>

int two(int p) {
  StackStroll ss{};

  auto walk_or_error = ss.stroll();
  if (walk_or_error) {
    for (const auto &f : *walk_or_error) {
      std::cout << "Name: " << f << "\n";
    }
  }

  return 2;
}

int one(int p) {
  p = two(p);
  return p;
}

int main() {
  int x = 1;
  int v = one(x);
  return v;
}
