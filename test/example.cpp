// Stack Stroll, Copyright 2026, Will Hawkins
//
// This file is part of Stack Stroll.

// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#include <stackstroll.hpp>
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
