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
#ifndef STACKSTROLL_HPP
#define STACKSTROLL_HPP

#include <cstdint>
#include <memory>
#include <string>

#include <llvm/ADT/StringRef.h>
#include <llvm/Object/Binary.h>
#include <llvm/Support/Error.h>
#include <llvm/Support/MemoryBufferRef.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;

class StackStroll {
public:
  StackStroll();
  StackStroll(std::string obj_filename);

  bool ok() const;

  Expected<std::vector<std::string>> stroll();

private:
  void initialize_with_filename(std::string obj_filename);
  Expected<std::string> query(uintptr_t loc);
  std::unique_ptr<MemoryBuffer> m_memory;
  std::unique_ptr<object::Binary> m_binary;
  bool m_dump{false};
};

#endif