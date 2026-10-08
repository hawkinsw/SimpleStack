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

#include "stackstroll.hpp"
#include "llvm/DebugInfo/DIContext.h"
#include "llvm/DebugInfo/DWARF/DWARFCompileUnit.h"
#include "llvm/DebugInfo/DWARF/DWARFContext.h"
#include "llvm/Object/ObjectFile.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/Threading.h"
#include "llvm/Support/WithColor.h"
#include "llvm/Support/raw_ostream.h"
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <llvm/Object/Binary.h>
#include <llvm/Object/Error.h>
#include <llvm/Support/Error.h>
#include <llvm/Support/ErrorOr.h>
#include <memory>
#include <sched.h>
#include <string>
#include <utility>
#include <fstream>
#include <unistd.h>
#include <iostream>

using namespace llvm;
using namespace llvm::object;

static Expected<std::string> comm_name_from_pid(pid_t pid) {
  std::ifstream proc_comm{"/proc/" + std::to_string(pid) + "/cmdline"};

  if (!proc_comm) {
    return createStringError("Could not open the proc file.");
  }

  std::string contents{};
  char next;
  while (proc_comm >> next) {
    contents += next;
  }

  return contents;
}

static void maybe_initialize_llvm() {
  static bool is_initialized{false};
  if (!is_initialized) {
    is_initialized = true;
    llvm::InitializeNativeTarget();
  }
}

static uintptr_t ra_from_rbp(uintptr_t rbp) {
  uintptr_t ra = rbp + sizeof(uintptr_t);

  return *(uintptr_t *)ra;
}

#define GET_RBP() asm volatile("mov %%rbp, %0" : "=r"(rbp));

Expected<std::vector<std::string>> StackStroll::stroll() {

  std::vector<std::string> result{};
  uintptr_t rbp = 0x0;

  GET_RBP();

  while (rbp != 0x0) {
    printf("rbp: %p\n", (void *)rbp);
    uintptr_t ra = ra_from_rbp(rbp);
    printf("ra: %p\n", (void *)ra);
    auto name_or_error{query(ra)};

    if (!name_or_error) {
      break;
    }
    result.push_back(std::move(name_or_error.get()));
    rbp = *(uintptr_t *)rbp;
  }

  return result;
}

StackStroll::StackStroll() : m_dump(false) {
  auto comm_name_or_error = comm_name_from_pid(getpid());
  if (!comm_name_or_error) {
    m_binary = nullptr;
    m_memory = nullptr;
    return;
  }

  initialize_with_filename(comm_name_or_error.get());
}

StackStroll::StackStroll(std::string obj_filename) : m_dump(false) {
  initialize_with_filename(obj_filename);
}

void StackStroll::initialize_with_filename(std::string obj_filename) {
  maybe_initialize_llvm();

  auto memory_buffer_or_error = MemoryBuffer::getFileOrSTDIN(obj_filename);
  if (!memory_buffer_or_error) {
    m_binary = nullptr;
    m_memory = nullptr;
    return;
  }
  m_memory = std::move(memory_buffer_or_error.get());

  auto binary_or_error = object::createBinary(*m_memory);
  if (!binary_or_error) {
    m_binary = nullptr;
    m_memory = nullptr;
    return;
  }

  m_binary = std::move(binary_or_error.get());
}

bool StackStroll::ok() const {
  return m_binary != nullptr;
}

Expected<std::string> StackStroll::query(uintptr_t loc) {
  assert(ok());

  bool Result = true;
  auto RecoverableErrorHandler = [&](Error E) {
    Result = false;
    WithColor::defaultErrorHandler(std::move(E));
  };

  if (auto *Obj = dyn_cast<ObjectFile>(m_binary.get())) {

    std::unique_ptr<DWARFContext> DICtx = DWARFContext::create(
        *Obj, DWARFContext::ProcessDebugRelocations::Process, nullptr, "",
        RecoverableErrorHandler, WithColor::defaultWarningHandler,
        /*ThreadSafe=*/true);

    if (!Result) {
      return createStringError("Could not create the DWARF context");
    }

    auto dies_for_loc = DICtx->getDIEsForAddress(loc);

    if (!dies_for_loc.FunctionDIE) {
      return createStringError("Address is not within function");
    }

    // If there is a compile unit and we are configured to dump information
    // about it, do that.
    if (dies_for_loc.CompileUnit && m_dump) {
      dies_for_loc.CompileUnit->dump(outs(), DIDumpOptions{});
    }

    if (m_dump) {
      // If we are configured to dump information about this function, do
      // that.
      dies_for_loc.FunctionDIE.dump(outs(), 0, DIDumpOptions{});
    }

    std::string name{};
    raw_string_ostream rss{name};
    dies_for_loc.FunctionDIE.getFullName(rss);
    return name;
  }
  return createStringError(
      "Given file is not a valid object file for the native platform.");
}
