/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef V8_OHOS_PERF_PARSE_JITCODE_H_
#define V8_OHOS_PERF_PARSE_JITCODE_H_

#include <elf.h>
#include <fcntl.h>

#include <cstring>
#include <sstream>
#include <unordered_map>
#include <vector>

namespace v8 {
namespace jitparse {

struct JITCodeBlock {
  uint64_t vaddr;
  std::vector<uint8_t> code;
  std::string name;

  JITCodeBlock(uint64_t vaddr, std::vector<uint8_t> code, std::string name)
      : vaddr(vaddr), code(code), name(name) {}
};

class ELFParser {
 public:
  bool Load(const std::vector<JITCodeBlock>& codeBlocks,
            const std::string& inputData);

  bool getInstruction(uint64_t pc, std::string& codeName) const;

 private:
  struct CodeSegment {
    uint64_t start;
    uint64_t end;
    uint64_t file_offset;
    std::string name;
  };
  std::unordered_map<uint64_t, CodeSegment> codeSegments;
};

class ELFGenerator {
 public:
  explicit ELFGenerator(const std::vector<JITCodeBlock>& blocks)
      : codeBlocks(blocks) {}

  bool generateELF(std::string& output);

 private:
  const std::vector<JITCodeBlock>& codeBlocks;
};

class JSVMSymbolExtractor {
 public:
  explicit JSVMSymbolExtractor(uint32_t pid);

  ~JSVMSymbolExtractor();

  bool ReadMem(const uint64_t addr, void* data, size_t size) const;

  void* FindJITSymbolAddress();

  ELFParser* GetParser() const;

  bool GetInstruction(uintptr_t pc, std::string& codeName);

 private:
  static uint32_t process_id_;
  ELFParser* parser = nullptr;
};

}  // namespace jitparse
}  // namespace v8

#endif  // V8_OHOS_PERF_PARSE_JITCODE_H_