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

#ifndef V8_CODEGEN_LLVM_LLVM_CALLCONV_H
#define V8_CODEGEN_LLVM_LLVM_CALLCONV_H

#include <string>

#include "llvm-c/Core.h"
#include "llvm-c/DebugInfo.h"

#include "src/compiler/linkage.h"
#include "src/compiler/opcodes.h"
#include "src/compiler/turboshaft/phase.h"
#include "src/objects/heap-object-inl.h"

namespace v8 {
namespace internal {
using namespace compiler;
using namespace turboshaft;

#define CC_DESC_LIST(V)                                                                         \
  V(LLVMAArch64V8BigIntEqCallConv, "26,28,27,0,1", "0", "0")                                    \
  V(LLVMAArch64V8KeyedLoadWithVectorCallConv, "26,28,27,1,0,2,3", "0", "0")                     \
  V(LLVMAArch64V8LoadWithVectorCallConv, "26, 28, 27, 1, 2, 0, 3", "0", "0")                    \
  V(LLVMAArch64V8ToNameCallConv, "26, 28, 27, 1, 0", "0", "0")                                  \
  V(LLVMAArch64V8CallFunctionTemplateGenericCallConv, "26, 28, 27, 1, 2, 3", "0", "0")          \
  V(LLVMAArch64V8CallApiCallbackOptimizedCallConv, "26, 28, 27, 1, 2, 3, 0", "0", "0")          \
  V(LLVMAArch64V8CallTrampolineCallConv, "26, 28, 27, 1, 0", "0", "0")                          \
  V(LLVMAArch64V8CallApiCallbackGenericCallConv, "26, 28, 27, 2, 1, 3, 0", "0", "0")            \
  V(LLVMAArch64V8AllocateCallConv, "26, 28, 1, 0, 2", "0", "0")                                 \
  V(LLVMAArch64V8ProxyGetPropertyCallConv, "26, 28, 27, 0, 1, 2, 3, 4", "0", "0")               \
  V(LLVMAArch64InterpreterDispatchCallConv, "26, 28, 27, 0, 19, 20, 21", "0", "0")              \
  V(LLVMAArch64InterpreterNoContextCallConv, "26, 28, 0, 19, 20, 21", "0", "0")                 \
  V(LLVMAArch64V8StoreWithVectorCallConv, "26, 28, 27, 1, 2, 0, 4, 3", "0", "0")                \
  V(LLVMAArch64V8StoreTransitionCallConv, "26, 28, 27, 1, 2, 5, 0, 4", "0", "0")                \
  V(LLVMAArch64JSCallCallConv, "26, 28, 27, 3, 0, 1", "0", "0")                                 \
  V(LLVMAArch64JSCallLeapTierCallConv, "26, 28, 27, 3, 0, 4, 1", "0", "0")                      \
  V(LLVMAArch64V8CLikeCallConv, "26, 28, 0, 1, 2", "0", "0")                                    \
  V(LLVMAArch64V8JSTrampolineCallConv, "26, 28, 27, 1, 3, 0, 4", "0", "0")                      \
  V(LLVMAArch64ConstructStubCallConv, "26, 28, 27, 1, 3, 0", "0", "0")                          \
  V(LLVMAArch64CallVarargsCallConv, "26, 28, 27, 1, 0, 4, 2", "0", "0")                         \
  V(LLVMAArch64GrowArrayElementsCallConv, "26, 28, 0, 3", "0", "0")                             \
  V(LLVMAArch64LoadGlobalWithVectorCallConv, "26, 28, 27, 2, 0, 3", "0", "0")                   \
  V(LLVMAArch64LoadBaselineCallConv, "26, 28, 1, 2, 0", "0", "0")                               \
  V(LLVMAArch64EnumeratedKeyedLoadCallConv, "26, 28, 27, 1, 0, 4, 5, 2", "0", "0")              \
  V(LLVMAArch64V8ApiGetterCallConv, "26, 28, 27, 1, 0, 3", "0", "0")

class LLVMCallConvDesc {
public:
  LLVMCallConvDesc(int id, const std::string &intRegList,
                   const std::string &nintRegList, const std::string &retRegList);

  ~LLVMCallConvDesc() {}
  bool verify(const CallDescriptor* call_descriptor);

private:
  void splitStringToVector(const std::string& str, std::vector<int>& vec);
  int id_;
  std::vector<int> intRegList_;
  std::vector<int> nintRegList_;
  std::vector<int> retRegList_;
};

class LLVMCallConvList {
public:
  LLVMCallConvList()
  {
    init();
  }
  
  bool verifyCallConv(int id, const CallDescriptor* call_descriptor);

private:
  std::unordered_map<int, LLVMCallConvDesc> ccId2cc_;

  void init()
  {
    #define INIT_CC_LIST(CC_ID, INT_REG_LIST, NONINT_REG_LIST, RETURN_REG_LIST) \
      ccId2cc_.emplace(CC_ID, LLVMCallConvDesc(CC_ID, INT_REG_LIST, NONINT_REG_LIST, RETURN_REG_LIST));
    
    CC_DESC_LIST(INIT_CC_LIST);
    #undef INIT_CC_LIST
    }
};

}  // namespace internal
}  // namespace v8

#endif  // V8_CODEGEN_LLVM_LLVM_CALLCONV_H
