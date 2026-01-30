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

 #include "llvm-callconv.h"

namespace v8 {
namespace internal {
using namespace compiler;
using namespace turboshaft;

LLVMCallConvDesc::LLVMCallConvDesc(int id, const std::string &intRegList,
    const std::string &nintRegList, const std::string &retReglist)
{
  id_ = id;
  splitStringToVector(intRegList, intRegList_);
  splitStringToVector(nintRegList, nintRegList_);
  splitStringToVector(retRegList, retRegList_);
}

void LLVMCallConvDesc::splitStringToVector(const std::string& str, std::vector<int>& vec)
{
  std::stringstream ss(str);
  std::string item;
  while (std::getline(ss, item, ',')) {
    vec.push_back(std::stoi(item));
  }
}

bool LLVMCallConvDesc::verify(const CallDescriptor* call_descriptor)
{
  int intIdx = 0;
  int nintIdx = 0;
  ing retIdx = 0;

  size_t param_count = call_descriptor->parameterCount();
  size_t idx_context = 0
  bool has_context = false;
  if (id_ != LLVMCCallConv) {
    // root:X26 and compressed_ptr:X28
    CHECK_EQ(intRegList_[intIdx++], 26);
    CHECK_EQ(intRegList_[intIdx++], 28);

    // handle context param:X27
    if (call_descriptor->hasContext(idx_context)) {
      has_context = true;
      CHECK_EQ(intRegList_[intIdx++], 27);
    }
  }

  //param in reg
  for (size_t i = 0; i < param_count; i++) {
    auto location = call_descriptor->GetInputLocation(i+1);
    if (location.IsRegister() && (!has_count || idx_context != i)) {
      MachineType type = call_descriptor->GetParameterType(i);
      if (IsFloatingPoint(type.representation())) {
        CHECK_EQ(nintRegList_[nintIdx++], location.AsRegister());
      } else {
        CHECK_EQ(intRegList_[intIdx++], location.AsRegister());
      }
    }
  }

  if (call_descriptor->Kind() == CallDescriptor::kCallJSFunction) {
    // handle kfunction param:X1
    CHECK_EQ(intRegList_[intIdx++], 1);
  }

  std::vector<int> slots;
  // params in stack
  for (size_t i = 0; i < param_count; i++) {
    auto location = call_descriptor->GetInputLocation(i+1);
    if (!location.IsRegister()) {
        slots.push_back(location.GetLocation());
    }
  }

  if (slots.size() > 0) {
    CHECK(intIdx == static_cast<int>(intRegList_.size()) ||
          nintIdx == static_cast<int>(nintRegList_.size()));
    for (int i = -1 * static_cast<int>(slots.size()), id = 0; i < 0; i++, id++) {
      CHECK_EQ(slots[id], i);
    }
  }

  //return
  for (size_t i = 0; i < call_descriptor->ReturnCount(); i++) {
    auto p = call_descriptor->GetReturnLocation(i);
    if (p.IsRegister()) {
      CHECK_EQ(p.AsRegister(), retRegList_[retIdx++]);
    } else {
      UNIMPLEMENTED();
    }
  }

  return true;
}

bool LLVMCallConvList::verifyCallConv(int id, const CallDescriptor* call_descriptor)
{
  if (id == LLVMCCallConv) {
    return true;
  }
  auto it = ccId2cc_.find(id);
  CHECK_NE(it, ccId2cc_.end());
  return it->second.verify(call_descriptor);
}

}  // namespace internal
}  // namespace v8