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

#ifndef V8_CODEGEN_LLVM_LLVM_IR_BUILDER_H
#define V8_CODEGEN_LLVM_LLVM_IR_BUILDER_H

#include <string>

#include "llvm-c/Core.h"
#include "llvm-c/DebugInfo.h"

#include "src/compiler/linkage.h"
#include "src/compiler/opcodes.h"
#include "src/compiler/turboshaft/phase.h"
#include "src/objects/heap-object-inl.h"
#include "llvm-callconv.h"

namespace v8 {
namespace internal {
using namespace compiler;
using namespace turboshaft;

class LLVMIRBuilder;
using HandleType = void(LLVMIRBuilder::*)(OpIndex);
using Rep = turboshaft::RegisterRepresentation;

#define IROPCODES(V)                                \
  V(Parameter, (OpIndex node))                      \
  V(TaggedBitcast, (OpIndex node))                  \
  V(Constant, (OpIndex node))                       \
  V(Load, (OpIndex node))                           \
  V(Store, (OpIndex node))                          \
  V(WordBinop, (OpIndex node))                      \
  V(FloatBinop, (OpIndex node))                     \
  V(FloatUnary, (OpIndex node))                     \
  V(Comparison, (OpIndex node))                     \
  V(Branch, (OpIndex node))                         \
  V(Shift, (OpIndex node))                          \
  V(Change, (OpIndex node))                         \
  V(Goto, (OpIndex node))                           \
  V(Phi, (OpIndex node))                            \
  V(Call, (OpIndex node))                           \
  V(TailCall, (OpIndex node))                       \
  V(Switch, (OpIndex node))                         \
  V(OverflowCheckedBinop, (OpIndex node))           \
  V(Projection, (OpIndex node))                     \
  V(Tuple, (OpIndex node))                          \
  V(DidntThrow, (OpIndex node))                     \
  V(AbortCSADcheck, (OpIndex node))                 \
  V(DebugBreak, (OpIndex node))                     \
  V(Unreachable, (OpIndex node))                    \
  V(StackPointerGreaterThan, (OpIndex node))        \
  V(FrameConstant, (OpIndex node))                  \
  V(Return, (OpIndex node))                         

class LLVMModule {
public:
  LLVMModule(const std::string &name, bool log_debug, const std::string &triple);
  ~LLVMModule();

  LLVMTypeRef ConvertLLVMTypeFromMachineType(MachineType type);
  LLVMTypeRef GetFuncType(CallDescriptor *descriptor);
  void GenParamTypeList(CallDescriptor *descriptor, std::vector<LLVMTypeRef>& param_types);
  void AddFunction(const std::string name, CallDescriptor *descriptor);

  int ptr_compr_cage_base_index()
  {
    return ptr_compr_cage_base_index_;
  }
  int root_index()
  {
    return root_index_;
  }
  int GetLLVMParamIdxByTurboshaftParamIdx(int shaftIdx)
  {
    CHECK_NE(turboshaftParamIdx2llvmParamIdx_.find(shaftIdx), turboshaftParamIdx2llvmParamIdx_.end());
    return turboshaftParamIdx2llvmParamIdx_[shaftIdx];
  }

  LLVMValueRef GetFunction() { return func_; }
  LLVMModuleRef GetModule() { return module_; }
  LLVMContextRef GetContext() { return context_; }

  LLVMTypeRef GetVoidT() { return voidT_; }
  LLVMTypeRef GetInt1T() { return int1T_; }
  LLVMTypeRef GetInt8T() { return int8T_; }
  LLVMTypeRef GetInt16T() { return int16T_; }
  LLVMTypeRef GetInt32T() { return int32T_; }
  LLVMTypeRef GetInt64T() { return int64T_; }
  LLVMTypeRef GetHalfT() { return halfT_; }
  LLVMTypeRef GetFloatT() { return floatT_; }
  LLVMTypeRef GetDoubleT() { return doubleT_; }
  LLVMTypeRef GetTaggedHPtrT() { return taggedHPtrT_; }
  LLVMTypeRef GetTaggedPtrT() { return taggedPtrT_; }
  LLVMTypeRef GetRawPtrT() { return rawPtrT_; }

private:
  LLVMValueRef func_ {nullptr};
  int ptr_compr_cage_base_index_ {-1};
  int root_index_ {-1};
  std::unordered_map<int, int> turboshaftParamIdx2llvmParamIdx_;

  LLVMModuleRef module_ {nullptr};
  LLVMContextRef context_ {nullptr};

  LLVMTypeRef voidT_ {nullptr};
  LLVMTypeRef int1T_ {nullptr};
  LLVMTypeRef int8T_ {nullptr};
  LLVMTypeRef int16T_ {nullptr};
  LLVMTypeRef int32T_ {nullptr};
  LLVMTypeRef int64T_ {nullptr};
  LLVMTypeRef halfT_ {nullptr};
  LLVMTypeRef floatT_ {nullptr};
  LLVMTypeRef doubleT_ {nullptr};
  LLVMTypeRef taggedHPtrT_ {nullptr};
  LLVMTypeRef taggedPtrT_ {nullptr};
  LLVMTypeRef rawPtrT_ {nullptr};
};

class LLVMIRBuilder {
public:
  LLVMIRBuilder(PipelineData* data, Linkage* linkage,
                LLVMModule* module);
  ~LLVMIRBuilder();
  void Build();
private:
  #define DECLAREVISITIROPCODE(name, signature) void Visit##name signature;
    IROPCODES(DECLAREVISITIROPCODE)
  #undef DECLAREVISITIROPCODE
  #define DECLAREHANDLEIROPCODE(name, ignore) void Handle##name(OpIndex node);
    IROPCODES(DECLAREHANDLEIROPCODE)
  #undef DECLAREHANDLEIROPCODE

  void InitialHandlers();
  LLVMTypeRef ConvertLLVMTypeFromMachineType(MachineType type)
  {
    return llvm_module_->ConvertLLVMTypeFromMachineType(type);
  }
  LLVMTypeRef ConvertLLVMTypeFromRep(Rep rep);
  LLVMTypeRef GetVoidT()
  {
    return llvm_module_->GetVoidT();
  }
  LLVMTypeRef GetInt1T()
  {
    return llvm_module_->GetInt1T();
  }
  LLVMTypeRef GetInt8T()
  {
    return llvm_module_->GetInt8T();
  }
  LLVMTypeRef GetInt16T()
  {
    return llvm_module_->GetInt16T();
  }
  LLVMTypeRef GetInt32T()
  {
    return llvm_module_->GetInt32T();
  }
  LLVMTypeRef GetInt64T()
  {
    return llvm_module_->GetInt64T();
  }
  LLVMTypeRef GetHalfT()
  {
    return llvm_module_->GetHalfT();
  }
  LLVMTypeRef GetFloatT()
  {
    return llvm_module_->GetFloatT();
  }
  LLVMTypeRef GetDoubleT()
  {
    return llvm_module_->GetDoubleT();
  }
  LLVMTypeRef GetTaggedHPtrT()
  {
    return llvm_module_->GetTaggedHPtrT();
  }
  LLVMTypeRef GetTaggedPtrT()
  {
    return llvm_module_->GetTaggedPtrT();
  }
  LLVMTypeRef GetRawPtrT()
  {
    return llvm_module_->GetRawPtrT();
  }
  void Bind(OpIndex node, LLVMValueRef llvm_value)
  {
    CHECK_WITH_MSG(llvm_value != nullptr,
                   ("Transformed To Null LLVMValue Node: " + NodeName(node)).c_str());
    node2LValue_[node.id()] = llvm_value;
  }
  LLVMValueRef GetLValueOf(OpIndex node)
  {
    LLVMValueRef get = node2LValue_[node.id()];
    CHECK_WITH_MSG(get != nullptr, ("Not Transformed Node: " + NodeName(node)).c_str());
    return get;
  }
  LLVMBasicBlockRef GetLBlockOf(const Block *block)
  {
    LLVMBasicBlockRef get = block2LBB_[block->index()];
    CHECK_NOT_NULL(get);
    return get;
  }
  LLVMBasicBlockRef GetEndLBlockOf(const Block *block)
  {
    LLVMBasicBlockRef get = block2EndLBB_[block->index()];
    CHECK_NOT_NULL(get);
    return get;
  }
  void CompletePendingPhis();
  uint32_t GetPtrAddressSpace(LLVMValueRef value) const;
  LLVMValueRef CanonicalizeToPtr(LLVMValueRef value, LLVMTypeRef ptr_type) const;
  LLVMValueRef FixTypeTo(LLVMValueRef value, LLVMTypeRef to_type, LLVMBasicBlockRef insert);
  void SetGCLeafFunction(LLVMValueRef call);

  // turboshaft node helper
  std::string NodeName(OpIndex node);
  Constant ToConstant(const ConstantOp& constant);
  bool IsMaterializableFromRoot(Handle<HeapObject> object, RootIndex* index_return);
  LLVMValueRef GetPtrComprCageBase()
  {
    return LLVMBuildIntToPtr(builder_, GetPurePtrComprCageBase(), GetTaggedHPtrT(), "");
  }
  LLVMValueRef GetRoot()
  {
    return LLVMBuildIntToPtr(builder_, GetPureRoot(), GetTaggedHPtrT(), "");
  }
  LLVMValueRef GetPurePtrComprCageBase()
  {
    return ptr_compr_cage_base_;
  }
   LLVMValueRef GetPureRoot()
  {
    return root_reg_;
  }

  LLVMCallConv GetLLVMCallConvByDescriptor(const CallDescriptor* descriptor);
  void InitDescriptorCallConvMap();
  void PrepareCallParams(const CallDescriptor* call_descriptor, base::Vector<const OpIndex>& arguments,
    std::vector<LLVMTypeRef>& param_types, std::vector<LLVMValueRef>& args, LLVMValueRef callee);
  LLVMValueRef GetLLVMParamByTurboshaftParamIdx(int shaftIdx)
  {
    return LLVMGetParam(function_, llvm_module_->GetLLVMParamIdxByTurboshaftParamIdx(shaftIdx));
  }
  LLVMValueRef CalculateFuncFp();
  void AddBranchWeight(LLVMValueRef branch, uint32_t true_weight, uint32_t false_weight);

  PipelineData* data_ {nullptr};
  Linkage* linkage_ {nullptr};
  const turboshaft::Graph& graph_;

  LLVMModuleRef module_ {nullptr};
  LLVMContextRef context_ {nullptr};
  LLVMValueRef function_ {nullptr};
  LLVMBuilderRef builder_ {nullptr};
  LLVMBasicBlockRef current_lbb_ {nullptr};

  LLVMValueRef root_reg_ {nullptr};
  LLVMValueRef ptr_compr_cage_base_ {nullptr};

  std::unordered_map<Opcode, HandleType> op_handlers_;
  std::unordered_map<std::string, std::vector<LLVMCallConv>> decscriptor2cc_;
  std::unordered_map<uint32_t, LLVMValueRef> node2LValue_;
  std::map<BlockIndex, LLVMBasicBlockRef> block2LBB_;
  // if a block was split to [ LBB1 ... LBB3], this map records the last LBB (LBB3).
  // if not, this map records the only LBB.
  std::map<BlockIndex, LLVMBasicBlockRef> block2EndLBB_;
  std::set<std::pair<OpIndex, Block*>> pending_phis_;
  Block* current_block_ {nullptr};

  LLVMModule* llvm_module_ {nullptr};
  std::shared_ptr<LLVMCallConvList> llvmCallConvList_;
};

}  // namespace internal
}  // namespace v8

#endif // V8_CODEGEN_LLVM_LLVM_IR_BUILDER_H