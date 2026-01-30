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

#include "src/compiler/turboshaft/pipelines.h"

#include "llvm-ir-builder.h"
#include "llvm-codegen.h"

namespace v8::internal::compiler::turboshaft {

bool Pipeline::IsSupportedBuiltin(Builtin builtin)
{
  switch (builtin) {
  case Builtin::kBigIntEqual:
  case Builtin::kLoadIC:
  case Builtin::kKeyedLoadIC_Megamorphic:
  case Builtin::kForInFilter:
  case Builtin::kStoreIC:
  case Builtin::kKeyedStoreIC_Megamorphic:
  case Builtin::kCreateShallowObjectLiteral:
  case Builtin::kDefineNamedOwnIC:
  case Builtin::kFindOrderedHashSetEntry:
  case Builtin::kGetProperty:
  case Builtin::kKeyedHasIC_Megamorphic:
  case Builtin::kKeyedLoadIC:
  case Builtin::kKeyedLoadIC_PolymorphicName:
  case Builtin::kKeyedStoreIC:
  case Builtin::kLoadIC_Megamorphic:
  case Builtin::kLoadIC_NoFeedback:
  case Builtin::kStoreIC_NoFeedback:
  case Builtin::kStringEqual:
  case Builtin::kArrayIncludesSmiOrObject:
  case Builtin::kArrayIndexOfSmiOrObject:
  case Builtin::kCreateEmptyLiteralObject:
  case Builtin::kCreateShallowArrayLiteral:
  case Builtin::kEnqueueMicrotask:
  case Builtin::kFastNewFunctionContextFunction:
  case Builtin::kForInEnumerate:
  case Builtin::kGetPropertyWithReceiver:
  case Builtin::kGrowFastSmiOrObjectElements:
  case Builtin::kLoadGlobalIC:
  case Builtin::kLoadGlobalIC_NoFeedback:
  case Builtin::kOrdinaryGetOwnPropertyDescriptor:
  case Builtin::kStoreFastElementIC_NoTransitionGrowAndHandleCOW:
  case Builtin::kStoreIC_Megamorphic:
  case Builtin::kStrictEqual_WithFeedback:
  case Builtin::kSubString:
  case Builtin::kTypeof:
  case Builtin::kWeakCollectionSet:
    return true;
  default:
    return false;
  }
}

MaybeHandle<Code> Pipeline::GenerateCodeByLLVM(Linkage *linkage)
{
  // build llvm ir
  LLVMModule *module = new LLVMModule("builtin", false, "aarch64-unknown-linux-gnu");
  module->AddFunction(data()->debug_name(), linkage->GetIncomingDescriptor());
  {
    LLVMIRBuilder builder(data(), linkage, module);
    builder.Build();
  }
  Handle<Code> code;
  {
    LLVMIRGenerator gen(data(), linkage, module, false);
    code = gen.GenerateCode();
  }
  delete module;
  module = nullptr;
  return code;
}

}