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

#ifdef V8_ENABLE_OHOS_PERF_JIT
// Only compile on v8_component_build.
#include "ohos-perf-jit.h"

#include "src/common/assert-scope.h"
#include "src/flags/flags.h"

// Only compile the {LinuxPerfJitLogger} on Linux.
#if V8_OS_LINUX

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <unistd.h>

#include <memory>

#include "hilog.h"
#include "perf-jit-common.h"
#include "src/base/platform/wrappers.h"
#include "src/codegen/assembler.h"
#include "src/codegen/source-position-table.h"
#include "src/diagnostics/eh-frame.h"
#include "src/objects/code-kind.h"
#include "src/objects/objects-inl.h"
#include "src/objects/shared-function-info.h"
#include "src/snapshot/embedded/embedded-data.h"
#include "src/utils/ostreams.h"

#if V8_ENABLE_WEBASSEMBLY
#include "src/wasm/wasm-code-manager.h"
#endif  // V8_ENABLE_WEBASSEMBLY

namespace v8 {
namespace internal {

base::LazyRecursiveMutex& GetFileMutex() {
  static base::LazyRecursiveMutex file_mutex;
  return file_mutex;
}

// The following static variables are protected by
// GetFileMutex().
int LinuxPerfJitLogger::process_id_ = 0;
uint64_t LinuxPerfJitLogger::reference_count_ = 0;
uint8_t* LinuxPerfJitLogger::marker_address_ = nullptr;
uint8_t* LinuxPerfJitLogger::marker_address_base_ = nullptr;
uint64_t LinuxPerfJitLogger::code_index_ = 0;

bool LinuxPerfJitLogger::HasEnoughMemory(uint64_t size) {
  if (static_cast<uint64_t>(marker_address_ - marker_address_base_) + size <
      SHM_SIZE) {
    return true;
  }
  HilogPrint(ERROR,
             "Generated jit code is too large, no enough memroy to write.");
  return false;
}

void LinuxPerfJitLogger::OpenJitDumpFile() {
  marker_address_base_ =
      reinterpret_cast<uint8_t*>(mmap(nullptr, SHM_SIZE, PROT_READ | PROT_WRITE,
                                      MAP_SHARED | MAP_ANONYMOUS, -1, 0));
  if (marker_address_base_ == MAP_FAILED) {
    marker_address_base_ = nullptr;
    HilogPrint(ERROR, "Failed to map shared memory.");
  } else {
    HilogPrint(INFO, "Success to map shared memory. Adderss is %{public}p",
               marker_address_base_);
    std::string tagName = JitSymbolMemTagName(process_id_);
    if (prctl(PR_SET_VMA, 0, marker_address_base_, SHM_SIZE, tagName.c_str()) ==
        -1) {
      HilogPrint(ERROR, "Failed to set map tag");
    }
  }
  marker_address_ = marker_address_base_;
}

LinuxPerfJitLogger::LinuxPerfJitLogger(Isolate* isolate)
    : CodeEventLogger(isolate) {
  base::LockGuard<base::RecursiveMutex> guard_file(GetFileMutex().Pointer());
  process_id_ = base::OS::GetCurrentProcessId();

  reference_count_++;
  // If this is the first logger, open the file and write the header.
  if (reference_count_ == 1) {
    OpenJitDumpFile();
    if (marker_address_base_ == nullptr) {
      return;
    }
    LogWriteHeader();
  }
}

LinuxPerfJitLogger::~LinuxPerfJitLogger() {
  base::LockGuard<base::RecursiveMutex> guard_file(GetFileMutex().Pointer());
  reference_count_--;
  if (reference_count_ == 0 && marker_address_base_ != nullptr) {
    munmap(marker_address_base_, SHM_SIZE);
    marker_address_base_ = nullptr;
  }
}

uint64_t LinuxPerfJitLogger::GetTimestamp() {
  struct timespec ts;
  int result = clock_gettime(CLOCK_MONOTONIC, &ts);
  DCHECK_EQ(0, result);
  USE(result);
  static const uint64_t kNsecPerSec = 1000000000;
  return (ts.tv_sec * kNsecPerSec) + ts.tv_nsec;
}

void LinuxPerfJitLogger::LogRecordedBuffer(
    Tagged<AbstractCode> abstract_code,
    MaybeHandle<SharedFunctionInfo> maybe_sfi,
    const char* name,
    int length) {
  DisallowGarbageCollection no_gc;
  if (v8_flags.perf_basic_prof_only_functions) {
    CodeKind code_kind = abstract_code->kind(isolate_);
    if (!CodeKindIsJSFunction(code_kind)) {
      return;
    }
  }

  base::LockGuard<base::RecursiveMutex> guard_file(GetFileMutex().Pointer());

  if (marker_address_base_ == nullptr) {
    return;
  }
  // We only support non-interpreted functions.
  if (!IsCode(abstract_code, isolate_)) {
    return;
  }
  Tagged<Code> code = Cast<Code>(abstract_code);

  const char* code_name = name;
  uint8_t* code_pointer = reinterpret_cast<uint8_t*>(code->instruction_start());

  WriteJitCodeLoadEntry(code_pointer, code->instruction_size(), code_name,
                        length);
}

#if V8_ENABLE_WEBASSEMBLY
void LinuxPerfJitLogger::LogRecordedBuffer(const wasm::WasmCode* code,
                                           const char* name,
                                           int length) {
  base::LockGuard<base::RecursiveMutex> guard_file(GetFileMutex().Pointer());

  if (marker_address_base_ == nullptr) {
    return;
  }

  WriteJitCodeLoadEntry(code->instructions().begin(),
                        code->instructions().length(), name, length);
}
#endif  // V8_ENABLE_WEBASSEMBLY

void LinuxPerfJitLogger::WriteJitCodeLoadEntry(const uint8_t* code_pointer,
                                               uint32_t code_size,
                                               const char* name,
                                               int name_length) {
  PerfJitCodeLoad code_load;
  code_load.event_ = PerfJitCodeLoad::kLoad;
  code_load.size_ =
      sizeof(code_load) + name_length + sizeof(kStringTerminator) + code_size;
  code_load.time_stamp_ = GetTimestamp();
  code_load.process_id_ = static_cast<uint32_t>(process_id_);
  code_load.thread_id_ = static_cast<uint32_t>(base::OS::GetCurrentThreadId());
  code_load.vma_ = reinterpret_cast<uint64_t>(code_pointer);
  code_load.code_address_ = reinterpret_cast<uint64_t>(code_pointer);
  code_load.code_size_ = code_size;
  code_load.code_id_ = code_index_;

  code_index_++;

  uint64_t codeLoadWithTerminatorSize = code_load.size_ + sizeof(kJitCodeTerminator);
  if (marker_address_base_ != nullptr && HasEnoughMemory(codeLoadWithTerminatorSize)) {
    LogWriteBytes(reinterpret_cast<const char*>(&code_load), sizeof(code_load));
    LogWriteBytes(name, name_length);
    LogWriteBytes(kStringTerminator, sizeof(kStringTerminator));
    LogWriteBytes(reinterpret_cast<const char*>(code_pointer), code_size);
    // write jit code Terminator
    LogWriteBytes(kJitCodeTerminator, sizeof(kJitCodeTerminator));
    // allow next jit code to overwrite this Terminator
    marker_address_ -= sizeof(kJitCodeTerminator);
    // the last jit code will keep the Terminator
  }
}

void LinuxPerfJitLogger::LogWriteBytes(const char* bytes, uint64_t size) {
  if (marker_address_base_ != nullptr && HasEnoughMemory(size)) {
    std::memcpy(marker_address_, bytes, size);
    marker_address_ += size;
  }
}

void LinuxPerfJitLogger::LogWriteHeader() {
  PerfJitHeader header;

  header.magic_ = PerfJitHeader::kMagic;
  header.version_ = PerfJitHeader::kVersion;
  header.size_ = sizeof(header);
  header.elf_mach_target_ = GetElfMach();
  header.reserved_ = 0xDEADBEEF;
  header.process_id_ = process_id_;
  header.time_stamp_ = static_cast<uint64_t>(
      V8::GetCurrentPlatform()->CurrentClockTimeMillisecondsHighResolution() *
      base::Time::kMicrosecondsPerMillisecond);
  header.flags_ = 0;

  LogWriteBytes(reinterpret_cast<const char*>(&header), sizeof(header));
}

}  // namespace internal
}  // namespace v8

#endif  // V8_OS_LINUX

#endif  // V8_ENABLE_OHOS_PERF_JIT