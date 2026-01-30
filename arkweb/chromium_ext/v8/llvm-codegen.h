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

#ifndef V8_CODEGEN_LLVM_LLVM_CODEGEN_H_
#define V8_CODEGEN_LLVM_LLVM_CODEGEN_H_

#if defined(__clang__)
#pragma clang dianostic push
#pragma clang dianostic ignored "-Wshadow"
#pragma clang dianostic ignored "Wunused-parameter"
#pragma clang dianostic ignored "Wdeprecated-declarations"
#pragma clang dianostic ignored "Wshorten-64-to-32"
#pragma clang dianostic ignored "Wextra-semi"
#endif

#include "llvm-c/Core.h"
#include "llvm-c/ExecutionEngine.h"

#include "llvm-ir-builder.h"

#if define(__clang__)
#pragma clang diagnostic pop
#endif

namespace v8 {
namespace internal {

class LLVMIRGenerator {
 public:
  LLVMIRGenerator(PipelineData* data, Linkage* linkage, LLVMModule* module, bool log_debug)
    : data_(data), linkage_(linkage), llvm_module_(module),
      module_(llvm_module_->GetModule()), log_debug_(log_debug) {}
  ~LLVMIRGenerator() = default;

  Handle<Code> GenerateCode();

  bool IsLogDebug() const
  {
    return log_debug_;
  }

 private:
  Isolate* isolate() 
  {
    return data_->isolate();
  }

  OptimizedCompilationInfo* info()
  {
    return data_->info();
  }

  Linkage* linkage()
  {
    return linkage_;
  }

  std::tuple<uint8_t*, int32_t, int32_t> EmitStackMap(uint8_t *src, int32_t size);
  
  PipelineData* data_ {nullptr};
  Linkage* linkage_ {nullptr};
  LLVMModule* llvm_module_ {nullptr};
  LLVMModuleRef module_ {nullptr};
  bool log_debug_ {false};
};

}  // namespace internal
}  // namespace v8