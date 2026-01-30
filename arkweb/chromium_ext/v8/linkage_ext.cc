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

#include "src/compiler/linkage.h"

namespace v8::internal::compiler {

namespace {

// Offsets from callee to caller frame, in slots.
constexpr int kFirstCallerSlotOffset = 1;
constexpr int kNoCallerSlotOffset = 0;

inline LinkageLocation regloc(Register reg, MachineType type) 
{
  return LinkageLocation::ForRegister(reg.code(), type);
}

}  // namespace

bool CallDescriptor::hasContext(size_t& context_idx) const
{
  for (size_t i = 1; i < InputCount(); i++) {
    LinkageLocation location = GetInputLocation(i);
    if (location.IsRegister() && location == regloc(kContextRegister, MachineType::AnyTagged())) {
      context_idx = i - 1;
      return true;
    }
  }
  return false;
}

void CallDescriptor::printDescInfo() const
{
  std::cout << debug_name() << ": kind=" << kind() << ", flags=" << flags() << ", shifted_tag="
            << shifted_tag() << ", ReturnCount=" << ReturnCount() << ", ParameterCount=" << ParameterCount()
            << ", GPParameterCount=" << GPParameterCount() << ", FPParameterCount=" << FPParameterCount()
            << ", NeedsCallerSavedRegisters=" << NeedsCallerSavedRegisters()
            << ", NeedsCallerSavedFPRegisters=" << NeedsCallerSavedFPRegisters()
            << ", GetTaggedParameterSlots((first slot << 16) | num_slots)" << GetTaggedParameterSlots()
            << ", ParameterSlotCount=" << ParameterSlotCount() << ", paramslot list=";
  for (size_t i = 0; i < ParameterSlotCount(); i++) {
    std::cout<<GetStackIndexFromSlot((int)i)<<",";
  }
  std::cout<<" ReturnSlotCount="<<ReturnSlotCount()<<", return list=";
  for (size_t i = 0; i < ReturnCount(); i++) {
    auto p = GetReturnLocation(i);
    if (p.IsRegister()) {
      std::cout<<"reg:"<<p.AsRegister()<<",";
    } else {
      std::cout<<"slot:"<<p.GetLocation()<<",";
    }
  }
  std::cout<<" InputCount="<<InputCount()<<", input list=";
  for (size_t i = 0; i < InputCount(); i++) {
    auto p = GetInputLocation(i);
    if (p.IsRegister()) {
      std::cout<<"reg:"<<p.AsRegister()<<",";
    } else {
      std::cout<<"slot:"<<p.GetLocation()<<",";
    }
  }
  std::cout<<", AllocatableRegisters="<<AllocatableRegisters()
           <<", CalleeSavedRegisters="<<CalleeSavedRegisters()
           <<", CalleeSavedFPRegisters="<<CalleeSavedFPRegisters()
           <<std::endl<<std::endl;
}

}