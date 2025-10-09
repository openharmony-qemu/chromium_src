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
#include "parse-jitcode.h"

#include <sys/mman.h>
#include <sys/uio.h>
#include <unistd.h>

#include <fstream>
#include <string>

#include "hilog.h"
#include "perf-jit-common.h"

namespace v8 {
namespace jitparse {
namespace elf {
static uint8_t* shared_mem_base = nullptr;

bool GetHeader(uint32_t processID, uint8_t*& shared_mem) {
  internal::PerfJitHeader* header =
      reinterpret_cast<internal::PerfJitHeader*>(shared_mem);
  if (header->process_id_ != processID) {
    return false;
  }
  shared_mem += header->size_;
  return true;
}

bool GetJitCode(uint32_t processID,
                uint64_t& codeID,
                uint8_t*& shared_mem,
                std::vector<JITCodeBlock>& jitCodeBlocks) {
  // Get load-header
  while (true) {
    internal::PerfJitCodeLoad* codeLoad =
        reinterpret_cast<internal::PerfJitCodeLoad*>(shared_mem);
    if (codeLoad->process_id_ != processID || codeLoad->code_id_ != codeID) {
      HilogPrint(ERROR, "codeLoad check failed!");
      return false;
    }
    codeID++;
    char* code_name =
        reinterpret_cast<char*>(shared_mem + sizeof(internal::PerfJitCodeLoad));
    uint8_t* code_pointer = reinterpret_cast<uint8_t*>(
        shared_mem + sizeof(internal::PerfJitCodeLoad) + strlen(code_name) +
        sizeof(internal::kStringTerminator));

    jitCodeBlocks.emplace_back(
        reinterpret_cast<uint64_t>(codeLoad->code_address_),
        std::vector<uint8_t>(codeLoad->code_size_), code_name);
    std::copy(code_pointer, code_pointer + codeLoad->code_size_,
              jitCodeBlocks.back().code.begin());
    shared_mem += codeLoad->size_;

    char* maybeKJitCodeTerminator = reinterpret_cast<char*>(shared_mem);
    if (strcmp(maybeKJitCodeTerminator, internal::kJitCodeTerminator) == 0) {
      HilogPrint(INFO, "last jit code block");
      return true;
    }
    if (shared_mem - shared_mem_base > SHM_SIZE) {
      HilogPrint(ERROR, "Reach max shared memory size!");
      return false;
    }
  }
}

void GenerateELF(const std::vector<JITCodeBlock>& jitCodeBlocks,
                 std::string& output) {
  ELFGenerator generator(jitCodeBlocks);
  generator.generateELF(output);
}
}  // namespace elf

uint32_t JSVMSymbolExtractor::process_id_ = 0;
JSVMSymbolExtractor::JSVMSymbolExtractor(uint32_t pid) {
  process_id_ = pid;
  void* dfxAddress = FindJITSymbolAddress();
  if (dfxAddress == nullptr) {
    return;
  }
  void* address = mmap(nullptr, SHM_SIZE, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  if (address == MAP_FAILED) {
    HilogPrint(ERROR, "failed mmap symbol memory!");
    return;
  }

  if (!ReadMem(reinterpret_cast<uint64_t>(dfxAddress), address, SHM_SIZE)) {
    HilogPrint(ERROR, "failed read dfxAddress to local address.");
    munmap(address, SHM_SIZE);
    return;
  }

  uint8_t* shared_mem = reinterpret_cast<uint8_t*>(address);
  elf::shared_mem_base = shared_mem;
  uint64_t codeID = 0;
  std::vector<JITCodeBlock> jitCodeBlocks;
  if (!elf::GetHeader(process_id_, shared_mem)) {
    munmap(address, SHM_SIZE);
    return;
  }

  if (!elf::GetJitCode(process_id_, codeID, shared_mem, jitCodeBlocks)) {
    HilogPrint(ERROR, "GetJitCode error!");
    munmap(address, SHM_SIZE);
    return;
  }

  munmap(address, SHM_SIZE);
  std::string elfFile;
  elf::GenerateELF(jitCodeBlocks, elfFile);
  parser = new ELFParser();
  if (!parser->Load(jitCodeBlocks, elfFile)) {
    delete parser;
    parser = nullptr;
  }
}

JSVMSymbolExtractor::~JSVMSymbolExtractor() {
  if (parser != nullptr) {
    delete parser;
  }
}

bool JSVMSymbolExtractor::ReadMem(const uint64_t addr,
                                  void* data,
                                  size_t size) const {
  uint64_t currentAddr = addr;
  if (__builtin_add_overflow(currentAddr, size, &currentAddr)) {
    return false;
  }
  struct iovec remoteIov = {
      .iov_base = reinterpret_cast<void*>(addr),
      .iov_len = size,
  };
  struct iovec dataIov = {
      .iov_base = static_cast<uint8_t*>(data),
      .iov_len = size,
  };
  ssize_t readCount =
      process_vm_readv(process_id_, &dataIov, 1, &remoteIov, 1, 0);
  return static_cast<size_t>(readCount) == size;
}

void* JSVMSymbolExtractor::FindJITSymbolAddress() {
  std::string maps_path = "/proc/" + std::to_string(process_id_) + "/maps";
  std::ifstream maps_file(maps_path);
  if (!maps_file.is_open()) {
    HilogPrint(ERROR, "can not open map file!");
    return nullptr;
  }
  std::string line;
  std::string memTagName = internal::JitSymbolMemTagName(process_id_);
  while (std::getline(maps_file, line)) {
    if (line.find(memTagName) != std::string::npos) {
      // parse start address
      size_t dash_pos = line.find('-');
      if (dash_pos == std::string::npos) {
        continue;
      }
      std::string start_addr_str = line.substr(0, dash_pos);
      // convert address string to uint64_t
      constexpr int kNumberBase = 16;
      return reinterpret_cast<void*>(
          std::stoull(start_addr_str, nullptr, kNumberBase));
    }
  }
  HilogPrint(ERROR, "cant find JSVM JIT symbol");
  return nullptr;
}

ELFParser* JSVMSymbolExtractor::GetParser() const {
  return parser;
}

bool JSVMSymbolExtractor::GetInstruction(uintptr_t pc, std::string& codeName) {
  return parser->getInstruction(pc, codeName);
}

bool ELFGenerator::generateELF(std::string& output) {
  std::ostringstream out(std::ios::binary);

  const size_t ehdr_size = sizeof(Elf64_Ehdr);
  const size_t phdr_size = sizeof(Elf64_Phdr) * codeBlocks.size();
  size_t file_offset = ehdr_size + phdr_size;

  Elf64_Ehdr ehdr = {};
  ehdr.e_ident[0] = ELFMAG0;      // 0x7f ELF magic number
  ehdr.e_ident[1] = ELFMAG1;      // 'E'
  ehdr.e_ident[2] = ELFMAG2;      // 'L'
  ehdr.e_ident[3] = ELFMAG3;      // 'F'
  ehdr.e_ident[4] = ELFCLASS64;   // 64-bit architecture
  ehdr.e_ident[5] = ELFDATA2LSB;  // Little-endian
  ehdr.e_ident[6] = EI_VERSION;   // ELF version
  ehdr.e_type = ET_EXEC;
  ehdr.e_machine = EM_ARM;
  ehdr.e_version = EV_CURRENT;
  ehdr.e_entry = codeBlocks.empty() ? 0 : codeBlocks[0].vaddr;
  ehdr.e_phoff = ehdr_size;
  ehdr.e_shoff = 0;
  ehdr.e_ehsize = ehdr_size;
  ehdr.e_phentsize = sizeof(Elf64_Phdr);
  ehdr.e_phnum = codeBlocks.size();
  ehdr.e_shentsize = 0;
  ehdr.e_shnum = 0;
  out.write(reinterpret_cast<char*>(&ehdr), ehdr_size);
  for (const auto& block : codeBlocks) {
    Elf64_Phdr phdr = {};
    phdr.p_type = PT_LOAD;
    phdr.p_flags = PF_X | PF_R;
    phdr.p_offset = file_offset;
    phdr.p_vaddr = block.vaddr;
    phdr.p_paddr = block.vaddr;
    phdr.p_filesz = block.code.size();
    phdr.p_memsz = block.code.size();
    phdr.p_align = 0x1000;
    out.write(reinterpret_cast<char*>(&phdr), sizeof(phdr));
    file_offset += block.code.size();
  }
  for (const auto& block : codeBlocks) {
    out.write(reinterpret_cast<const char*>(block.code.data()),
              block.code.size());
  }
  output = out.str();
  return true;
}

bool ELFParser::Load(const std::vector<JITCodeBlock>& codeBlocks,
                     const std::string& inputData) {
  const Elf64_Ehdr* ehdr =
      reinterpret_cast<const Elf64_Ehdr*>(inputData.data());
  const Elf64_Phdr* phdr =
      reinterpret_cast<const Elf64_Phdr*>(inputData.data() + ehdr->e_phoff);

  if (ehdr->e_phnum != codeBlocks.size()) {
    return false;
  }

  for (size_t i = 0; i < ehdr->e_phnum; ++i) {
    if (phdr[i].p_type == PT_LOAD && (phdr[i].p_flags & PF_X)) {
      codeSegments[phdr[i].p_vaddr] = {.start = phdr[i].p_vaddr,
                                       .end = phdr[i].p_vaddr + phdr[i].p_memsz,
                                       .file_offset = phdr[i].p_offset,
                                       .name = codeBlocks[i].name};
    }
  }
  return true;
}

bool ELFParser::getInstruction(uint64_t pc, std::string& codeName) const {
  for (const auto& seg : codeSegments) {
    if (pc >= seg.second.start && pc < seg.second.end) {
      codeName = seg.second.name;
      return true;
    }
  }
  return false;
}

}  // namespace jitparse
}  // namespace v8
