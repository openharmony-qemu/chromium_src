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

#ifndef V8_DIAGNOSTICS_OHOS_PERF_JIT_H_
#define V8_DIAGNOSTICS_OHOS_PERF_JIT_H_

#ifdef V8_ENABLE_OHOS_PERF_JIT

#include "include/v8config.h"

// {LinuxPerfJitLogger} is only implemented on Linux.
#if V8_OS_LINUX

#include "src/logging/log.h"

namespace v8 {
namespace internal {

// Linux perf tool logging support.
class LinuxPerfJitLogger : public CodeEventLogger {
 public:
  explicit LinuxPerfJitLogger(Isolate* isolate);
  ~LinuxPerfJitLogger() override;

  void CodeMoveEvent(Tagged<InstructionStream> from,
                     Tagged<InstructionStream> to) override {
    UNREACHABLE();  // Unsupported.
  }
  void BytecodeMoveEvent(Tagged<BytecodeArray> from,
                         Tagged<BytecodeArray> to) override {}
  void CodeDisableOptEvent(Handle<AbstractCode> code,
                           Handle<SharedFunctionInfo> shared) override {}

 private:
  bool HasEnoughMemory(uint64_t size);
  void OpenJitDumpFile();
  void CloseJitDumpFile();

  uint64_t GetTimestamp();
  void LogRecordedBuffer(Tagged<AbstractCode> code,
                         MaybeHandle<SharedFunctionInfo> maybe_shared,
                         const char* name,
                         int length) override;
#if V8_ENABLE_WEBASSEMBLY
  void LogRecordedBuffer(const wasm::WasmCode* code,
                         const char* name,
                         int length) override;
#endif  // V8_ENABLE_WEBASSEMBLY

  // File buffer size of the low-level log. We don't use the default to
  // minimize the associated overhead.
  static const int kLogBufferSize = 2 * MB;

  void WriteJitCodeLoadEntry(const uint8_t* code_pointer,
                             uint32_t code_size,
                             const char* name,
                             int name_length);

  void WriteByteCodeLoadEntry(uintptr_t code_pointer,
                              uint32_t code_size,
                              const char* name,
                              int name_length);

  void LogWriteBytes(const char* bytes, uint64_t size);
  void LogWriteHeader();

  static const uint32_t kElfMachIA32 = 3;
  static const uint32_t kElfMachX64 = 62;
  static const uint32_t kElfMachARM = 40;
  static const uint32_t kElfMachMIPS64 = 8;
  static const uint32_t kElfMachLOONG64 = 258;
  static const uint32_t kElfMachARM64 = 183;
  static const uint32_t kElfMachS390x = 22;
  static const uint32_t kElfMachPPC64 = 21;
  static const uint32_t kElfMachRISCV = 243;

  uint32_t GetElfMach() {
#if V8_TARGET_ARCH_IA32
    return kElfMachIA32;
#elif V8_TARGET_ARCH_X64
    return kElfMachX64;
#elif V8_TARGET_ARCH_ARM
    return kElfMachARM;
#elif V8_TARGET_ARCH_MIPS64
    return kElfMachMIPS64;
#elif V8_TARGET_ARCH_LOONG64
    return kElfMachLOONG64;
#elif V8_TARGET_ARCH_ARM64
    return kElfMachARM64;
#elif V8_TARGET_ARCH_S390X
    return kElfMachS390x;
#elif V8_TARGET_ARCH_PPC64
    return kElfMachPPC64;
#elif V8_TARGET_ARCH_RISCV32 || V8_TARGET_ARCH_RISCV64
    return kElfMachRISCV;
#else
    UNIMPLEMENTED();
    return 0;
#endif
  }

#if V8_TARGET_ARCH_32_BIT
  static const int kElfHeaderSize = 0x34;
#elif V8_TARGET_ARCH_64_BIT
  static const int kElfHeaderSize = 0x40;
#else
#error Unknown target architecture pointer size
#endif

  // Per-process singleton file. We assume that there is one main isolate;
  // to determine when it goes away, we keep reference count.
  static uint64_t reference_count_;
  static uint8_t* marker_address_;
  static uint8_t* marker_address_base_;
  static uint64_t code_index_;
  static int process_id_;
};

}  // namespace internal
}  // namespace v8

#endif  // V8_OS_LINUX

#endif  // V8_DIAGNOSTICS_PERF_JIT_H_
#endif  // V8_ENABLE_OHOS_PERF_JIT