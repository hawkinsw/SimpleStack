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