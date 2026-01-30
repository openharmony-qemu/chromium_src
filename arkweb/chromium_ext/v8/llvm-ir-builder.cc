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

#include "llvm-ir-builder.h"
#include <string>
#include <unordered_map>

#include "llvm-c/Analysis.h"

#include "llvm-c/Core.h"
#include "src/base/logging.h"
#include "src/compiler/backend/code-generator.h"
#include "src/compiler/turboshaft/opmasks.h"
#include "src/builtins/constants-table-builder.h"
#include "src/roots/roots-inl.h"

namespace v8 {
namespace internal {

LLVMModule::LLVMModule(const std::string &name, bool log_debug, const std::string &triple) {
  context_ = LLVMContextCreate();
  module_ = LLVMModuleCreateWithNameInContext(name.c_str(), context_);
  LLVMSetTarget(module_, triple.c_str());

  voidT_ = LLVMVoidTypeInContext(context_);
  int1T_ = LLVMInt1TypeInContext(context_);
  int8T_ = LLVMInt8TypeInContext(context_);
  int16T_ = LLVMInt16TypeInContext(context_);
  int32T_ = LLVMInt32TypeInContext(context_);
  int64T_ = LLVMInt64TypeInContext(context_);
  halfT_ = LLVMHalfTypeInContext(context_);
  floatT_ = LLVMFloatTypeInContext(context_);
  doubleT_ = LLVMDoubleTypeInContext(context_);
  taggedHPtrT_ = LLVMPointerType(LLVMInt64TypeInContext(context_), 1);
  taggedPtrT_ = LLVMPointerType(LLVMInt64TypeInContext(context_), 0);
  rawPtrT_ = LLVMPointerType(LLVMInt8TypeInContext(context_), 0);
}

LLVMModule::~LLVMModule() {
  if (module_) {
    LLVMDisposeModule(module_);
    module_ = nullptr;
  }
  if (context_) {
    LLVMContextDispose(context_);
    context_ = nullptr;
  }
}

LLVMTypeRef LLVMModule::ConvertLLVMTypeFromMachineType(MachineType type) {
  if (type == MachineType::None()) { return voidT_; }
  else if (type == MachineType::Bool()) { return int1T_; }
  else if (type == MachineType::Int8()) { return int8T_; }
  else if (type == MachineType::Int16()) { return int16T_; }
  else if (type == MachineType::Int32()) { return int32T_; }
  else if (type == MachineType::Int64()) { return int64T_; }
  else if (type == MachineType::Uint8()) { return int8T_; }
  else if (type == MachineType::Uint16()) { return int16T_; }
  else if (type == MachineType::Uint32()) { return int32T_; }
  else if (type == MachineType::Uint64()) { return int64T_; }
  else if (type == MachineType::Float16()) { return halfT_; }
  else if (type == MachineType::Float32()) { return floatT_; }
  else if (type == MachineType::Float64()) { return doubleT_; }
  else if (type == MachineType::Pointer()) { return int64T_; }
  else if (type == MachineType::TaggedPointer()) { return taggedHPtrT_; }
  else if (type == MachineType::AnyTagged()) { return taggedHPtrT_; }
  else if (type == MachineType::TaggedSigned()) { return taggedHPtrT_; }
  else if (type == MachineType::SandboxedPointer()) { return int64T_; }
  else {
      std::cout << "[Bad Type]: " << type.representation() << " + " << type.semantic() << std::endl;
      UNREACHABLE();
  }
}


LLVMTypeRef LLVMModule::GetFuncType(CallDescriptor *descriptor) {
  size_t return_count = descriptor->ReturnCount();
  CHECK(return_count == 1 || return_count == 0);
  LLVMTypeRef return_type = (return_count == 0) ?
    voidT_ : ConvertLLVMTypeFromMachineType(descriptor->GetReturnType(0));

  std::vector<LLVMTypeRef> param_types;
  GenParamTypeList(descriptor, param_types);
  // false: not allow va-args
  LLVMTypeRef func_type = LLVMFunctionType(return_type, param_types.data(),
      static_cast<int>(param_types.size()), false);
  return func_type;
}

void LLVMModule::GenParamTypeList(CallDescriptor *descriptor, std::vector<LLVMTypeRef>& param_types) {
  int idx = 0;
  size_t idx_context = 0;
  bool has_context = false;
  if (descriptor->hasContext(idx_context)) {
    has_context = true;
    param_types.push_back(ConvertLLVMTypeFromMachineType(descriptor->GetParameterType(idx_context)));
    turboshaftParamIdx2llvmParamIdx_[static_cast<int>(idx_context)] = idx++;
  }

  size_t param_count = descriptor->ParameterCount();
  // params in reg
  for (size_t i = 0; i < param_count; i++) {
    auto location = descriptor->GetInputLocation(i+1);
    if (location.IsRegister() && (!has_context || idx_context != i)) {
        param_types.push_back(ConvertLLVMTypeFromMachineType(descriptor->GetParameterType(i)));
        turboshaftParamIdx2llvmParamIdx_[static_cast<int>(i)] = idx++;
    }
  }

  // push kfunction param
  if (descriptor->kind() == CallDescriptor::kCallJSFunction) {
    param_types.push_back(LLVMPointerType(GetInt64T(), 0));
    turboshaftParamIdx2llvmParamIdx_[static_cast<int>(param_count)] = idx++;
  }

  std::vector<LLVMTypeRef> stack_param_types;
  // params in stack
  for (size_t i = 0; i < param_count; i++) {
    auto location = descriptor->GetInputLocation(i+1);
    if (!location.IsRegister()) {
        stack_param_types.push_back(ConvertLLVMTypeFromMachineType(descriptor->GetParameterType(i)));
    }
  }
  param_types.insert(param_types.end(), stack_param_types.rbegin(), stack_param_types.rend());
  for (int i = static_cast<int>(param_count - 1); i >= 0; i--) {
    auto location = descriptor->GetInputLocation(i+1);
    if (!location.IsRegister()) {
        turboshaftParamIdx2llvmParamIdx_[i] = idx++;
    }
  }
  CHECK(static_cast<int>(param_types.size()) == idx);
}

void LLVMModule::AddFunction(const std::string name, CallDescriptor *descriptor) {
    LLVMTypeRef func_type = GetFuncType(descriptor);
    func_ = LLVMAddFunction(module_, name.c_str(), func_type);
}

// ============================  LLVMIRBuilder Implement  ========================
LLVMIRBuilder::LLVMIRBuilder(PipelineData* data, Linkage* linkage, LLVMModule* module)
    : data_(data), linkage_(linkage), graph_(data_->graph()), module_(module->GetModule()),
      context_(module->GetContext()), function_(module->GetFunction()), llvm_module_(module) {
  builder_ = LLVMCreateBuilderInContext(context_);

  InitDescriptorCallConvMap();
  const CallDescriptor* incoming_descriptor = linkage_->GetIncomingDescriptor();
  auto cc = GetLLVMCallConvByDescriptor(incoming_descriptor);
  CHECK_NE(cc, LLVMCCallConv);
  llvmCallConvList_ = std::make_shared<LLVMCallConvList>();
  CHECK(llvmCallConvList_->verifyCallConv(cc, incoming_descriptor));
  LLVMSetFunctionCallConv(function_, cc);
  LLVMSetGC(function_, "statepoint-example");
  LLVMAttributeRef sign_ret_attr = LLVMCreateStringAttribute(context_, "sign-return-address",
      strlen("sign-return-address"), "non-leaf", strlen("non-leaf"));
  LLVMAddAttributeAtIndex(function_, LLVMAttributeFunctionIndex, sign_ret_attr);
  sign_ret_attr = LLVMCreateStringAttribute(context_, "sign-return-address-key",
      strlen("sign-return-address-key"), "b_key", strlen("b_key"));
  LLVMAddAttributeAtIndex(function_, LLVMAttributeFunctionIndex, sign_ret_attr);
  LLVMAttributeRef bti_attr = LLVMCreateStringAttribute(context_, "branch-target-enforcement",
      strlen("branch-target-enforcement"), "true", strlen("true"));
  LLVMAddAttributeAtIndex(function_, LLVMAttributeFunctionIndex, bti_attr);
  InitialHandlers();
}

LLVMIRBuilder::~LLVMIRBuilder() {
  if (builder_) {
    LLVMDisposeBuilder(builder_);
    builder_ = nullptr;
  }
}


LLVMCallConv LLVMIRBuilder::GetLLVMCallConvByDescriptor(const CallDescriptor* descriptor) {
  std::string name(descriptor->debug_name());
  auto it = decscriptor2cc_.find(name);
  if (it != decscriptor2cc_.end()) {
    size_t context_idx;
    int cc_idx = descriptor->hasContext(context_idx) ? 1 : 0;
    auto cc = it->second[cc_idx];
    CHECK_NE(cc, LLVMInvalidCallConv);
    return cc;
  }
  descriptor->printDescInfo();
  UNIMPLEMENTED();
}

void LLVMIRBuilder::Build() {
  ZoneVector<Block*> blocks = graph_.blocks_vector();

  for (auto block = blocks.begin(); block != blocks.end(); ++block) {
    std::string buf = "B" + std::to_string((*block)->index().id());
    LLVMBasicBlockRef llvmBB = LLVMAppendBasicBlockInContext(context_, function_, buf.c_str());
    block2LBB_.emplace((*block)->index(), llvmBB);
    block2EndLBB_.emplace((*block)->index(), llvmBB);
  }

  for (auto block = blocks.begin(); block != blocks.end(); ++block) {
    current_block_ = *block;
    current_lbb_ = GetLBlockOf(*block);
    LLVMPositionBuilderAtEnd(builder_, current_lbb_);

    if ((*block)->index().id() == 0) {
      // Gen Frame
      if (linkage_->GetIncomingDescriptor()->kind() == CallDescriptor::kCallCodeObject) {
        LLVMAddTargetDependentFunctionAttr(function_, "frame-pointer", "non-leaf");
        int32_t frame_kind = StackFrame::TypeToMarker(data_->info()->GetOutputStackFrameType());
        const size_t frame_kind_size = 8;
        const size_t pad_size = 8;
        LLVMAddTargetDependentFunctionAttr(function_, "frame-reserved-slots",
                                           std::to_string(frame_kind_size + pad_size).c_str());
        LLVMValueRef frame_address = CalculateFuncFp();

        LLVMValueRef frame_kind_slot = LLVMBuildSub(builder_, frame_address,
                                                   LLVMConstInt(GetInt64T(), frame_kind_size, 0), "");
        LLVMValueRef addr = LLVMBuildIntToPtr(builder_, frame_kind_slot,
                                              LLVMPointerType(GetInt64T(), 0), "frame_kind_slot");
        LLVMValueRef llvm_frame_kind = LLVMConstInt(GetInt64T(), frame_kind, 0);
        LLVMBuildStore(builder_, llvm_frame_kind, addr);

        LLVMValueRef pad_slot = LLVMBuildSub(builder_, frame_address,
                                             LLVMConstInt(GetInt64T(), frame_kind_size + pad_size, 0), "");
        addr = LLVMBuildIntToPtr(builder_, pad_slot, LLVMPointerType(GetInt64T(), 0), "pad_slot");
        LLVMValueRef pad = LLVMConstInt(GetInt64T(), 0, 0);
        LLVMBuildStore(builder_, pad, addr);
      } else if (linkage_->GetIncomingDescriptor()->kind() == CallDescriptor::kCallJSFunction) {
        std::cout << data_->info()->GetDebugName() << std::endl;
        UNIMPLEMENTED();
      } else {
        UNIMPLEMENTED();
      }

      // init Root & PtrComprCageBase
      LLVMMetadataRef meta_string = LLVMMDStringInContext2(context_, "x26", 3);
      LLVMMetadataRef meta = LLVMMDNodeInContext2(context_, &meta_string, 1);
      std::vector<LLVMValueRef> args = {LLVMMetadataAsValue(context_, meta)};
      LLVMValueRef fn = LLVMGetNamedFunction(module_, "llvm.read_register.i64");
      LLVMTypeRef param_types[] = { LLVMMetadataTypeInContext(context_) };
      LLVMTypeRef fn_type = LLVMFunctionType(GetInt64T(), param_types, 1, 0);
      if (!fn) {
        /* init instrinsic function declare */
        fn = LLVMAddFunction(module_, "llvm.read_register.i64", fn_type);
      }
      root_reg_ = LLVMBuildCall2(builder_, fn_type, fn, args.data(), 1, "");
      SetGCLeafFunction(root_reg_);

      meta_string = LLVMMDStringInContext2(context_, "x28", 3);
      meta = LLVMMDNodeInContext2(context_, &meta_string, 1);
      args = { LLVMMetadataAsValue(context_, meta) };
      fn = LLVMGetNamedFunction(module_, "llvm.read_register.i64");
      ptr_compr_cage_base_ = LLVMBuildCall2(builder_, fn_type, fn, args.data(), 1, "");
      SetGCLeafFunction(ptr_compr_cage_base_);
    }

    for (OpIndex node : graph_.OperationIndices(**block)) {
      const turboshaft::Operation& op = graph_.Get(node);

      auto found = op_handlers_.find(op.opcode);
      if (found != op_handlers_.end()) {
        (this->*(found->second))(node);
        continue;
      }
      
      FATAL("LLVMIRBuilder::Build - Not Yet Implement Build Turboshaft Opcode: %s", OpcodeName(op.opcode));
    }
  }

  CompletePendingPhis();
}

LLVMValueRef LLVMIRBuilder::CalculateFuncFp() {
  std::vector<LLVMValueRef> args = { LLVMConstInt(GetInt32T(), 0, 0) };
  LLVMTypeRef param[] = { GetInt32T() };
  LLVMTypeRef frame_func_type = LLVMFunctionType(GetRawPtrT(), param, 1, 0);
  LLVMValueRef frame_func = LLVMGetNamedFunction(module_, "llvm.frameaddress.p0");
  if (!frame_func) {
    frame_func = LLVMAddFunction(module_, "llvm.frameaddress.p0", frame_func_type);
  }
  LLVMValueRef frame_address = LLVMBuildCall2(builder_, frame_func_type, frame_func, args.data(), 1, "");
  SetGCLeafFunction(frame_address);
  frame_address = LLVMBuildPtrToInt(builder_, frame_address, GetInt64T(), "");
  return frame_address;
}

void LLVMIRBuilder::InitialHandlers() {
  op_handlers_ = {
    { Opcode::kParameter, &LLVMIRBuilder::HandleParameter },
    { Opcode::kTaggedBitcast, &LLVMIRBuilder::HandleTaggedBitcast },
    { Opcode::kConstant, &LLVMIRBuilder::HandleConstant },
    { Opcode::kLoad, &LLVMIRBuilder::HandleLoad },
    { Opcode::kStore, &LLVMIRBuilder::HandleStore },
    { Opcode::kWordBinop, &LLVMIRBuilder::HandleWordBinop },
    { Opcode::kFloatBinop, &LLVMIRBuilder::HandleFloatBinop },
    { Opcode::kFloatUnary, &LLVMIRBuilder::HandleFloatUnary },
    { Opcode::kComparison, &LLVMIRBuilder::HandleComparison },
    { Opcode::kBranch, &LLVMIRBuilder::HandleBranch },
    { Opcode::kShift, &LLVMIRBuilder::HandleShift },
    { Opcode::kChange, &LLVMIRBuilder::HandleChange },
    { Opcode::kGoto, &LLVMIRBuilder::HandleGoto },
    { Opcode::kPhi, &LLVMIRBuilder::HandlePhi },
    { Opcode::kCall, &LLVMIRBuilder::HandleCall },
    { Opcode::kTailCall, &LLVMIRBuilder::HandleTailCall },
    { Opcode::kSwitch, &LLVMIRBuilder::HandleSwitch },
    { Opcode::kOverflowCheckedBinop, &LLVMIRBuilder::HandleOverflowCheckedBinop },
    { Opcode::kProjection, &LLVMIRBuilder::HandleProjection },
    { Opcode::kTuple, &LLVMIRBuilder::HandleTuple },
    { Opcode::kDidntThrow, &LLVMIRBuilder::HandleDidntThrow },
    { Opcode::kAbortCSADcheck, &LLVMIRBuilder::HandleAbortCSADcheck },
    { Opcode::kDebugBreak, &LLVMIRBuilder::HandleDebugBreak },
    { Opcode::kUnreachable, &LLVMIRBuilder::HandleUnreachable },
    { Opcode::kStackPointerGreaterThan, &LLVMIRBuilder::HandleStackPointerGreaterThan },
    { Opcode::kFrameConstant, &LLVMIRBuilder::HandleFrameConstant },
    { Opcode::kReturn, &LLVMIRBuilder::HandleReturn },
  };
}

void LLVMIRBuilder::InitDescriptorCallConvMap() {
  decscriptor2cc_ = {
    // {desc name, {no context reg cc, has context reg cc}}
    { "LoadWithVector Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8LoadWithVectorCallConv }},
    { "c-call", { LLVMCCallConv, LLVMCCallConv }},
    { "CallFunctionTemplateGeneric Descriptor", { LLVMInvalidCallConv,
        LLVMAArch64V8CallFunctionTemplateGenericCallConv }},
    { "CallApiCallbackOptimized Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8CallApiCallbackOptimizedCallConv}},
    { "EnumeratedKeyedLoad Descriptor", { LLVMInvalidCallConv, LLVMAArch64EnumeratedKeyedLoadCallConv }},
    { "CallTrampoline Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8CallTrampolineCallConv }},
    { "GlobalPrint", { LLVMInvalidCallConv, LLVMAArch64V8ToNameCallConv }},
    { "ThrowIteratorError", { LLVMInvalidCallConv, LLVMAArch64V8ToNameCallConv }},
    { "ThrowSymbolIteratorInvalid", { LLVMInvalidCallConv, LLVMAArch64V8ToNameCallConv }},
    { "ArrayIndexOf", { LLVMInvalidCallConv, LLVMAArch64V8ToNameCallConv }},
    { "CallApiCallbackGeneric Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8CallApiCallbackGenericCallConv }},
    { "Allocate Descriptor", { LLVMAArch64V8AllocateCallConv, LLVMInvalidCallConv }},
    { "BinaryOp_Baseline Descriptor", { LLVMAArch64V8AllocateCallConv, LLVMInvalidCallConv }},
    { "KeyedLoadWithVector Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8KeyedLoadWithVectorCallConv }},
    { "KeyedLoad Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8KeyedLoadWithVectorCallConv }},
    { "Compare_Baseline Descriptor", { LLVMAArch64V8AllocateCallConv, LLVMInvalidCallConv }},
    { "StringAdd_CheckNone Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8BigIntEqCallConv }},
    { "FastNewFunctionContextFunction Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8BigIntEqCallConv }},
    { "FastNewStrictArguments Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8BigIntEqCallConv }},
    { "GetOwnPropertyDescriptor Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8BigIntEqCallConv }},
    { "ForInEnumerate Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8BigIntEqCallConv }},
    { "ObjectToString Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8BigIntEqCallConv }},
    { "ArrayIsArray", { LLVMInvalidCallConv, LLVMAArch64V8BigIntEqCallConv }},
    { "ToString Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8BigIntEqCallConv }},
    { "OrdinaryGetOwnPropertyDescriptor Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8BigIntEqCallConv }},
    { "SetOrSetIteratorToList Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8BigIntEqCallConv }},
    { "EnqueueMicrotask Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8BigIntEqCallConv }},
    { "MapIteratorToList Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8BigIntEqCallConv }},
    { "StringToList Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8BigIntEqCallConv }},
    { "CloneFastJSArrayFillingHoles Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8BigIntEqCallConv }},
    { "ToInteger Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8BigIntEqCallConv }},
    { "AccessCheck", { LLVMInvalidCallConv, LLVMAArch64V8ToNameCallConv }},
    { "ForInEnumerate", { LLVMInvalidCallConv, LLVMAArch64V8ToNameCallConv }},
    { "LoadGlobalIC_Slow", { LLVMInvalidCallConv, LLVMAArch64V8ToNameCallConv }},
    { "LoadGlobalIC_Miss", { LLVMInvalidCallConv, LLVMAArch64V8ToNameCallConv }},
    { "GetOwnPropertyDescriptorObject", { LLVMInvalidCallConv, LLVMAArch64V8ToNameCallConv }},
    { "StoreInArrayLiteralIC_Slow", { LLVMInvalidCallConv, LLVMAArch64V8ToNameCallConv }},
    { "StringSubstring", { LLVMInvalidCallConv, LLVMAArch64V8ToNameCallConv }},
    { "CreateListFromArrayLike", { LLVMInvalidCallConv, LLVMAArch64V8ToNameCallConv }},
    { "ThrowApplyNonFunction", { LLVMInvalidCallConv, LLVMAArch64V8ToNameCallConv }},
    { "StringAdd", { LLVMInvalidCallConv, LLVMAArch64V8ToNameCallConv }},
    { "StringIndexOf Descriptor", { LLVMAArch64V8CLikeCallConv, LLVMInvalidCallConv }},
    { "NumberToString Descriptor", { LLVMAArch64V8CLikeCallConv, LLVMInvalidCallConv }},
    { "StringSlowFlatten Descriptor", { LLVMAArch64V8CLikeCallConv, LLVMInvalidCallConv }},
    { "Typeof Descriptor", { LLVMAArch64V8CLikeCallConv, LLVMInvalidCallConv }},
    { "ToName Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8BigIntEqCallConv }},
    { "ToObject Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8BigIntEqCallConv }},
    { "ForInFilter Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8BigIntEqCallConv }},
    { "StringCharCodeAt", { LLVMInvalidCallConv, LLVMAArch64V8ToNameCallConv }},
    { "ForInHasProperty", { LLVMInvalidCallConv, LLVMAArch64V8ToNameCallConv }},
    { "BigIntEqual Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8BigIntEqCallConv }},

    { "LoadPropertyWithInterceptor", { LLVMInvalidCallConv, LLVMAArch64V8ToNameCallConv }},
    { "ThrowReferenceError", { LLVMInvalidCallConv, LLVMAArch64V8ToNameCallConv }},
    { "ProxyGetProperty Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8ProxyGetPropertyCallConv }},
    { "ArrayIncludesSmiOrObject Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8ProxyGetPropertyCallConv }},
    { "ArrayIndexOfSmiOrObject Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8ProxyGetPropertyCallConv }},
    { "ArrayIndexOfHoleyDoubles Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8ProxyGetPropertyCallConv }},
    { "ArrayIndexOfPackedDoubles Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8ProxyGetPropertyCallConv }},
    { "ArrayIndexOfSmi Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8ProxyGetPropertyCallConv }},
    { "SetProperty Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8ProxyGetPropertyCallConv }},
    { "GetPropertyWithReceiver Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8ProxyGetPropertyCallConv }},
    { "CreateDataProperty Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8ProxyGetPropertyCallConv }},
    { "Compare_WithFeedback Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8ProxyGetPropertyCallConv }},
    { "SubString Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8ProxyGetPropertyCallConv }},
    { "WeakCollectionSet Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8ProxyGetPropertyCallConv }},
    { "GrowArrayElements Descriptor", { LLVMAArch64GrowArrayElementsCallConv, LLVMInvalidCallConv}},
    { "GrowArrayElements", { LLVMInvalidCallConv, LLVMAArch64V8ToNameCallConv }},
    { "StoreInArrayLiteralIC_Miss", { LLVMInvalidCallConv, LLVMAArch64V8ToNameCallConv }},
    { "ObjectAssignTryFastcase", { LLVMInvalidCallConv, LLVMAArch64V8ToNameCallConv }},
    { "WeakCollectionSet", { LLVMInvalidCallConv, LLVMAArch64V8ToNameCallConv }},
    { "LoadGlobalWithVector Descriptor", { LLVMInvalidCallConv, LLVMAArch64LoadGlobalWithVectorCallConv }},
    { "CreateDataProperty", { LLVMInvalidCallConv, LLVMAArch64LoadGlobalWithVectorCallConv }},
    { "LoadGlobalNoFeedback Descriptor", { LLVMInvalidCallConv, LLVMAArch64LoadGlobalWithVectorCallConv }},
    { "LoadBaseline Descriptor", { LLVMAArch64LoadBaselineCallConv, LLVMInvalidCallConv }},
    { "SetDataProperties Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8BigIntEqCallConv }},

    { "GetProperty", { LLVMInvalidCallConv, LLVMAArch64V8ToNameCallConv }},
    { "ApiGetter Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8ApiGetterCallConv }},
    { "LoadIC_Miss", { LLVMInvalidCallConv, LLVMAArch64V8ToNameCallConv }},
    { "LoadNoFeedback Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8LoadWithVectorCallConv }},
    { "CallFunctionTemplate Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8LoadWithVectorCallConv }},
    { "CallWithArrayLike Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8LoadWithVectorCallConv }},
    { "KeyedLoadIC_Miss", { LLVMInvalidCallConv, LLVMAArch64V8ToNameCallConv }},
    { "StringCharAt Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8BigIntEqCallConv }},
    { "InterpreterDispatch Descriptor", { LLVMAArch64InterpreterNoContextCallConv,
        LLVMAArch64InterpreterDispatchCallConv }},
    { "StoreWithVector Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8StoreWithVectorCallConv} },
    { "StorePropertyWithInterceptor", { LLVMInvalidCallConv, LLVMAArch64V8CallTrampolineCallConv } },
    { "ProxySetProperty Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8ProxyGetPropertyCallConv } },
    { "SharedValueBarrierSlow", { LLVMInvalidCallConv, LLVMAArch64V8CallTrampolineCallConv } },
    { "StoreCallbackProperty", { LLVMInvalidCallConv, LLVMAArch64V8CallTrampolineCallConv } },
    { "KeyedStoreIC_Slow", { LLVMInvalidCallConv, LLVMAArch64V8CallTrampolineCallConv } },
    { "AddDictionaryProperty", { LLVMInvalidCallConv, LLVMAArch64V8CallTrampolineCallConv } },
    { "StoreNoFeedback Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8LoadWithVectorCallConv } },
    { "StoreIC_Miss", { LLVMInvalidCallConv, LLVMAArch64V8CallTrampolineCallConv } },
    { "DebugPrint", { LLVMInvalidCallConv, LLVMAArch64V8CallTrampolineCallConv } },
    { "KeyedStoreIC_Miss", { LLVMInvalidCallConv, LLVMAArch64V8CallTrampolineCallConv } },
    { "ThrowTypeErrorIfStrict", { LLVMInvalidCallConv, LLVMAArch64V8CallTrampolineCallConv } },
    { "Abort", { LLVMInvalidCallConv, LLVMAArch64V8CallTrampolineCallConv } },
    { "FatalProcessOutOfMemoryInvalidArrayLength", { LLVMInvalidCallConv, LLVMAArch64V8CallTrampolineCallConv } },
    { "FatalProcessOutOfMemoryInAllocateRaw", { LLVMInvalidCallConv, LLVMAArch64V8CallTrampolineCallConv } },
    { "AllocateInYoungGeneration", { LLVMInvalidCallConv, LLVMAArch64V8CallTrampolineCallConv } },
    { "SetKeyedProperty", { LLVMInvalidCallConv, LLVMAArch64V8CallTrampolineCallConv } },
    { "GetPropertyWithReceiver", { LLVMInvalidCallConv, LLVMAArch64V8CallTrampolineCallConv } },
    { "SetPropertyWithReceiver", { LLVMInvalidCallConv, LLVMAArch64V8CallTrampolineCallConv } },
    { "StoreTransition Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8StoreTransitionCallConv } },
    { "LoadNoFeedbackIC_Miss", { LLVMInvalidCallConv, LLVMAArch64V8CallTrampolineCallConv } },
    { "Load Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8LoadWithVectorCallConv } },
    { "KeyedHasICWithVector Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8ProxyGetPropertyCallConv } },
    { "CreateShallowArrayLiteral Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8ProxyGetPropertyCallConv } },
    { "ProxyHasProperty Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8ProxyGetPropertyCallConv } },
    { "HasProperty", { LLVMInvalidCallConv, LLVMAArch64V8ToNameCallConv } },
    { "GetProperty Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8ProxyGetPropertyCallConv } },
    { "FindOrderedHashSetEntry Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8BigIntEqCallConv } },
    { "BigIntEqualToBigInt", { LLVMInvalidCallConv, LLVMAArch64V8CallTrampolineCallConv } },
    { "PrintWithNameForAssert", { LLVMInvalidCallConv, LLVMAArch64V8CallTrampolineCallConv } },
    { "ToLength Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8BigIntEqCallConv }},
    { "ArrayForEachLoopContinuation Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8ProxyGetPropertyCallConv }},
    { "ThrowCalledNonCallable", { LLVMInvalidCallConv, LLVMAArch64V8ToNameCallConv }},
    { "ThrowTypeError", { LLVMInvalidCallConv, LLVMAArch64V8ToNameCallConv }},
    { "CreateArrayLiteral", { LLVMInvalidCallConv, LLVMAArch64V8ToNameCallConv }},
    { "ConstructStub Descriptor", { LLVMInvalidCallConv, LLVMAArch64ConstructStubCallConv }},
    { "CallVarargs Descriptor", { LLVMInvalidCallConv, LLVMAArch64CallVarargsCallConv }},
    { "CreateEmptyLiteralObject Descriptor", { LLVMInvalidCallConv, LLVMAArch64CallVarargsCallConv }},
#ifdef V8_ENABLE_LEAPTIERING
    { "js-call", { LLVMInvalidCallConv, LLVMAArch64JSCallLeapTireCallConv }},
#else
    { "js-call", { LLVMInvalidCallConv, LLVMAArch64JSCallCallConv }},
#endif
    { "StackGuard", { LLVMInvalidCallConv, LLVMAArch64V8ToNameCallConv }},
    { "StringEqual Descriptor", { LLVMAArch64V8CLikeCallConv, LLVMInvalidCallConv } },
    { "FastNewClosure Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8BigIntEqCallConv } },
    { "DefineNamedOwnIC_Slow", { LLVMInvalidCallConv, LLVMAArch64V8CallTrampolineCallConv } },
    { "DefineNamedOwnIC_Miss", { LLVMInvalidCallConv, LLVMAArch64V8CallTrampolineCallConv } },
    { "CreateShallowObjectLiteral Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8ProxyGetPropertyCallConv } },
    { "CreateObjectLiteral", { LLVMInvalidCallConv, LLVMAArch64V8CallTrampolineCallConv } },
    { "FunctionLogNextExecution", { LLVMInvalidCallConv, LLVMAArch64V8CallTrampolineCallConv } },
    { "CompileOptimized", { LLVMInvalidCallConv, LLVMAArch64V8CallTrampolineCallConv } },
    { "InstallSFICode", { LLVMInvalidCallConv, LLVMAArch64V8CallTrampolineCallConv } },
    { "CompileLazy", { LLVMInvalidCallConv, LLVMAArch64V8CallTrampolineCallConv } },
    { "HealOptimizedCodeSlot", { LLVMInvalidCallConv, LLVMAArch64V8CallTrampolineCallConv } },
    { "InstallBaselineCode", { LLVMInvalidCallConv, LLVMAArch64V8CallTrampolineCallConv } },
    { "StringEqual", { LLVMInvalidCallConv, LLVMAArch64V8CallTrampolineCallConv } },
    { "IncrementUseCounter", { LLVMInvalidCallConv, LLVMAArch64V8CallTrampolineCallConv } },
    { "NonPrimitiveToPrimitive_Default Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8BigIntEqCallConv } },
    { "TransitionElementsKindWithKind", { LLVMInvalidCallConv, LLVMAArch64V8CallTrampolineCallConv } },
    { "ExtractFastJSArray Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8ProxyGetPropertyCallConv } },
    { "ArraySpeciesConstructor", { LLVMInvalidCallConv, LLVMAArch64V8CallTrampolineCallConv } },
    { "HasProperty Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8BigIntEqCallConv } },
    { "FastCreateDataProperty Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8ProxyGetPropertyCallConv } },
    { "DeleteProperty Descriptor", { LLVMInvalidCallConv, LLVMAArch64V8ProxyGetPropertyCallConv } },
  };
}

LLVMTypeRef LLVMIRBuilder::ConvertLLVMTypeFromRep(Rep rep) {
  switch (rep.value()) {
    case Rep::Enum::kWord32: return GetInt32T();
    case Rep::Enum::kWord64: return GetInt64T();
    case Rep::Enum::kFloat32: return GetFloatT();
    case Rep::Enum::kFloat64: return GetDoubleT();
    case Rep::Enum::kTagged: return GetTaggedHPtrT();
    case Rep::Enum::kCompressed: return GetTaggedHPtrT();
    default: UNREACHABLE();
  }
}

std::string LLVMIRBuilder::NodeName(OpIndex node) {
  const turboshaft::Operation& op = graph_.Get(node);
  return std::string(OpcodeName(op.opcode)) + "_" + std::to_string(node.id());
}

void LLVMIRBuilder::CompletePendingPhis() {
  for (auto item : pending_phis_) {
    OpIndex phi = item.first;
    Block* block = item.second;
    const turboshaft::Operation& op = graph_.Get(phi);
    CHECK(op.opcode == Opcode::kPhi);
    const int input_count = op.input_count;
    LLVMValueRef llvm_phi = GetLValueOf(phi);

    for (int i = 0; i < input_count; ++i) {
      OpIndex input = op.input(i);
      LLVMBasicBlockRef lbb = GetEndLBlockOf(block->Predecessors()[i]);
      LLVMValueRef val = FixTypeTo(GetLValueOf(input), LLVMTypeOf(llvm_phi), lbb);
      LLVMAddIncoming(llvm_phi, &val, &lbb, 1);
    }
  }
}

uint32_t LLVMIRBuilder::GetPtrAddressSpace(LLVMValueRef value) const {
  LLVMTypeRef type = LLVMTypeOf(value);
  if (LLVMGetTypeKind(type) == LLVMPointerTypeKind) {
    return LLVMGetPointerAddressSpace(type);
  }
  return 0;
}

LLVMValueRef LLVMIRBuilder::CanonicalizeToPtr(LLVMValueRef value, LLVMTypeRef ptr_type) const {
  LLVMTypeRef value_type = LLVMTypeOf(value);
  if (LLVMGetTypeKind(value_type) == LLVMPointerTypeKind) {
      if (value_type != ptr_type) {
          return LLVMBuildPointerCast(builder_, value, ptr_type, "");
      }
  } else if (LLVMGetTypeKind(value_type) == LLVMIntegerTypeKind) {
      LLVMValueRef result = LLVMBuildIntToPtr(builder_, value, ptr_type, "");
      return result;
  } else {
    UNIMPLEMENTED();
  }
  return value;
}

LLVMValueRef LLVMIRBuilder::FixTypeTo(LLVMValueRef value, LLVMTypeRef to_type, LLVMBasicBlockRef insert) {
  LLVMTypeRef from_type = LLVMTypeOf(value);
  if (from_type == to_type) {
    return value;
  }

  LLVMTypeKind from_kind = LLVMGetTypeKind(from_type);
  LLVMTypeKind to_kind = LLVMGetTypeKind(to_type);

  if (insert != nullptr) {
    LLVMPositionBuilderBefore(builder_, LLVMGetLastInstruction(insert));
  }
  CHECK_NOT_NULL(current_lbb_);

  if (from_kind == LLVMIntegerTypeKind && to_kind == LLVMIntegerTypeKind) {
    uint32_t from_width = LLVMGetIntTypeWidth(from_type);
    uint32_t to_width = LLVMGetIntTypeWidth(to_type);
    if (to_width > from_width) {
        value = LLVMBuildZExt(builder_, value, to_type, "");
    } else {
        CHECK(to_width < from_width);
        value = LLVMBuildTrunc(builder_, value, to_type, "");
    }

    goto END;
  }

  if (from_kind == LLVMPointerTypeKind && to_kind == LLVMIntegerTypeKind) {
    value = LLVMBuildPtrToInt(builder_, value, GetInt64T(), "");
    uint32_t from_width = LLVMGetIntTypeWidth(GetInt64T());
    uint32_t to_width = LLVMGetIntTypeWidth(to_type);
    if (to_width == from_width) {
    } else {
        CHECK(to_width < from_width);
        value = LLVMBuildTrunc(builder_, value, to_type, "");
    }

    goto END;
  }

  if (from_kind == LLVMIntegerTypeKind && to_kind == LLVMPointerTypeKind) {
    uint32_t from_width = LLVMGetIntTypeWidth(from_type);
    uint32_t to_width = LLVMGetIntTypeWidth(GetInt64T());
    if (from_width < to_width) {
        value = LLVMBuildZExt(builder_, value, GetInt64T(), "");
    } 

    value = CanonicalizeToPtr(value, to_type);

    goto END;
  }

  if (from_kind == LLVMPointerTypeKind && to_kind == LLVMPointerTypeKind) {
    value = LLVMBuildPointerCast(builder_, value, to_type, "");

    goto END;
  }

  UNIMPLEMENTED();

END:
  if (insert != nullptr) {
    LLVMPositionBuilderAtEnd(builder_, current_lbb_);
  }
  return value;
}

void LLVMIRBuilder::SetGCLeafFunction(LLVMValueRef call) {
  const char *attrName = "gc-leaf-function";
  const char *attrValue = "true";
  LLVMAttributeRef llvmAttr = LLVMCreateStringAttribute(context_, attrName, static_cast<uint32_t>(strlen(attrName)),
                                                        attrValue, static_cast<uint32_t>(strlen(attrValue)));
  LLVMAddCallSiteAttribute(call, LLVMAttributeFunctionIndex, llvmAttr);
}

void LLVMIRBuilder::AddBranchWeight(LLVMValueRef branch, uint32_t true_weight, uint32_t false_weight) {
  LLVMMetadataRef branch_weights = LLVMMDStringInContext2(context_, "branch_weights", 14);
  LLVMMetadataRef true_w = LLVMValueAsMetadata(LLVMConstInt(GetInt32T(), true_weight, 0));
  LLVMMetadataRef false_w = LLVMValueAsMetadata(LLVMConstInt(GetInt32T(), false_weight, 0));
  LLVMMetadataRef mds[] = {branch_weights, true_w, false_w};
  LLVMMetadataRef metadata = LLVMMDNodeInContext2(context_, mds, 3);
  LLVMValueRef metadata_value = LLVMMetadataAsValue(context_, metadata);
  LLVMSetMetadata(branch, LLVMGetMDKindID("prof", 4), metadata_value); //4:length of "prof"
}

// ==========================  LLVMIRBuilder Node Helper  =====================
Constant LLVMIRBuilder::ToConstant(const ConstantOp& constant) {
  using Kind = turboshaft::ConstantOp::Kind;
  switch (constant.kind) {
    case Kind::kWord32:
      return Constant(static_cast<int32_t>(constant.word32()));
    case Kind::kWord64:
      return Constant(static_cast<int64_t>(constant.word64()));
    case Kind::kSmi:
      if constexpr (Is64()) {
        return Constant(static_cast<int64_t>(constant.smi().ptr()));
      } else {
        return Constant(static_cast<int32_t>(constant.smi().ptr()));
      }
    case Kind::kHeapObject:
    case Kind::kCompressedHeapObject:
    case Kind::kTrustedHeapObject:
      return Constant(constant.handle(),
                      constant.kind == Kind::kCompressedHeapObject);
    case Kind::kExternal:
      return Constant(constant.external_reference());
    case Kind::kNumber:
      return Constant(constant.number());
    case Kind::kFloat32:
      return Constant(constant.float32());
    case Kind::kFloat64:
      return Constant(constant.float64());
    case Kind::kTaggedIndex: {
      // Unencoded index value.
      intptr_t value = static_cast<intptr_t>(constant.tagged_index());
      CHECK(TaggedIndex::IsValid(value));
      // Generate it as 32/64-bit constant in a tagged form.
      Address tagged_index = TaggedIndex::FromIntptr(value).ptr();
      if (kSystemPointerSize == kInt32Size) {
        return Constant(static_cast<int32_t>(tagged_index));
      } else {
        return Constant(static_cast<int64_t>(tagged_index));
      }
    }
    case Kind::kRelocatableWasmCall:
    case Kind::kRelocatableWasmStubCall: {
      uint64_t value = constant.integral();
      auto mode = constant.kind == Kind::kRelocatableWasmCall
                      ? RelocInfo::WASM_CALL
                      : RelocInfo::WASM_STUB_CALL;
      using constant_type = std::conditional_t<Is64(), int64_t, int32_t>;
      return Constant(RelocatablePtrConstantInfo(
          base::checked_cast<constant_type>(value), mode));
    }
    case Kind::kRelocatableWasmCanonicalSignatureId:
      return Constant(RelocatablePtrConstantInfo(
          base::checked_cast<int32_t>(constant.integral()),
          RelocInfo::WASM_CANONICAL_SIG_ID));
    case Kind::kRelocatableWasmIndirectCallTarget:
      uint64_t value = constant.integral();
      using constant_type = 
          std::conditional_t<V8_ENABLE_WASM_CODE_POINTER_TABLE_BOOL ||
                                 !Is64(),
                             int32_t, int64_t>;
      return Constant(RelocatablePtrConstantInfo(
          base::checked_cast<constant_type>(value),
          RelocInfo::WASM_INDIRECT_CALL_TARGET));
  }
  UNREACHABLE();
}

bool LLVMIRBuilder::IsMaterializableFromRoot(Handle<HeapObject> object, RootIndex* index_return) {
  const CallDescriptor* incoming_descriptor = 
      linkage_->GetIncomingDescriptor();
  if (incoming_descriptor->flags() & CallDescriptor::kCanUseRoots) {
    return data_->isolate()->roots_table().IsRootHandle(object, index_return) &&
           RootsTable::IsImmortalImmovable(*index_return);
  }
  return false;
}


// ==========================  LLVMIRBuilder IR Transform  ==========================
void LLVMIRBuilder::HandleParameter(OpIndex node) {
  VisitParameter(node);
}

void LLVMIRBuilder::VisitParameter(OpIndex node) {
  const turboshaft::Operation& op = graph_.Get(node);
  int index = op.Cast<turboshaft::ParameterOp>().parameter_index;

  LLVMValueRef param = GetLLVMParamByTurboshaftParamIdx(index);
  CHECK(param != nullptr);

  MachineType type = linkage_->GetParameterType(index);
  CHECK(LLVMTypeOf(param) == ConvertLLVMTypeFromMachineType(type));

  Bind(node, param);
}

void LLVMIRBuilder::HandleTaggedBitcast(OpIndex node) {
  VisitTaggedBitcast(node);
}

void LLVMIRBuilder::VisitTaggedBitcast(OpIndex node) {
  const turboshaft::Operation& op = graph_.Get(node);
  const TaggedBitcastOp& cast = op.Cast<TaggedBitcastOp>();

  LLVMValueRef from = GetLValueOf(cast.input());
  LLVMValueRef to = nullptr;
  switch (multi(cast.from, cast.to)) {
    case multi(Rep::Tagged(), Rep::Word64()): {
      to = LLVMBuildPtrToInt(builder_, from, GetInt64T(), NodeName(node).c_str());
      break;
    }
    case multi(Rep::Word64(), Rep::Tagged()): {
      to = LLVMBuildIntToPtr(builder_, from, GetTaggedHPtrT(), NodeName(node).c_str());
      break;
    }
    case multi(Rep::Compressed(), Rep::Word32()): {
      if (LLVMTypeOf(from) == GetInt32T()) {
        to = from;
      } else if (LLVMTypeOf(from) == GetTaggedHPtrT()) {
        to = LLVMBuildPtrToInt(builder_, from, GetInt64T(), "");
        to = LLVMBuildTrunc(builder_, to, GetInt32T(), NodeName(node).c_str());
      } else if (LLVMTypeOf(from) == GetInt64T()) {
        to = LLVMBuildTrunc(builder_, from, GetInt32T(), NodeName(node).c_str());
      } else {
        UNREACHABLE();
      }
      break;
    }
    default:
      UNIMPLEMENTED();
  }

  Bind(node, to);
}

void LLVMIRBuilder::HandleConstant(OpIndex node) {
  VisitConstant(node);
}

void LLVMIRBuilder::VisitConstant(OpIndex node) {
  const turboshaft::Operation& op = graph_.Get(node);
  const ConstantOp& constant = op.Cast<ConstantOp>();

  Constant cons = ToConstant(constant);
  if (cons.type() == Constant::kHeapObject) {
    Handle<HeapObject> src_object = cons.ToHeapObject();
    RootIndex index;
    if (IsMaterializableFromRoot(src_object, &index)) {
      if (V8_STATIC_ROOTS_BOOL && RootsTable::IsReadOnly(index) &&
          v8::internal::Assembler::IsImmAddSub(MacroAssemblerBase::ReadOnlyRootPtr(index, data_->isolate()))) {
        uint32_t imm = MacroAssemblerBase::ReadOnlyRootPtr(index, data_->isolate());
        LLVMValueRef immediate = LLVMConstInt(GetInt64T(), imm, 0);
        LLVMValueRef l_constant = LLVMBuildAdd(builder_, GetPurePtrComprCageBase(), immediate, NodeName(node).c_str());
        Bind(node, l_constant);
        return;
      }
      // Many roots have addresses that are too large to fit into addition immediate
      // operands. Evidence suggests that the extra instruction for decompression
      // costs us more than the load.
      int32_t offset = MacroAssemblerBase::RootRegisterOffsetForRootIndex(index);
      LLVMValueRef l_offset = LLVMConstInt(GetInt64T(), offset, 1);
      LLVMValueRef l_address = LLVMBuildGEP2(builder_, GetInt8T(), GetRoot(), &l_offset, 1, "");
      LLVMValueRef l_constant = LLVMBuildLoad2(builder_, GetInt64T(), l_address, NodeName(node).c_str());
      Bind(node, l_constant);
      return;
    } else {
      CHECK(data_->isolate()->IsGeneratingEmbeddedBuiltins());
      RootIndex root_index;
      if (data_->isolate()->roots_table().IsRootHandle(src_object, &root_index)) {
        UNIMPLEMENTED();
      }
      if (IsCode(*src_object)) {
        Builtin builtin = Builtin::kNoBuiltinId;
        if (!data_->isolate()->builtins()->IsBuiltinHandle(cons.ToCode(), &builtin)) {
          UNIMPLEMENTED();
        }
        uint32_t offset = IsolateData::BuiltinEntrySlotOffset(builtin);
        LLVMValueRef l_offset = LLVMConstInt(GetInt64T(), offset, 0);
        LLVMValueRef l_address = LLVMBuildGEP2(builder_, GetInt8T(), GetRoot(), &l_offset, 1, "");
        LLVMValueRef l_constant = LLVMBuildLoad2(builder_, GetInt64T(), l_address, NodeName(node).c_str());
        Bind(node, l_constant);
        return;
      }
      // Ensure the given object is in the builtins constants table and fetch its
      // index.
      BuiltinsConstantsTableBuilder* builder = 
          data_->isolate()->builtins_constants_table_builder();
      uint32_t index = builder->AddObject(src_object);

      // Slow load from the constant table
      CHECK(RootsTable::IsImmortalImmovable(RootIndex::kBuiltinsConstantsTable));

      int32_t builder_offset = MacroAssemblerBase::RootRegisterOffsetForRootIndex(RootIndex::kBuiltinsConstantsTable);
      LLVMValueRef l_builder_offset = LLVMConstInt(GetInt64T(), builder_offset, 1);
      LLVMValueRef l_builder_address = LLVMBuildGEP2(builder_, GetInt8T(), GetRoot(),
                                                     &l_builder_offset, 1, "");
      LLVMValueRef l_builder = LLVMBuildLoad2(builder_, GetTaggedHPtrT(), l_builder_address, "");

      index = FixedArray::OffsetOfElementAt(index) - kHeapObjectTag;
      LLVMValueRef l_object_offset = LLVMConstInt(GetInt64T(), index, 1);
      LLVMValueRef l_constant = nullptr;
      if (COMPRESS_POINTERS_BOOL) {
        LLVMValueRef l_object_address = LLVMBuildGEP2(builder_, GetInt8T(), l_builder,
                                                      &l_object_offset, 1, "");
        l_object_address = CanonicalizeToPtr(l_object_address, LLVMPointerType(GetInt32T(), 1));
        l_constant = LLVMBuildLoad2(builder_, GetInt32T(), l_object_address, "");
        l_constant = LLVMBuildZExt(builder_, l_constant, GetInt64T(), "");
        l_constant = LLVMBuildAdd(builder_, GetPurePtrComprCageBase(), l_constant, NodeName(node).c_str());
      } else {
        LLVMValueRef l_object_address = LLVMBuildGEP2(builder_, GetInt8T(), l_builder,
                                                      &l_object_offset, 1, "");
        l_constant = LLVMBuildLoad2(builder_, GetInt64T(), l_object_address, NodeName(node).c_str());                                              
      }

      Bind(node, l_constant);
      return;
    }
  } else if (cons.type() == Constant::kCompressedHeapObject) {
    Handle<HeapObject> src_object = cons.ToHeapObject();
    RootIndex index;
    LLVMValueRef l_constant = nullptr;
    if (IsMaterializableFromRoot(src_object, &index)) {
      if (V8_STATIC_ROOTS_BOOL && RootsTable::IsReadOnly(index)) {
        uint32_t imm = MacroAssemblerBase::ReadOnlyRootPtr(index, data_->isolate());
        l_constant = LLVMConstInt(GetInt32T(), imm, 0);
      } else {
        int32_t offset = MacroAssemblerBase::RootRegisterOffsetForRootIndex(index);
        LLVMValueRef l_offset = LLVMConstInt(GetInt64T(), offset, 1);
        LLVMValueRef l_address = LLVMBuildGEP2(builder_, GetInt8T(), GetRoot(), &l_offset, 1, "");
        l_constant = LLVMBuildLoad2(builder_, GetInt32T(), l_address, NodeName(node).c_str());
      }
    } else {
      CHECK(data_->isolate()->IsGeneratingEmbeddedBuiltins());
      if (data_->isolate()->roots_table().IsRootHandle(src_object, &index)) {
        UNIMPLEMENTED();
      }
      if (IsCode(*src_object)) {
        Builtin builtin = Builtin::kNoBuiltinId;
        if (!data_->isolate()->builtins()->IsBuiltinHandle(src_object, &builtin)) {
          UNIMPLEMENTED();
        }
        uint32_t offset = IsolateData::BuiltinEntrySlotOffset(builtin);
        LLVMValueRef l_offset = LLVMConstInt(GetInt64T(), offset, 0);
        LLVMValueRef l_address = LLVMBuildGEP2(builder_, GetInt8T(), GetRoot(), &l_offset, 1, "");
        LLVMValueRef l_constant = LLVMBuildLoad2(builder_, GetInt64T(), l_address, NodeName(node).c_str());
        Bind(node, l_constant);
        return;
      }
      UNIMPLEMENTED();
    }
    Bind(node, l_constant);
    return;
  } else if (cons.type() == Constant::kExternalReference) {
    ExternalReference ref = cons.ToExternalReference();
    if (ref.IsIsolateFieldId()) {
      int32_t offset = ref.offset_from_root_register();
      LLVMValueRef l_offset = LLVMConstInt(GetInt64T(), offset, 1);
      LLVMValueRef l_constant = LLVMBuildAdd(builder_, GetPureRoot(), l_offset, NodeName(node).c_str());
      Bind(node, l_constant);
      return;
    }
    if (MacroAssemblerBase::IsAddressableThroughRootRegister(data_->isolate(), ref)) {
      // Some external references can be efficiently loaded as an offset from
      // kRootRegister.
      intptr_t offset = 
          MacroAssemblerBase::RootRegisterOffsetForExternalReference(data_->isolate(), ref);
      LLVMValueRef l_offset = LLVMConstInt(GetInt64T(), offset, 1);
      LLVMValueRef l_constant = LLVMBuildAdd(builder_, GetPureRoot(), l_offset, NodeName(node).c_str());
      Bind(node, l_constant);
      return;
    } else {
      // Otherwise, do a memory load from the external reference table.
      int32_t offset = 
          MacroAssemblerBase::RootRegisterOffsetForExternalReferenceTableEntry(data_->isolate(), ref);
      LLVMValueRef l_object_offset = LLVMConstInt(GetInt64T(), offset, 1);
      LLVMValueRef l_object_address = LLVMBuildGEP2(builder_, GetInt8T(), GetRoot(),
                                                      &l_object_offset, 1, "");
      LLVMValueRef l_constant = LLVMBuildLoad2(builder_, GetInt64T(), l_object_address, NodeName(node).c_str());
      Bind(node, l_constant);
      return;
    }
  } else if (cons.type() == Constant::kInt32) {
    LLVMValueRef l_constant = LLVMConstInt(GetInt32T(), cons.ToInt32(), 0);
    Bind(node, l_constant);
  } else if (cons.type() == Constant::kInt64) {
    LLVMValueRef l_constant = LLVMConstInt(GetInt64T(), cons.ToInt64(), 0);
    Bind(node, l_constant);
  } else if (cons.type() == Constant::kFloat32) {
    LLVMValueRef l_constant = LLVMConstReal(GetFloatT(), cons.ToFloat32());
    Bind(node, l_constant);
  } else if (cons.type() == Constant::kFloat64) {
    LLVMValueRef l_constant = LLVMConstReal(GetDoubleT(), cons.ToFloat64().value());
    Bind(node, l_constant);
  } else {
    UNIMPLEMENTED();
  }
}

void LLVMIRBuilder::HandleLoad(OpIndex node) {
  VisitLoad(node);
}

void LLVMIRBuilder::VisitLoad(OpIndex node) {
  const turboshaft::Operation& op = graph_.Get(node);
  const LoadOp& load = op.Cast<LoadOp>();

  MemoryRepresentation loaded_rep = load.loaded_rep;
  RegisterRepresentation result_rep = load.result_rep;
  LLVMValueRef l_load = nullptr;
  
  LLVMValueRef base = GetLValueOf(load.base());
  OpIndex index = load.index().value();
  if (index.valid()) {
    if (LLVMGetTypeKind(LLVMTypeOf(base)) == LLVMPointerTypeKind) {
      base = LLVMBuildPtrToInt(builder_, base, GetInt64T(), "");
    }
    base = LLVMBuildAdd(builder_, base, GetLValueOf(index), "");
  }

  switch (loaded_rep) {
    case MemoryRepresentation::Int8():
    case MemoryRepresentation::Uint8(): {
      CHECK_EQ(result_rep, RegisterRepresentation::Word32());
      LLVMTypeRef memory_type = LLVMPointerType(GetInt8T(), GetPtrAddressSpace(base));
      base = CanonicalizeToPtr(base, memory_type);
      l_load = LLVMBuildLoad2(builder_, GetInt8T(), base, "");
      if (load.kind.is_atomic) {
        LLVMSetOrdering(l_load, LLVMAtomicOrderingAcquire);
      }
      if (loaded_rep == MemoryRepresentation::Int8()) { 
        l_load = LLVMBuildSExt(builder_, l_load, GetInt32T(), NodeName(node).c_str());
      } else {
        l_load = LLVMBuildZExt(builder_, l_load, GetInt32T(), NodeName(node).c_str());
      }
      Bind(node, l_load);
      return;
    }
    case MemoryRepresentation::Int16():
    case MemoryRepresentation::Uint16(): {
      CHECK_EQ(result_rep, RegisterRepresentation::Word32());
      LLVMTypeRef memory_type = LLVMPointerType(GetInt16T(), GetPtrAddressSpace(base));
      base = CanonicalizeToPtr(base, memory_type);
      l_load = LLVMBuildLoad2(builder_, GetInt16T(), base, "");
      if (load.kind.is_atomic) {
        LLVMSetOrdering(l_load, LLVMAtomicOrderingAcquire);
      }
      if (loaded_rep == MemoryRepresentation::Int16()) {
        l_load = LLVMBuildSExt(builder_, l_load, GetInt32T(), NodeName(node).c_str());
      } else {
        l_load = LLVMBuildZExt(builder_, l_load, GetInt32T(), NodeName(node).c_str());
      }
      Bind(node, l_load);
      return;
    }
    case MemoryRepresentation::Int32():
    case MemoryRepresentation::Uint32(): {
      CHECK_EQ(result_rep, RegisterRepresentation::Word32());
      LLVMTypeRef memory_type = LLVMPointerType(GetInt32T(), GetPtrAddressSpace(base));
      base = CanonicalizeToPtr(base, memory_type);
      l_load = LLVMBuildLoad2(builder_, GetInt32T(), base, NodeName(node).c_str());
      if (load.kind.is_atomic) {
        LLVMSetOrdering(l_load, LLVMAtomicOrderingAcquire);
      }
      Bind(node, l_load);
      return;
    }
    case MemoryRepresentation::Int64():
    case MemoryRepresentation::Uint64(): {
      CHECK_EQ(result_rep, RegisterRepresentation::Word64());
      LLVMTypeRef memory_type = LLVMPointerType(GetInt64T(), GetPtrAddressSpace(base));
      base = CanonicalizeToPtr(base, memory_type);
      l_load = LLVMBuildLoad2(builder_, GetInt64T(), base, NodeName(node).c_str());
      if (load.kind.is_atomic) {
        LLVMSetOrdering(l_load, LLVMAtomicOrderingAcquire);
      }
      Bind(node, l_load);
      return;
    }
    case MemoryRepresentation::Float16(): {
      CHECK_EQ(result_rep, RegisterRepresentation::Float32());
      LLVMTypeRef memory_type = LLVMPointerType(GetHalfT(), GetPtrAddressSpace(base));
      base = CanonicalizeToPtr(base, memory_type);
      l_load = LLVMBuildLoad2(builder_, GetHalfT(), base, NodeName(node).c_str());
      if (load.kind.is_atomic) {
        UNREACHABLE();
      }
      Bind(node, l_load);
      return;
    }
    case MemoryRepresentation::Float32(): {
      CHECK_EQ(result_rep, RegisterRepresentation::Float32());
      LLVMTypeRef memory_type = LLVMPointerType(GetFloatT(), GetPtrAddressSpace(base));
      base = CanonicalizeToPtr(base, memory_type);
      l_load = LLVMBuildLoad2(builder_, GetFloatT(), base, NodeName(node).c_str());
      if (load.kind.is_atomic) {
        UNREACHABLE();
      }
      Bind(node, l_load);
      return;
    }
    case MemoryRepresentation::Float64(): {
      CHECK_EQ(result_rep, RegisterRepresentation::Float64());
      LLVMTypeRef memory_type = LLVMPointerType(GetDoubleT(), GetPtrAddressSpace(base));
      base = CanonicalizeToPtr(base, memory_type);
      l_load = LLVMBuildLoad2(builder_, GetDoubleT(), base, NodeName(node).c_str());
      if (load.kind.is_atomic) {
        UNREACHABLE();
      }
      Bind(node, l_load);
      return;
    }
#ifdef V8_COMPRESS_POINTERS
    case MemoryRepresentation::AnyTagged():
    case MemoryRepresentation::TaggedPointer(): {
      LLVMTypeRef memory_type = LLVMPointerType(GetInt32T(), GetPtrAddressSpace(base));
      base = CanonicalizeToPtr(base, memory_type);
      if (result_rep == RegisterRepresentation::Compressed()) {
        l_load = LLVMBuildLoad2(builder_, GetInt32T(), base, "");
        l_load = LLVMBuildZExt(builder_, l_load, GetInt64T(), "");
        l_load = LLVMBuildIntToPtr(builder_, l_load, GetTaggedHPtrT(), NodeName(node).c_str());
        if (load.kind.is_atomic) {
          UNREACHABLE();
        }
        Bind(node, l_load);
        return;
      }
      CHECK_EQ(result_rep, RegisterRepresentation::Tagged());
      l_load = LLVMBuildLoad2(builder_, GetInt32T(), base, "");
      if (load.kind.is_atomic) {
        UNREACHABLE();
      }
      l_load = LLVMBuildZExt(builder_, l_load, GetInt64T(), "");
      l_load = LLVMBuildAdd(builder_, l_load, GetPurePtrComprCageBase(), "");
      l_load = LLVMBuildIntToPtr(builder_, l_load, GetTaggedHPtrT(), NodeName(node).c_str());
      Bind(node, l_load);
      return;
    }
    case MemoryRepresentation::TaggedSigned(): {
      LLVMTypeRef memory_type = LLVMPointerType(GetInt32T(), GetPtrAddressSpace(base));
      base = CanonicalizeToPtr(base, memory_type);
      if (load.kind.is_atomic) {
        UNREACHABLE();
      }
      if (result_rep == RegisterRepresentation::Compressed()) {
        l_load = LLVMBuildLoad2(builder_, GetInt32T(), base, NodeName(node).c_str());
        Bind(node, l_load);
        return;
      }
      CHECK_EQ(result_rep, RegisterRepresentation::Tagged());
      l_load = LLVMBuildLoad2(builder_, GetInt32T(), base, "");
      l_load = LLVMBuildZExt(builder_, l_load, GetInt64T(), "");
      l_load = LLVMBuildIntToPtr(builder_, l_load, GetTaggedHPtrT(), NodeName(node).c_str());
      Bind(node, l_load);
      return;
    }
#else
    case MemoryRepresentation::AnyTagged():
    case MemoryRepresentation::TaggedPointer():
    case MemoryRepresentation::TaggedSigned(): {
      LLVMTypeRef memory_type = LLVMPointerType(GetTaggedHPtrT(), GetPtrAddressSpace(base));
      base = CanonicalizeToPtr(base, memory_type);
      l_load = LLVMBuildLoad2(builder_, GetTaggedHPtrT(), base, NodeName(node).c_str());
      if (load.kind.is_atomic) {
        UNREACHABLE();
      }
      Bind(node, l_load);
      return;
    }
#endif
    case MemoryRepresentation::AnyUncompressedTagged():
    case MemoryRepresentation::UncompressedTaggedPointer():
    case MemoryRepresentation::UncompressedTaggedSigned(): {
      CHECK_EQ(result_rep, RegisterRepresentation::Tagged());
      LLVMTypeRef memory_type = LLVMPointerType(GetTaggedHPtrT(), GetPtrAddressSpace(base));
      base = CanonicalizeToPtr(base, memory_type);
      l_load = LLVMBuildLoad2(builder_, GetTaggedHPtrT(), base, NodeName(node).c_str());
      if (load.kind.is_atomic) {
        UNREACHABLE();
      }
      Bind(node, l_load);
      return;
    }
    case MemoryRepresentation::ProtectedPointer(): {
#if V8_ENABLE_SANDBOX
      LLVMTypeRef memory_type = LLVMPointerType(GetInt32T(), GetPtrAddressSpace(base));
      base = CanonicalizeToPtr(base, memory_type);
      l_load = LLVMBuildLoad2(builder_, GetInt32T(), base, "");
      if (load.kind.is_atomic) {
        UNREACHABLE();
      }
      l_load = LLVMBuildZExt(builder_, l_load, GetInt64T(), "");
      LLVMValueRef scratch_offset = LLVMConstInt(GetInt64T(), IsolateData::trusted_cage_base_offset(), 0);
      LLVMValueRef scratch_base = LLVMBuildAdd(builder_, GetPureRoot(), scratch_offset, "");
      LLVMValueRef scratch = LLVMBuildLoad2(builder_, GetInt64T(), scratch_base, "");
      l_load = LLVMBuildOr(builder_, l_load, scratch, "");
      l_load = LLVMBuildIntToPtr(builder_, l_load, GetTaggedHPtrT(), NodeName(node).c_str());
      Bind(node, l_load);
      return;
#else
      UNREACHABLE();
#endif
    }
    case MemoryRepresentation::IndirectPointer():
      UNREACHABLE();
    case MemoryRepresentation::SandboxedPointer(): {
#if V8_ENABLE_SANDBOX
      LLVMTypeRef memory_type = LLVMPointerType(GetInt64T(), GetPtrAddressSpace(base));
      base = CanonicalizeToPtr(base, memory_type);
      l_load = LLVMBuildLoad2(builder_, GetInt64T(), base, "");
      if (load.kind.is_atomic) {
        UNREACHABLE();
      }
      l_load = LLVMBuildLShr(builder_, l_load, LLVMConstInt(GetInt64T(), kSandboxedPointerShift, 0), "");
      l_load = LLVMBuildAdd(builder_, GetPurePtrComprCageBase(), l_load, "");
      l_load = LLVMBuildIntToPtr(builder_, l_load, GetTaggedHPtrT(), NodeName(node).c_str());
      Bind(node, l_load);
      return;
#else
      UNREACHABLE();
#endif  
    }
    case MemoryRepresentation::Simd128(): {
      LLVMTypeRef vector_type = LLVMVectorType(GetInt64T(), 2);
      LLVMTypeRef memory_type = LLVMPointerType(vector_type, GetPtrAddressSpace(base));
      base = CanonicalizeToPtr(base, memory_type);
      l_load = LLVMBuildLoad2(builder_, vector_type, base, NodeName(node).c_str());
      if (load.kind.is_atomic) {
        UNREACHABLE();
      }
      Bind(node, l_load);
      return;
    }
    case MemoryRepresentation::Simd256():
      UNREACHABLE();
  }
}

void LLVMIRBuilder::HandleStore(OpIndex node) {
  VisitStore(node);
}

void LLVMIRBuilder::VisitStore(OpIndex node) {
  const turboshaft::Operation& op = graph_.Get(node);
  const StoreOp& store = op.Cast<StoreOp>();

  MemoryRepresentation stored_rep = store.stored_rep;
  WriteBarrierKind write_barrier_kind = store.write_barrier;

  LLVMValueRef base = GetLValueOf(store.base());
  OpIndex index = store.index().value();
  LLVMValueRef value = GetLValueOf(store.value());
  if (index.valid()) {
    if (LLVMGetTypeKind(LLVMTypeOf(base)) == LLVMPointerTypeKind) {
      base = LLVMBuildPtrToInt(builder_, base, GetInt64T(), "");
    }
    base = LLVMBuildAdd(builder_, base, GetLValueOf(index), "");
  }
  LLVMValueRef l_store = nullptr;

  if (write_barrier_kind != kNoWriteBarrier && !v8_flags.disable_write_barriers) {
    MachineRepresentation representation = stored_rep.ToMachineType().representation();
    CHECK(CanBeTaggedOrCompressedOrIndirectPointer(representation));
    if (representation == MachineRepresentation::kIndirectPointer) {
      CHECK_EQ(write_barrier_kind, kIndirectPointerWriteBarrier);
#ifdef V8_ENABLE_SANDBOX
      IndirectPointerTag tag = store.indirect_pointer_tag();
      LLVMValueRef scratch_offset = LLVMConstInt(GetInt64T(), 
                                                ExposedTrustedObject::kSelfIndirectPointerOffset - kHeapObjectTag, 0);
      LLVMValueRef scratch_base = LLVMBuildAdd(builder_, value, scratch_offset, "");
      LLVMTypeRef memory_type = LLVMPointerType(GetInt32T(), GetPtrAddressSpace(base));
      scratch_base = CanonicalizeToPtr(scratch_base, memory_type);
      LLVMValueRef scratch = LLVMBuildLoad2(builder_, memory_type, scratch_base, "");

      base = CanonicalizeToPtr(base, memory_type);
      LLVMBuildStore(builder_, scratch, base);
      if (store.kind.is_atomic) {
        UNREACHABLE();
      }

      LLVMTypeRef mark_type = LLVMPointerType(GetInt8T(), GetPtrAddressSpace(base));
      LLVMValueRef mark_base = LLVMBuildAdd(builder_, GetPureRoot(),
                                            LLVMConstInt(GetInt64T(), IsolateData::is_marking_flag_offset(), 0), "");
      mark_base = CanonicalizeToPtr(mark_base, mark_type);
      LLVMValueRef mark = LLVMBuildLoad2(builder_, GetInt8T(), mark_base, "");
      LLVMValueRef check = LLVMBuildICmp(builder_, LLVMIntNE, mark, LLVMConstInt(GetInt8T(), 0, 0), "");

      LLVMBasicBlockRef check_bb = LLVMAppendBasicBlockInContext(context_, function_, "");
      LLVMBasicBlockRef pass_bb = LLVMAppendBasicBlockInContext(context_, function_, "");

      LLVMBuildCondBr(builder_, check, pass_bb, check_bb);
      current_lbb_ = check_bb;
      LLVMPositionBuilderAtEnd(builder_, current_lbb_);

      auto builtin_offset = 
          IsolateData::BuiltinEntrySlotOffset(Builtin::kIndirectPointerBarrierIgnoreFP);
      LLVMValueRef l_offset = LLVMConstInt(GetInt64T(), builtin_offset, 0);
      LLVMValueRef l_address = LLVMBuildGEP2(builder_, GetInt8T(), GetRoot(), &l_offset, 1, "");
      LLVMValueRef callee = LLVMBuildLoad2(builder_, GetInt64T(), l_address, "");

      std::vector<LLVMTypeRef> param_types = { GetInt64T(), GetInt64T(), GetInt64T() };
      std::vector<LLVMValueRef> args = 
          { FixTypeTo(GetLValueOf(store.base()), GetInt64T(), nullptr),
            FixTypeTo(base, GetInt64T(), nullptr),
            LLVMConstInt(GetInt64T(), tag, 0) };
      LLVMTypeRef callee_type = LLVMFunctionType(GetVoidT(), param_types.data(),
                                                 static_cast<uint32_t>(param_types.size()), false);
      callee = CanonicalizeToPtr(callee, LLVMPointerType(callee_type, 0));
      LLVMValueRef llvm_call = LLVMBuildCall2(builder_, callee_type, callee, args.data(),
                                              static_cast<uint32_t>(args.size()), "");
      LLVMSetInstructionCallConv(llvm_call, LLVMAArch64V8WriteBarrierCallConv);
      SetGCLeafFunction(llvm_call);
      LLVMBuildBr(builder_, pass_bb);
      current_lbb_ = pass_bb;
      LLVMPositionBuilderAtEnd(builder_, current_lbb_);
      block2EndLBB_[current_block_->index()] = pass_bb;
      return;
#else
      UNREACHABLE();
#endif
    } else {
      RecordWriteMode mode = WriteBarrierKindToRecordWriteMode(write_barrier_kind);
      CHECK_NE(mode, RecordWriteMode::kValueIsIndirectPointer);
      if (COMPRESS_POINTERS_BOOL) {
        if (LLVMTypeOf(value) != GetInt32T()) {
          value = FixTypeTo(value, GetInt32T(), nullptr);
        }
      } else {
        CHECK_EQ(LLVMTypeOf(value), GetTaggedHPtrT());
      }
      LLVMTypeRef memory_type = LLVMPointerType(
          COMPRESS_POINTERS_BOOL ? GetInt32T() : GetTaggedHPtrT(), GetPtrAddressSpace(base));
      base = CanonicalizeToPtr(base, memory_type);
      l_store = LLVMBuildStore(builder_, value, base);
      if (store.kind.is_atomic) {
        LLVMSetOrdering(l_store, LLVMAtomicOrderingRelease);
      }
      LLVMBasicBlockRef check_bb = LLVMAppendBasicBlockInContext(context_, function_, "");
      LLVMBasicBlockRef barrier_bb = LLVMAppendBasicBlockInContext(context_, function_, "");
      LLVMBasicBlockRef call_bb = LLVMAppendBasicBlockInContext(context_, function_, "");
      LLVMBasicBlockRef pass_bb = LLVMAppendBasicBlockInContext(context_, function_, "");
      if (mode > RecordWriteMode::kValueIsIndirectPointer) {
        // check is smi
        LLVMValueRef mask = LLVMBuildAnd(builder_, FixTypeTo(value, GetInt32T(), nullptr),
                                         LLVMConstInt(GetInt32T(), 1, 0), "");
        LLVMValueRef check = LLVMBuildICmp(builder_, LLVMIntEQ, mask, LLVMConstInt(GetInt32T(), 0, 0), "");
        LLVMBuildCondBr(builder_, check, pass_bb, check_bb);                                  
      } else {
        LLVMBuildBr(builder_, check_bb);
      }
      current_lbb_ = check_bb;
      LLVMPositionBuilderAtEnd(builder_, current_lbb_);
      LLVMValueRef scratch = LLVMBuildAnd(builder_, FixTypeTo(GetLValueOf(store.base()), GetInt64T(), nullptr),
          LLVMConstInt(GetInt64T(), ~MemoryChunk::GetAlignmentMaskForAssembler(), 0), "");
      scratch = LLVMBuildAdd(builder_, scratch, LLVMConstInt(GetInt64T(), MemoryChunkLayout::kFlagsOffset, 0), "");
      scratch = LLVMBuildIntToPtr(builder_, scratch, LLVMPointerType(GetInt64T(), 0), "");
      scratch = LLVMBuildLoad2(builder_, GetInt64T(), scratch, "");
      LLVMValueRef mask = LLVMBuildAnd(builder_, scratch,
          LLVMConstInt(GetInt64T(), MemoryChunk::kPointersFromHereAreInterestingMask, 0), "");
      LLVMValueRef check = LLVMBuildICmp(builder_, LLVMIntEQ, mask, LLVMConstInt(GetInt64T(), 0, 0), "");
      LLVMBuildCondBr(builder_, check, pass_bb, barrier_bb);

      current_lbb_ = barrier_bb;
      LLVMPositionBuilderAtEnd(builder_, current_lbb_);
      LLVMValueRef val = GetLValueOf(store.value());
      if (COMPRESS_POINTERS_BOOL) {
        val = FixTypeTo(val, GetInt32T(), nullptr);
        val = LLVMBuildZExt(builder_, val, GetInt64T(), "");
        val = LLVMBuildAdd(builder_, val, GetPurePtrComprCageBase(), "");
      }

      scratch = LLVMBuildAnd(builder_, FixTypeTo(val, GetInt64T(), nullptr),
          LLVMConstInt(GetInt64T(), ~MemoryChunk::GetAlignmentMaskForAssembler(), 0), "");
      scratch = LLVMBuildAdd(builder_, scratch, LLVMConstInt(GetInt64T(), MemoryChunkLayout::kFlagsOffset, 0), "");
      scratch = LLVMBuildIntToPtr(builder_, scratch, LLVMPointerType(GetInt64T(), 0), "");
      scratch = LLVMBuildLoad2(builder_, GetInt64T(), scratch, "");
      mask = LLVMBuildAnd(builder_, scratch,
          LLVMConstInt(GetInt64T(), MemoryChunk::kPointersToHereAreInterestingMask, 0), "");
      check = LLVMBuildICmp(builder_, LLVMIntEQ, mask, LLVMConstInt(GetInt64T(), 0, 0), "");    
      LLVMBuildCondBr(builder_, check, pass_bb, call_bb);

      current_lbb_ = call_bb;
      LLVMPositionBuilderAtEnd(builder_, current_lbb_);

      int32_t builtin_offset = -1;
      if (mode == RecordWriteMode::kValueIsEphemeronKey) {
        builtin_offset = IsolateData::BuiltinEntrySlotOffset(Builtin::kEphemeronKeyBarrierIgnoreFP);
#if V8_ENABLE_WEBASSEMBLY
      } else if (data_->info()->code_kind() == CodeKind::WASM_FUNCTION) {
        UNIMPLEMENTED();
#endif // V8_ENABLE_WEBASSEMBLY
      } else {
        builtin_offset = IsolateData::BuiltinEntrySlotOffset(Builtin::kRecordWriteIgnoreFP);
      }
      LLVMValueRef l_offset = LLVMConstInt(GetInt64T(), builtin_offset, 0);
      LLVMValueRef l_address = LLVMBuildGEP2(builder_, GetInt8T(), GetRoot(), &l_offset, 1, "");
      LLVMValueRef callee = LLVMBuildLoad2(builder_, GetInt64T(), l_address, "");

      std::vector<LLVMTypeRef> param_types = { GetInt64T(), GetInt64T() };
      std::vector<LLVMValueRef> args = 
          { FixTypeTo(GetLValueOf(store.base()), GetInt64T(), nullptr),
            FixTypeTo(base, GetInt64T(), nullptr) };
      LLVMTypeRef callee_type = LLVMFunctionType(GetVoidT(), param_types.data(),
                                                 static_cast<uint32_t>(param_types.size()), false);
      callee = CanonicalizeToPtr(callee, LLVMPointerType(callee_type, 0));
      LLVMValueRef llvm_call = LLVMBuildCall2(builder_, callee_type, callee, args.data(),
                                              static_cast<uint32_t>(args.size()), "");
      LLVMSetInstructionCallConv(llvm_call, LLVMAArch64V8WriteBarrierCallConv);
      SetGCLeafFunction(llvm_call);
      LLVMBuildBr(builder_, pass_bb);

      current_lbb_ = pass_bb;
      LLVMPositionBuilderAtEnd(builder_, current_lbb_);
      block2EndLBB_[current_block_->index()] = pass_bb;
      return;
    }
  }

  switch (stored_rep) {
    case MemoryRepresentation::Int8():
    case MemoryRepresentation::Uint8(): {
      if (LLVMTypeOf(value) != GetInt8T()) {
        value = FixTypeTo(value, GetInt8T(), nullptr);
      }
      LLVMTypeRef memory_type = LLVMPointerType(GetInt8T(), GetPtrAddressSpace(base));
      base = CanonicalizeToPtr(base, memory_type);
      l_store = LLVMBuildStore(builder_, value, base);
      if (store.kind.is_atomic) {
        LLVMSetOrdering(l_store, LLVMAtomicOrderingRelease);
      }
      Bind(node, l_store);
      return;
    }
    case MemoryRepresentation::Int16():
    case MemoryRepresentation::Uint16(): {
      if (LLVMTypeOf(value) != GetInt16T()) {
        value = FixTypeTo(value, GetInt16T(), nullptr);
      }
      LLVMTypeRef memory_type = LLVMPointerType(GetInt16T(), GetPtrAddressSpace(base));
      base = CanonicalizeToPtr(base, memory_type);
      l_store = LLVMBuildStore(builder_, value, base);
      if (store.kind.is_atomic) {
        LLVMSetOrdering(l_store, LLVMAtomicOrderingRelease);
      }
      Bind(node, l_store);
      return;
    }
    case MemoryRepresentation::Int32():
    case MemoryRepresentation::Uint32(): {
      CHECK_EQ(LLVMTypeOf(value), GetInt32T());
      LLVMTypeRef memory_type = LLVMPointerType(GetInt32T(), GetPtrAddressSpace(base));
      base = CanonicalizeToPtr(base, memory_type);
      l_store = LLVMBuildStore(builder_, value, base);
      if (store.kind.is_atomic) {
        LLVMSetOrdering(l_store, LLVMAtomicOrderingRelease);
      }
      Bind(node, l_store);
      return;
    }
    case MemoryRepresentation::Int64():
    case MemoryRepresentation::Uint64(): {
      CHECK_EQ(LLVMTypeOf(value), GetInt64T());
      LLVMTypeRef memory_type = LLVMPointerType(GetInt64T(), GetPtrAddressSpace(base));
      base = CanonicalizeToPtr(base, memory_type);
      l_store = LLVMBuildStore(builder_, value, base);
      if (store.kind.is_atomic) {
        LLVMSetOrdering(l_store, LLVMAtomicOrderingRelease);
      }
      Bind(node, l_store);
      return;
    }
    case MemoryRepresentation::Float16(): {
      CHECK_EQ(LLVMTypeOf(value), GetHalfT());
      LLVMTypeRef memory_type = LLVMPointerType(GetHalfT(), GetPtrAddressSpace(base));
      base = CanonicalizeToPtr(base, memory_type);
      l_store = LLVMBuildStore(builder_, value, base);
      if (store.kind.is_atomic) {
        UNREACHABLE();
      }
      Bind(node, l_store);
      return;
    }
    case MemoryRepresentation::Float32(): {
      CHECK_EQ(LLVMTypeOf(value), GetFloatT());
      LLVMTypeRef memory_type = LLVMPointerType(GetFloatT(), GetPtrAddressSpace(base));
      base = CanonicalizeToPtr(base, memory_type);
      l_store = LLVMBuildStore(builder_, value, base);
      if (store.kind.is_atomic) {
        UNREACHABLE();
      }
      Bind(node, l_store);
      return;
    }
    case MemoryRepresentation::Float64(): {
      CHECK_EQ(LLVMTypeOf(value), GetDoubleT());
      LLVMTypeRef memory_type = LLVMPointerType(GetDoubleT(), GetPtrAddressSpace(base));
      base = CanonicalizeToPtr(base, memory_type);
      l_store = LLVMBuildStore(builder_, value, base);
      if (store.kind.is_atomic) {
        UNREACHABLE();
      }
      Bind(node, l_store);
      return;
    }
    case MemoryRepresentation::AnyTagged():
    case MemoryRepresentation::TaggedPointer():
    case MemoryRepresentation::TaggedSigned(): {
      if (COMPRESS_POINTERS_BOOL) {
        if (LLVMTypeOf(value) != GetInt32T()) {
          value = FixTypeTo(value, GetInt32T(), nullptr);
        }
      } else {
        if (LLVMTypeOf(value) == GetInt64T()) {
          value = FixTypeTo(value, GetTaggedHPtrT(), nullptr);
        } else if (LLVMTypeOf(value) != GetTaggedHPtrT()) {
          UNREACHABLE();
        }
      }
      LLVMTypeRef memory_type = LLVMPointerType(
          COMPRESS_POINTERS_BOOL ? GetInt32T() : GetTaggedHPtrT(), GetPtrAddressSpace(base));
      base = CanonicalizeToPtr(base, memory_type);
      l_store = LLVMBuildStore(builder_, value, base);
      if (store.kind.is_atomic) {
        UNREACHABLE();
      }
      Bind(node, l_store);
      return;
    }
    case MemoryRepresentation::AnyUncompressedTagged():
    case MemoryRepresentation::UncompressedTaggedPointer():
    case MemoryRepresentation::UncompressedTaggedSigned(): {
      CHECK_EQ(LLVMTypeOf(value), GetTaggedHPtrT());
      LLVMTypeRef memory_type = LLVMPointerType(GetTaggedHPtrT(), GetPtrAddressSpace(base));
      base = CanonicalizeToPtr(base, memory_type);
      l_store = LLVMBuildStore(builder_, value, base);
      if (store.kind.is_atomic) {
        UNREACHABLE();
      }
      Bind(node, l_store);
      return;
    }
    case MemoryRepresentation::ProtectedPointer():
      // We never store directly to protected pointersfrom generated code.
      UNREACHABLE();
    case MemoryRepresentation::IndirectPointer():{
#ifdef V8_ENABLE_SANDBOX
      LLVMValueRef scratch_offset = LLVMConstInt(GetInt64T(), 
                                                ExposedTrustedObject::kSelfIndirectPointerOffset - kHeapObjectTag, 0);
      LLVMValueRef scratch_base = LLVMBuildAdd(builder_, value, scratch_offset, "");
      LLVMTypeRef memory_type = LLVMPointerType(GetInt32T(), GetPtrAddressSpace(base));
      scratch_base = CanonicalizeToPtr(scratch_base, memory_type);
      LLVMValueRef scratch = LLVMBuildLoad2(builder_, memory_type, scratch_base, "");

      base = CanonicalizeToPtr(base, memory_type);
      l_store = LLVMBuildStore(builder_, scratch, base);
      if (store.kind.is_atomic) {
        UNREACHABLE();
      }
      Bind(node, l_store);
      return;
#else
      UNREACHABLE();
#endif
    }
    case MemoryRepresentation::SandboxedPointer(): {
#ifdef V8_ENABLE_SANDBOX
      LLVMValueRef scratch = LLVMBuildAdd(builder_, value, GetPurePtrComprCageBase(), "");
      scratch = LLVMBuildShl(builder_, scratch, LLVMConstInt(GetInt64T(), kSandboxedPointerShift, 0), "");
      LLVMTypeRef memory_type = LLVMPointerType(GetInt64T(),GetPtrAddressSpace(base));
      base = CanonicalizeToPtr(base, memory_type);
      l_store = LLVMBuildStore(builder_, scratch, base);
      if (store.kind.is_atomic) {
        UNREACHABLE();
      }
      Bind(node, l_store);
      return;
#else
      UNREACHABLE();
#endif
    }
    case MemoryRepresentation::Simd128(): {
      LLVMTypeRef vector_type = LLVMVectorType(GetInt64T(), 2);
      CHECK_EQ(LLVMTypeOf(value), vector_type);
      LLVMTypeRef memory_type = LLVMPointerType(vector_type, GetPtrAddressSpace(base));
      base = CanonicalizeToPtr(base, memory_type);
      l_store = LLVMBuildStore(builder_, value, base);
      if (store.kind.is_atomic) {
        UNREACHABLE();
      }
      Bind(node, l_store);
      return;
    }
    case MemoryRepresentation::Simd256():
      UNREACHABLE();
  }
}

void LLVMIRBuilder::HandleWordBinop(OpIndex node) {
  VisitWordBinop(node);
}

void LLVMIRBuilder::VisitWordBinop(OpIndex node) {
  const turboshaft::Operation& op = graph_.Get(node);
  const WordBinopOp& binop = op.Cast<WordBinopOp>();

  LLVMValueRef left = GetLValueOf(binop.input(0));
  LLVMValueRef right = GetLValueOf(binop.input(1));

  LLVMTypeRef left_type = LLVMTypeOf(left);
  LLVMTypeRef right_type = LLVMTypeOf(right);
  if (binop.rep == WordRepresentation::Word32()) {
    if (left_type != GetInt32T()) {
      left = LLVMBuildZExt(builder_, left, GetInt32T(), "");
    }
    if (right_type != GetInt32T()) {
      right = LLVMBuildZExt(builder_, right, GetInt32T(), "");
    }
  } else {
    CHECK(binop.rep == WordRepresentation::Word64());
    if (left_type != GetInt64T()) {
      left = FixTypeTo(left, GetInt64T(), nullptr);
    }
    if (right_type != GetInt64T()) {
      right = FixTypeTo(right, GetInt64T(), nullptr);
    }
  }

  LLVMValueRef bin = nullptr;
  switch (binop.kind) {
    case WordBinopOp::Kind::kAdd: {
      bin = LLVMBuildAdd(builder_, left, right, NodeName(node).c_str());
      break;
    }
    case WordBinopOp::Kind::kMul: {
      bin = LLVMBuildMul(builder_, left, right, NodeName(node).c_str());
      break;
    }
    case WordBinopOp::Kind::kSignedMulOverflownBits: 
      UNIMPLEMENTED();
    case WordBinopOp::Kind::kUnsignedMulOverflownBits: 
      UNIMPLEMENTED();
    case WordBinopOp::Kind::kBitwiseAnd: {
      bin = LLVMBuildAnd(builder_, left, right, NodeName(node).c_str());
      break;
    }
    case WordBinopOp::Kind::kBitwiseOr: {
      bin = LLVMBuildOr(builder_, left, right, NodeName(node).c_str());
      break;
    }
    case WordBinopOp::Kind::kBitwiseXor: {
      bin = LLVMBuildXor(builder_, left, right, NodeName(node).c_str());
      break;
    }
    case WordBinopOp::Kind::kSub: {
      bin = LLVMBuildSub(builder_, left, right, NodeName(node).c_str());
      break;
    }
    case WordBinopOp::Kind::kSignedDiv: {
      bin = LLVMBuildSDiv(builder_, left, right, NodeName(node).c_str());
      break;
    }
    case WordBinopOp::Kind::kUnsignedDiv: {
      bin = LLVMBuildUDiv(builder_, left, right, NodeName(node).c_str());
      break;
    }
    case WordBinopOp::Kind::kSignedMod: {
      bin = LLVMBuildSRem(builder_, left, right, NodeName(node).c_str());
      break;
    }
    case WordBinopOp::Kind::kUnsignedMod: {
      bin = LLVMBuildURem(builder_, left, right, NodeName(node).c_str());
      break;
    }
  }

  Bind(node, bin);
}

void LLVMIRBuilder::HandleFloatBinop(OpIndex node) {
  VisitFloatBinop(node);
}

void LLVMIRBuilder::VisitFloatBinop(OpIndex node) {
  const turboshaft::Operation& op = graph_.Get(node);
  const FloatBinopOp& binop = op.Cast<FloatBinopOp>();

  LLVMValueRef left = GetLValueOf(binop.input(0));
  LLVMValueRef right = GetLValueOf(binop.input(1));
  LLVMValueRef bin = nullptr;
  switch (binop.kind) {
    case FloatBinopOp::Kind::kAdd: {
      bin = LLVMBuildFAdd(builder_, left, right, NodeName(node).c_str());
      break;
    }
    case FloatBinopOp::Kind::kSub: {
      bin = LLVMBuildFSub(builder_, left, right, NodeName(node).c_str());
      break;
    }
    case FloatBinopOp::Kind::kMul: {
      bin = LLVMBuildFMul(builder_, left, right, NodeName(node).c_str());
      break;
    }
    case FloatBinopOp::Kind::kDiv: 
      UNIMPLEMENTED();
    case FloatBinopOp::Kind::kMin:
    case FloatBinopOp::Kind::kMax: 
    case FloatBinopOp::Kind::kMod: {
      if (binop.rep == Rep::Float32()) {
        UNREACHABLE();
      }
      UNIMPLEMENTED();
    }
    case FloatBinopOp::Kind::kPower: {
      if (binop.rep == Rep::Float32()) {
        UNREACHABLE();
      }
      UNIMPLEMENTED();
    }
    case FloatBinopOp::Kind::kAtan2: {
      if (binop.rep == Rep::Float32()) {
        UNREACHABLE();
      }
      UNIMPLEMENTED();
    }
  }

  Bind(node, bin);
}

void LLVMIRBuilder::HandleFloatUnary(OpIndex node) {
  VisitFloatUnary(node);
}

void LLVMIRBuilder::VisitFloatUnary(OpIndex node) {
  const turboshaft::Operation& op = graph_.Get(node);
  const FloatUnaryOp& unop = op.Cast<FloatUnaryOp>();

  LLVMValueRef input = GetLValueOf(unop.input());
  LLVMValueRef unary = nullptr;
  switch (unop.kind) {
    case FloatUnaryOp::Kind::kSilenceNaN: {
      CHECK(unop.rep == Rep::Float64());
      unary = LLVMBuildFSub(builder_, input, LLVMConstReal(GetDoubleT(), 0.0), NodeName(node).c_str());
      break;
    }
    case FloatUnaryOp::Kind::kAbs: {
      CHECK(unop.rep == Rep::Float64());
      CHECK(LLVMTypeOf(input) == GetDoubleT());
      LLVMValueRef fn = LLVMGetNamedFunction(module_, "llvm.fabs.f64");
      LLVMTypeRef param_types[] = { GetDoubleT() };
      LLVMTypeRef fn_type = LLVMFunctionType(GetDoubleT(), param_types, 1, 0);
      if (!fn) {
        /* init instrinsic function declare */
        fn = LLVMAddFunction(module_, "llvm.fabs.f64", fn_type);
      } 
      LLVMValueRef args[] = { input };
      unary = LLVMBuildCall2(builder_, fn_type, fn, args, 1, NodeName(node).c_str());
      SetGCLeafFunction(unary);
      break;
    }
    default:
      UNIMPLEMENTED();
  }

  Bind(node, unary);
}
void LLVMIRBuilder::HandleStackPointerGreaterThan(OpIndex node) {
  VisitStackPointerGreaterThan(node);
}

void LLVMIRBuilder::VisitStackPointerGreaterThan(OpIndex node) {
  const turboshaft::Operation& op = graph_.Get(node);
  const StackPointerGreaterThanOp& spGt = op.Cast<StackPointerGreaterThanOp>();
  StackCheckKind kind = spGt.kind;
  LLVMValueRef value = GetLValueOf(spGt.input(0));
  if (kind == StackCheckKind::kJSFunctionEntry) {
    UNIMPLEMENTED();
  }
  LLVMMetadataRef meta_string = LLVMMDStringInContext2(context_, "sp", 2);
  LLVMMetadataRef meta = LLVMMDNodeInContext2(context_, &meta_string, 1);
  std::vector<LLVMValueRef> args = {LLVMMetadataAsValue(context_, meta)};
  LLVMValueRef fn = LLVMGetNamedFunction(module_, "llvm.read_register.i64");
  LLVMTypeRef param_types[] = { LLVMMetadataTypeInContext(context_) };
  LLVMTypeRef fn_type = LLVMFunctionType(GetInt64T(), param_types, 1, 0);
  if (!fn) {
    /* init instrinsic function declare */
    fn = LLVMAddFunction(module_, "llvm.read_register.i64", fn_type);
  } 
  LLVMValueRef sp_reg = LLVMBuildCall2(builder_, fn_type, fn, args.data(), 1, "");
  SetGCLeafFunction(sp_reg);
  value = FixTypeTo(value, GetInt64T(), nullptr);
  CHECK(LLVMTypeOf(value) == LLVMTypeOf(sp_reg));
  LLVMValueRef comp = LLVMBuildICmp(builder_, LLVMIntSGT, sp_reg, value, NodeName(node).c_str());
  Bind(node, comp);
}

void LLVMIRBuilder::HandleFrameConstant(OpIndex node) {
  VisitFrameConstant(node);
}

void LLVMIRBuilder::VisitFrameConstant(OpIndex node) {
  const turboshaft::Operation& op = graph_.Get(node);
  const FrameConstantOp& constant = op.Cast<FrameConstantOp>();
  using Kind = turboshaft::FrameConstantOp::Kind;
  LLVMValueRef value = nullptr;
  if (constant.kind == Kind::kStackCheckOffset) {
    // kArchStackCheckOffset
    UNIMPLEMENTED();
  } else if (constant.kind == Kind::kFramePointer) {
    value = CalculateFuncFp();
  } else if (constant.kind == Kind::kParentFramePointer) {
    // kParentFramePointer
    LLVMValueRef current_fp = CalculateFuncFp();
    current_fp = CanonicalizeToPtr(current_fp, LLVMPointerType(GetInt64T(), 0));
    value = LLVMBuildLoad2(builder_, GetInt64T(), current_fp, NodeName(node).c_str());
  } else {
    UNREACHABLE();
  }
  Bind(node, value);
}

void LLVMIRBuilder::HandleComparison(OpIndex node) {
  VisitComparison(node);
}

void LLVMIRBuilder::VisitComparison(OpIndex node) {
  const turboshaft::Operation& op = graph_.Get(node);
  const ComparisonOp& comparison = op.Cast<ComparisonOp>();
  using Kind = ComparisonOp::Kind;

  LLVMValueRef left = GetLValueOf(comparison.input(0));
  LLVMValueRef right = GetLValueOf(comparison.input(1));

  LLVMTypeRef left_type = LLVMTypeOf(left);
  LLVMTypeRef right_type = LLVMTypeOf(right);

  LLVMTypeRef cmp_type = nullptr;

  if (comparison.rep == Rep::Word32()) {
    cmp_type = GetInt32T();
  } else if (comparison.rep == Rep::Word64()) {
    cmp_type = GetInt64T();
  } else if (comparison.rep == Rep::Tagged()) {
    if (!COMPRESS_POINTERS_BOOL) {
      cmp_type = GetTaggedHPtrT();
    } else {
      cmp_type = GetInt32T();
    }
  } else if (comparison.rep == Rep::Float32()) {
    cmp_type = GetFloatT();
    if (left_type != GetFloatT()) {
      UNIMPLEMENTED();
    }
    if (right_type != GetFloatT()) {
      UNIMPLEMENTED();
    }
  } else if (comparison.rep == Rep::Float64()) {
    cmp_type = GetDoubleT();
    if (left_type != GetDoubleT()) {
      UNIMPLEMENTED();
    }
    if (right_type != GetDoubleT()) {
      UNIMPLEMENTED();
    }
  } else {
    UNREACHABLE();
  }

  left = FixTypeTo(left, cmp_type, nullptr);
  right = FixTypeTo(right, cmp_type, nullptr);
  
  LLVMValueRef comp = nullptr;
  switch (multi(comparison.kind, comparison.rep)) {
    case multi(Kind::kEqual, Rep::Word32()):
    case multi(Kind::kEqual, Rep::Word64()):
    case multi(Kind::kEqual, Rep::Tagged()): {
      comp = LLVMBuildICmp(builder_, LLVMIntEQ, left, right, NodeName(node).c_str());
      break;
    }
    case multi(Kind::kEqual, Rep::Float32()):
    case multi(Kind::kEqual, Rep::Float64()): {
      comp = LLVMBuildFCmp(builder_, LLVMRealOEQ, left, right, NodeName(node).c_str());
      break;
    }
    case multi(Kind::kSignedLessThan, Rep::Word32()):
    case multi(Kind::kSignedLessThan, Rep::Word64()): {
      comp = LLVMBuildICmp(builder_, LLVMIntSLT, left, right, NodeName(node).c_str());
      break;
    }
    case multi(Kind::kSignedLessThan, Rep::Float32()):
    case multi(Kind::kSignedLessThan, Rep::Float64()): {
      comp = LLVMBuildFCmp(builder_, LLVMRealOLT, left, right, NodeName(node).c_str());
      break;
    }
    case multi(Kind::kSignedLessThanOrEqual, Rep::Word32()):
    case multi(Kind::kSignedLessThanOrEqual, Rep::Word64()): {
      comp = LLVMBuildICmp(builder_, LLVMIntSLE, left, right, NodeName(node).c_str());
      break;
    }
    case multi(Kind::kSignedLessThanOrEqual, Rep::Float32()):
    case multi(Kind::kSignedLessThanOrEqual, Rep::Float64()): {
      comp = LLVMBuildFCmp(builder_, LLVMRealOLE, left, right, NodeName(node).c_str());
      break;
    }
    case multi(Kind::kUnsignedLessThan, Rep::Word32()):
    case multi(Kind::kUnsignedLessThan, Rep::Word64()): {
      comp = LLVMBuildICmp(builder_, LLVMIntULT, left, right, NodeName(node).c_str());
      break;
    }
    case multi(Kind::kUnsignedLessThanOrEqual, Rep::Word32()):
    case multi(Kind::kUnsignedLessThanOrEqual, Rep::Word64()): {
      comp = LLVMBuildICmp(builder_, LLVMIntULE, left, right, NodeName(node).c_str());
      break;
    }
    default:
      UNREACHABLE();
  }

  Bind(node, comp);
}

void LLVMIRBuilder::HandleBranch(OpIndex node) {
  VisitBranch(node);
}

void LLVMIRBuilder::VisitBranch(OpIndex node) {
  const turboshaft::Operation& op = graph_.Get(node);
  const BranchOp& branch = op.Cast<BranchOp>();

  LLVMBasicBlockRef tbranch = block2LBB_[branch.if_true->index()];
  CHECK_NOT_NULL(tbranch);
  LLVMBasicBlockRef fbranch = block2LBB_[branch.if_false->index()];
  CHECK_NOT_NULL(fbranch);
  LLVMValueRef cond = GetLValueOf(branch.condition());
  LLVMTypeRef cond_type = LLVMTypeOf(cond);
  if (cond_type != GetInt1T()) {
    cond = LLVMBuildICmp(builder_, LLVMIntNE, cond, LLVMConstInt(cond_type, 0, 0), "");
  }
  LLVMValueRef br = nullptr;
  if (tbranch == fbranch) {
    br = LLVMBuildBr(builder_, tbranch);
  } else {
    br = LLVMBuildCondBr(builder_, cond, tbranch, fbranch);
    if (branch.hint != BranchHint::kNone) {
      uint32_t true_weight = branch.hint == BranchHint::kTrue ? 95 : 5;
      uint32_t false_weight = branch.hint == BranchHint::kFalse ? 95 : 5;
      AddBranchWeight(br, true_weight, false_weight);
    }
  }

  Bind(node, br);
}

void LLVMIRBuilder::HandleShift(OpIndex node) {
  VisitShift(node);
}

void LLVMIRBuilder::VisitShift(OpIndex node) {
  const turboshaft::Operation& op = graph_.Get(node);
  const auto& shift = op.Cast<ShiftOp>();

  LLVMValueRef left = GetLValueOf(shift.left());
  LLVMValueRef right = GetLValueOf(shift.right());
  // turboshaft may produce smaller type for operands, fix it
  if (LLVMTypeOf(left) != LLVMTypeOf(right)) {
    if (shift.rep == RegisterRepresentation::Word32()) {
      if (LLVMTypeOf(left) != GetInt32T()) {
        left = LLVMBuildZExt(builder_, left, GetInt32T(), "");
      }
      if (LLVMTypeOf(right) != GetInt32T()) {
        right = LLVMBuildZExt(builder_, right, GetInt32T(), "");
      }
    } else if (shift.rep == RegisterRepresentation::Word64()) {
      if (LLVMTypeOf(left) != GetInt64T()) {
        left = LLVMBuildZExt(builder_, left, GetInt64T(), "");
      }
      if (LLVMTypeOf(right) != GetInt64T()) {
        right = LLVMBuildZExt(builder_, right, GetInt64T(), "");
      }
    } else {
      UNREACHABLE();
    }
  }
  LLVMValueRef l_shift = nullptr;
  switch (shift.kind) {
    case ShiftOp::Kind::kShiftRightArithmeticShiftOutZeros:
    case ShiftOp::Kind::kShiftRightArithmetic: {
      l_shift = LLVMBuildAShr(builder_, left, right, NodeName(node).c_str());
      break;
    }
    case ShiftOp::Kind::kShiftRightLogical: {
      l_shift = LLVMBuildLShr(builder_, left, right, NodeName(node).c_str());
      break;
    }
    case ShiftOp::Kind::kShiftLeft: {
      l_shift = LLVMBuildShl(builder_, left, right, NodeName(node).c_str());
      break;
    }
    case ShiftOp::Kind::kRotateRight: 
      UNIMPLEMENTED();
    case ShiftOp::Kind::kRotateLeft: 
      UNIMPLEMENTED();
  }

  Bind(node, l_shift);
}

void LLVMIRBuilder::HandleChange(OpIndex node) {
  VisitChange(node);
}

void LLVMIRBuilder::VisitChange(OpIndex node) {
  const turboshaft::Operation& op = graph_.Get(node);
  const turboshaft::ChangeOp& change = op.Cast<turboshaft::ChangeOp>();

  LLVMValueRef input = GetLValueOf(op.input(0));
  LLVMValueRef cvt = nullptr;
  switch (change.kind) {
    case ChangeOp::Kind::kFloatConversion: {
      if (change.from == Rep::Float64()) {
        CHECK_EQ(change.to, Rep::Float32());
        cvt = LLVMBuildFPTrunc(builder_, input, GetFloatT(), NodeName(node).c_str());
      } else {
        CHECK_EQ(change.from, Rep::Float32());
        CHECK_EQ(change.to, Rep::Float64());
        cvt = LLVMBuildFPExt(builder_, input, GetDoubleT(), NodeName(node).c_str());
      }
      break;
    }
    case ChangeOp::Kind::kSignedFloatTruncateOverflowToMin:
    case ChangeOp::Kind::kUnsignedFloatTruncateOverflowToMin: {
      using A = ChangeOp::Assumption;
      bool is_signed = 
          change.kind == ChangeOp::Kind::kSignedFloatTruncateOverflowToMin;
      switch (multi(change.from, change.to, is_signed, change.assumption)) {
        case multi(Rep::Float32(), Rep::Word32(), true, A::kNoOverflow):
        case multi(Rep::Float32(), Rep::Word32(), true, A::kNoAssumption): {
          CHECK(LLVMTypeOf(input) == GetFloatT());
          cvt = LLVMBuildFPToSI(builder_, input, GetInt32T(), NodeName(node).c_str());
          break;
        }
        case multi(Rep::Float32(), Rep::Word32(), false, A::kNoOverflow):
        case multi(Rep::Float32(), Rep::Word32(), false, A::kNoAssumption): {
          CHECK(LLVMTypeOf(input) == GetFloatT());
          cvt = LLVMBuildFPToUI(builder_, input, GetInt32T(), NodeName(node).c_str());
          break;
        }
        case multi(Rep::Float64(), Rep::Word32(), true, A::kReversible):
          UNIMPLEMENTED();
        case multi(Rep::Float64(), Rep::Word32(), false, A::kReversible): 
          UNIMPLEMENTED();
        case multi(Rep::Float64(), Rep::Word32(), true, A::kNoOverflow): {
          CHECK(LLVMTypeOf(input) == GetDoubleT());
          cvt = LLVMBuildFPToSI(builder_, input, GetInt32T(), NodeName(node).c_str());
          break;
        }
        case multi(Rep::Float64(), Rep::Word32(), false, A::kNoAssumption):
        case multi(Rep::Float64(), Rep::Word32(), false, A::kNoOverflow): {
          CHECK(LLVMTypeOf(input) == GetDoubleT());
          cvt = LLVMBuildFPToUI(builder_, input, GetInt32T(), NodeName(node).c_str());
          break;
        }
        case multi(Rep::Float64(), Rep::Word64(), true, A::kReversible): 
          UNIMPLEMENTED();
        case multi(Rep::Float64(), Rep::Word64(), false, A::kReversible): {
          // fcvtzu
          CHECK(LLVMTypeOf(input) == GetDoubleT());
          cvt = LLVMBuildFPToUI(builder_, input, GetInt64T(), NodeName(node).c_str());
          break;
        }
        case multi(Rep::Float64(), Rep::Word64(), true, A::kNoOverflow):
        case multi(Rep::Float64(), Rep::Word64(), true, A::kNoAssumption): {
          CHECK(LLVMTypeOf(input) == GetDoubleT());
          cvt = LLVMBuildFPToSI(builder_, input, GetInt64T(), NodeName(node).c_str());
          break;
        }
        default:
          // Invalid combination
          UNREACHABLE();
      }
      break;
    }
    case ChangeOp::Kind::kJSFloatTruncate:
      CHECK_EQ(change.from, Rep::Float64());
      CHECK_EQ(change.to, Rep::Word32());
      UNIMPLEMENTED();
    case ChangeOp::Kind::kSignedToFloat: {
      if (change.from == Rep::Word32()) {
        if (change.to == Rep::Float32()) {
          cvt = LLVMBuildSIToFP(builder_, input, GetFloatT(), NodeName(node).c_str());
        } else {
          CHECK_EQ(change.to, Rep::Float64());
          CHECK_EQ(change.assumption, ChangeOp::Assumption::kNoAssumption);
          cvt = LLVMBuildSIToFP(builder_, input, GetDoubleT(), NodeName(node).c_str());
        }
      } else {
        CHECK_EQ(change.from, Rep::Word64());
        if (change.to == Rep::Float32()) {
          cvt = LLVMBuildSIToFP(builder_, input, GetFloatT(), NodeName(node).c_str());
        } else {
          CHECK_EQ(change.to, Rep::Float64());
          if (change.assumption == ChangeOp::Assumption::kReversible) {
            cvt = LLVMBuildSIToFP(builder_, input, GetDoubleT(), NodeName(node).c_str());
          } else {
            cvt = LLVMBuildSIToFP(builder_, input, GetDoubleT(), NodeName(node).c_str());
          }
        }
      }
      break;
    }
    case ChangeOp::Kind::kUnsignedToFloat: {
      switch (multi(change.from, change.to)) {
        case multi(Rep::Word32(), Rep::Float32()): {
          cvt = LLVMBuildUIToFP(builder_, input, GetFloatT(), NodeName(node).c_str());
          break;
        }
        case multi(Rep::Word32(), Rep::Float64()): {
          cvt = LLVMBuildUIToFP(builder_, input, GetDoubleT(), NodeName(node).c_str());
          break;
        }
        case multi(Rep::Word64(), Rep::Float32()): {
          cvt = LLVMBuildUIToFP(builder_, input, GetFloatT(), NodeName(node).c_str());
          break;
        }
        case multi(Rep::Word64(), Rep::Float64()): {
          cvt = LLVMBuildUIToFP(builder_, input, GetDoubleT(), NodeName(node).c_str());
          break;
        }
        default:
          UNREACHABLE();
      }
      break;
    }
    case ChangeOp::Kind::kExtractHighHalf:
      CHECK_EQ(change.from, Rep::Float64());
      CHECK_EQ(change.to, Rep::Word32());
      cvt = LLVMBuildBitCast(builder_, input, GetInt64T(), "");
      cvt = LLVMBuildLShr(builder_, cvt, LLVMConstInt(GetInt64T(), 32, 0), "");
      cvt = LLVMBuildTrunc(builder_, cvt, GetInt32T(), NodeName(node).c_str());
      break;
    case ChangeOp::Kind::kExtractLowHalf:
      CHECK_EQ(change.from, Rep::Float64());
      CHECK_EQ(change.to, Rep::Word32());
      cvt = LLVMBuildBitCast(builder_, input, GetInt64T(), "");
      cvt = LLVMBuildTrunc(builder_, cvt, GetInt32T(), NodeName(node).c_str());
      break;
    case ChangeOp::Kind::kZeroExtend: {
      CHECK_EQ(change.from, Rep::Word32());
      CHECK_EQ(change.to, Rep::Word64());
      cvt = LLVMBuildZExt(builder_, input, GetInt64T(), NodeName(node).c_str());
      break;
    }
    case ChangeOp::Kind::kSignExtend: {
      CHECK_EQ(change.from, Rep::Word32());
      CHECK_EQ(change.to, Rep::Word64());
      cvt = LLVMBuildSExt(builder_, input, GetInt64T(), NodeName(node).c_str());
      break;
    }
    case ChangeOp::Kind::kTruncate: {
      CHECK_EQ(change.from, Rep::Word64());
      CHECK_EQ(change.to, Rep::Word32());
      cvt = LLVMBuildIntCast(builder_, input, GetInt32T(), NodeName(node).c_str());
      break;
    }
    case ChangeOp::Kind::kBitcast: {
      switch (multi(change.from, change.to)) {
        case multi(Rep::Word32(), Rep::Word64()): 
          UNIMPLEMENTED();
        case multi(Rep::Word32(), Rep::Float32()): {
          cvt = LLVMBuildBitCast(builder_, input, GetFloatT(), NodeName(node).c_str());
          break;
        }
        case multi(Rep::Word64(), Rep::Float64()): {
          cvt = LLVMBuildBitCast(builder_, input, GetDoubleT(), NodeName(node).c_str());
          break;
        }
        case multi(Rep::Float32(), Rep::Word32()): {
          cvt = LLVMBuildBitCast(builder_, input, GetInt32T(), NodeName(node).c_str());
          break;
        }
        case multi(Rep::Float64(), Rep::Word64()): {
          cvt = LLVMBuildBitCast(builder_, input, GetInt64T(), NodeName(node).c_str());
          break;
        }
        default:
          UNREACHABLE();
      }
      break;
    }
  }

  Bind(node, cvt);
}

void LLVMIRBuilder::HandleGoto(OpIndex node) {
  VisitGoto(node);
}

void LLVMIRBuilder::VisitGoto(OpIndex node) {
  const turboshaft::Operation& op = graph_.Get(node);
  const GotoOp& branch = op.Cast<GotoOp>();

  LLVMBasicBlockRef target_branch = block2LBB_[branch.destination->index()];
  CHECK_NOT_NULL(target_branch);
  LLVMValueRef br = nullptr;
  br = LLVMBuildBr(builder_, target_branch);

  Bind(node, br);
}

void LLVMIRBuilder::HandlePhi(OpIndex node) {
  VisitPhi(node);
}

void LLVMIRBuilder::VisitPhi(OpIndex node) {
  const turboshaft::Operation& op = graph_.Get(node);
  const int input_count = op.input_count;
  CHECK_EQ(input_count, current_block_->PredecessorCount());

  LLVMValueRef phi = LLVMBuildPhi(builder_, ConvertLLVMTypeFromRep(op.Cast<PhiOp>().rep), NodeName(node).c_str());
  pending_phis_.emplace(node, current_block_);

  Bind(node, phi);
}

void LLVMIRBuilder::HandleReturn(OpIndex node) {
  VisitReturn(node);
}

void LLVMIRBuilder::VisitReturn(OpIndex node) {
  const turboshaft::Operation& op = graph_.Get(node);
  const ReturnOp& ret = op.Cast<ReturnOp>();

  size_t return_count = ret.return_values().size();
  if (return_count == 0) {
    LLVMBuildRetVoid(builder_);
  } else if (return_count == 1) {
    LLVMValueRef ret_val = GetLValueOf(ret.return_values()[0]);
    ret_val = FixTypeTo(ret_val,
        ConvertLLVMTypeFromMachineType(linkage_->GetIncomingDescriptor()->GetReturnType(0)), nullptr);
    LLVMBuildRet(builder_, ret_val);
  } else {
    UNIMPLEMENTED();
  }
}

void LLVMIRBuilder::HandleCall(OpIndex node) {
  VisitCall(node);
}

void LLVMIRBuilder::VisitCall(OpIndex node) {
  const turboshaft::Operation& op = graph_.Get(node);
  const CallOp& call = op.Cast<CallOp>();
  const CallDescriptor* call_descriptor = call.descriptor->descriptor;

  if (call_descriptor->NeedsFrameState()) {
    UNIMPLEMENTED();
  }

  if (call_descriptor->kind() != CallDescriptor::kCallCodeObject &&
      call_descriptor->kind() != CallDescriptor::kCallAddress) {
    UNIMPLEMENTED();
  }

  size_t return_count = call_descriptor->ReturnCount();
  CHECK(return_count == 1 || return_count == 0);
  LLVMTypeRef return_type = (return_count == 0) ?
    GetVoidT() : ConvertLLVMTypeFromMachineType(call_descriptor->GetReturnType(0));
  
  auto cc = GetLLVMCallConvByDescriptor(call_descriptor);
  std::vector<LLVMValueRef> args;
  std::vector<LLVMTypeRef> param_types;
  base::Vector<const OpIndex> arguments = call.arguments();
  LLVMValueRef callee = GetLValueOf(call.callee());
  PrepareCallParams(call_descriptor, arguments, param_types, args, callee);

  //false: not allow va-args
  LLVMTypeRef callee_type = LLVMFunctionType(return_type, param_types.data(),
                                             static_cast<uint32_t>(param_types.size()), false);
  const ConstantOp* constant = graph_.Get(call.callee()).TryCast<ConstantOp>();
  if (call_descriptor->kind() == CallDescriptor::kCallCodeObject && 
      (constant == nullptr || constant->kind != ConstantOp::Kind::kHeapObject)) {
#ifdef V8_ENABLE_SANDBOX
    LLVMValueRef offset = LLVMConstInt(GetInt64T(), Code::kSelfIndirectPointerOffset - kHeapObjectTag, 0);
    callee = FixTypeTo(callee, GetInt64T(), nullptr);
    callee = LLVMBuildAdd(builder_, callee, offset, "");
    callee = CanonicalizeToPtr(callee, LLVMPointerType(GetInt32T(), 0));
    callee = LLVMBuildLoad2(builder_, GetInt32T(), callee, "");
    callee = LLVMBuildLShr(builder_, callee, LLVMConstInt(GetInt32T(), kCodePointerHandleShift, 0), "");
    callee = LLVMBuildShl(builder_, callee, LLVMConstInt(GetInt32T(), kCodePointerTableEntrySizeLog2, 0), "");
    callee = LLVMBuildZExt(builder_, callee, GetInt64T(), "");
    ExternalReference ref = ExternalReference::code_pointer_table_address();
    LLVMValueRef l_ref = nullptr;
    if (ref.IsIsolateFieldId()) {
      UNREACHABLE();
    }
    if (MacroAssemblerBase::IsAddressableThroughRootRegister(data_->isolate(), ref)) {
      UNREACHABLE();
    } else {
      int32_t offset = 
          MacroAssemblerBase::RootRegisterOffsetForExternalReferenceTableEntry(data_->isolate(), ref);
      LLVMValueRef l_object_offset = LLVMConstInt(GetInt64T(), offset, 1);
      LLVMValueRef l_object_address = LLVMBuildGEP2(builder_, GetInt8T(), GetRoot(),
                                                      &l_object_offset, 1, "");
      l_ref = LLVMBuildLoad2(builder_, GetInt64T(), l_object_address, "");
    }
    callee = LLVMBuildAdd(builder_, l_ref, callee, "");
    callee = CanonicalizeToPtr(callee, LLVMPointerType(GetInt64T(), 0));
    callee = LLVMBuildLoad2(builder_, GetInt64T(), callee, "");
    if (call_descriptor->RequiresEntrypointTagForCall()) {
      uint64_t tag = static_cast<uint64_t>(call_descriptor->shifted_tag());
      if (tag != 0) {
        callee = LLVMBuildXor(builder_, callee, LLVMConstInt(GetInt64T(), tag << kCodeEntrypointTagShift, 0), "");
      }
    }
#else
    LLVMValueRef offset = LLVMConstInt(GetInt64T(), Code::kInstructionStartOffset - kHeapObjectTag, 0);
    callee = FixTypeTo(callee, GetInt64T(), nullptr);
    callee = LLVMBuildAdd(builder_, callee, offset, "");
    callee = CanonicalizeToPtr(callee, LLVMPointerType(GetInt64T(), 0));
    callee = LLVMBuildLoad2(builder_, GetInt64T(), callee, "");
#endif
  }

  callee = CanonicalizeToPtr(callee, LLVMPointerType(callee_type, 0));

  LLVMValueRef llvm_call = LLVMBuildCall2(builder_, callee_type, callee, args.data(),
                                              static_cast<uint32_t>(args.size()), NodeName(node).c_str());
  LLVMSetInstructionCallConv(llvm_call, cc);
  Bind(node, llvm_call);
}

void LLVMIRBuilder::PrepareCallParams(const CallDescriptor* call_descriptor, base::Vector<const OpIndex>& arguments,
    std::vector<LLVMTypeRef>& param_types, std::vector<LLVMValueRef>& args, LLVMValueRef callee) {
  auto cc = GetLLVMCallConvByDescriptor(call_descriptor);
  size_t param_count = call_descriptor->ParameterCount();
  size_t idx_context = 0;
  bool has_context = false;
  if (cc != LLVMCCallConv) {
    // handle context param
    if (call_descriptor->hasContext(idx_context)) {
      has_context = true;
      auto param_type = ConvertLLVMTypeFromMachineType(call_descriptor->GetParameterType(idx_context));
      param_types.push_back(param_type);
      LLVMValueRef arg = GetLValueOf(*(arguments.begin()+idx_context));
      arg = FixTypeTo(arg, param_type, nullptr);
      args.push_back(arg);
    }
  }

  auto iter(arguments.begin());
  // params in reg
  for (size_t i = 0; i < param_count; i++) {
    CHECK_NE(iter, arguments.end());
    LLVMValueRef arg = GetLValueOf(*iter);
    auto location = call_descriptor->GetInputLocation(i+1);
    if (location.IsRegister() && (!has_context || idx_context != i)) {
      auto param_type = ConvertLLVMTypeFromMachineType(call_descriptor->GetParameterType(i));
      param_types.push_back(param_type);
      arg = FixTypeTo(arg, param_type, nullptr);
      args.push_back(arg);
    }
    ++iter;
  }
  CHECK_EQ(iter, arguments.end());

  // push kfunction param
  if (call_descriptor->kind() == CallDescriptor::kCallJSFunction) {
    param_types.push_back(LLVMPointerType(GetInt64T(), 0));
    LLVMValueRef arg = FixTypeTo(callee, GetInt64T(), nullptr);
    args.push_back(CanonicalizeToPtr(arg, LLVMPointerType(GetInt64T(), 0)));
  }

  iter = arguments.begin();
  std::vector<LLVMValueRef> stack_args;
  std::vector<LLVMTypeRef> stack_param_types;
  // params in stack
  for (size_t i = 0; i < param_count; i++) {
    CHECK_NE(iter, arguments.end());
    LLVMValueRef arg = GetLValueOf(*iter);
    auto location = call_descriptor->GetInputLocation(i+1);
    if (!location.IsRegister()) {
      auto param_type = ConvertLLVMTypeFromMachineType(call_descriptor->GetParameterType(i));
      stack_param_types.push_back(param_type);
      arg = FixTypeTo(arg, param_type, nullptr);
      stack_args.push_back(arg);
    }
    ++iter;
  }
  if (call_descriptor->GetStackArgumentOrder() == StackArgumentOrder::kDefault) {
    args.insert(args.end(), stack_args.rbegin(), stack_args.rend());
    param_types.insert(param_types.end(), stack_param_types.rbegin(), stack_param_types.rend());
  } else {
    CHECK(call_descriptor->GetStackArgumentOrder() == StackArgumentOrder::kJS);
    args.insert(args.end(), stack_args.begin(), stack_args.end());
    param_types.insert(param_types.end(), stack_param_types.begin(), stack_param_types.end());
  }

  if (call_descriptor->ParameterSlotCount() % 2 != 0) {
    // stack parameter needs to align to 16 bytes, force set padding slot with 0
    // to make gc work pass with this slot
    args.push_back(LLVMConstInt(GetInt64T(), 0, 0));
    param_types.push_back(GetInt64T());
  }

  CHECK_EQ(iter, arguments.end());
  CHECK_EQ(param_count +
           // fix with kFunction
           (call_descriptor->kind() == CallDescriptor::kCallJSFunction ? 1 : 0) + 
           // fix with padding stack slot
           call_descriptor->ParameterSlotCount() % 2, 
           args.size());
  CHECK(llvmCallConvList_->verifyCallConv(cc, call_descriptor));
}

void LLVMIRBuilder::HandleTailCall(OpIndex node) {
  VisitTailCall(node);
}

void LLVMIRBuilder::VisitTailCall(OpIndex node) {
  const turboshaft::Operation& op = graph_.Get(node);
  const TailCallOp& call = op.Cast<TailCallOp>();
  const CallDescriptor* call_descriptor = call.descriptor->descriptor;

   if (call_descriptor->NeedsFrameState()) {
    UNIMPLEMENTED();
  }

  if (call_descriptor->kind() == CallDescriptor::kCallAddress) {
    DCHECK(!linkage_->GetIncomingDescriptor()->IsJSFunctionCall());
    if (LLVMGetTypeKind(LLVMTypeOf(GetLValueOf(call.callee()))) != LLVMIntegerTypeKind) {
      UNIMPLEMENTED();
    }
  } else if (call_descriptor->kind() != CallDescriptor::kCallCodeObject) {
    UNIMPLEMENTED();
  }

  size_t return_count = call_descriptor->ReturnCount();
  CHECK(return_count == 1 || return_count == 0);
  LLVMTypeRef return_type = (return_count == 0) ?
    GetVoidT() : ConvertLLVMTypeFromMachineType(call_descriptor->GetReturnType(0));
  
  std::vector<LLVMTypeRef> param_types;
  std::vector<LLVMValueRef> args;
  base::Vector<const OpIndex> arguments = call.arguments();
  LLVMValueRef callee = GetLValueOf(call.callee());
  PrepareCallParams(call_descriptor, arguments, param_types, args, callee);

  //false: not allow va-args
  LLVMTypeRef callee_type = LLVMFunctionType(return_type, param_types.data(),
                                             static_cast<uint32_t>(param_types.size()), false);
  const ConstantOp* constant = graph_.Get(call.callee()).TryCast<ConstantOp>();
  if (call_descriptor->kind() == CallDescriptor::kCallCodeObject && 
      (constant == nullptr || constant->kind != ConstantOp::Kind::kHeapObject)) {
#ifdef V8_ENABLE_SANDBOX
    LLVMValueRef offset = LLVMConstInt(GetInt64T(), Code::kSelfIndirectPointerOffset - kHeapObjectTag, 0);
    callee = FixTypeTo(callee, GetInt64T(), nullptr);
    callee = LLVMBuildAdd(builder_, callee, offset, "");
    callee = CanonicalizeToPtr(callee, LLVMPointerType(GetInt32T(), 0));
    callee = LLVMBuildLoad2(builder_, GetInt32T(), callee, "");
    callee = LLVMBuildLShr(builder_, callee, LLVMConstInt(GetInt32T(), kCodePointerHandleShift, 0), "");
    callee = LLVMBuildShl(builder_, callee, LLVMConstInt(GetInt32T(), kCodePointerTableEntrySizeLog2, 0), "");
    callee = LLVMBuildZExt(builder_, callee, GetInt64T(), "");
    ExternalReference ref = ExternalReference::code_pointer_table_address();
    LLVMValueRef l_ref = nullptr;
    if (ref.IsIsolateFieldId()) {
      UNREACHABLE();
    }
    if (MacroAssemblerBase::IsAddressableThroughRootRegister(data_->isolate(), ref)) {
      UNREACHABLE();
    } else {
      int32_t offset = 
          MacroAssemblerBase::RootRegisterOffsetForExternalReferenceTableEntry(data_->isolate(), ref);
      LLVMValueRef l_object_offset = LLVMConstInt(GetInt64T(), offset, 1);
      LLVMValueRef l_object_address = LLVMBuildGEP2(builder_, GetInt8T(), GetRoot(),
                                                      &l_object_offset, 1, "");
      l_ref = LLVMBuildLoad2(builder_, GetInt64T(), l_object_address, "");
    }
    callee = LLVMBuildAdd(builder_, l_ref, callee, "");
    callee = CanonicalizeToPtr(callee, LLVMPointerType(GetInt64T(), 0));
    callee = LLVMBuildLoad2(builder_, GetInt64T(), callee, "");
    if (call_descriptor->RequiresEntrypointTagForCall()) {
      uint64_t tag = static_cast<uint64_t>(call_descriptor->shifted_tag());
      if (tag != 0) {
        callee = LLVMBuildXor(builder_, callee, LLVMConstInt(GetInt64T(), tag << kCodeEntrypointTagShift, 0), "");
      }
    }
#else
    LLVMValueRef offset = LLVMConstInt(GetInt64T(), Code::kInstructionStartOffset - kHeapObjectTag, 0);
    callee = FixTypeTo(callee, GetInt64T(), nullptr);
    callee = LLVMBuildAdd(builder_, callee, offset, "");
    callee = CanonicalizeToPtr(callee, LLVMPointerType(GetInt64T(), 0));
    callee = LLVMBuildLoad2(builder_, GetInt64T(), callee, "");
#endif       
  }

  callee = CanonicalizeToPtr(callee, LLVMPointerType(callee_type, 0));
  LLVMValueRef llvm_call = LLVMBuildCall2(builder_, callee_type, callee, args.data(),
                                              static_cast<uint32_t>(args.size()), NodeName(node).c_str());
  LLVMSetTailCall(llvm_call, true);
  LLVMSetTailCallKind(llvm_call, LLVMTailCallKindTail);

  LLVMBuildRet(builder_, llvm_call);
  auto cc = GetLLVMCallConvByDescriptor(call_descriptor);
  LLVMSetInstructionCallConv(llvm_call, cc);
  Bind(node, llvm_call);
}

void LLVMIRBuilder::HandleDidntThrow(OpIndex node) {
  VisitDidntThrow(node);
}

void LLVMIRBuilder::VisitDidntThrow(OpIndex node) {
  const turboshaft::Operation& op = graph_.Get(node);
  const DidntThrowOp& not_throw = op.Cast<DidntThrowOp>();
  if (current_block_->begin() == node) {
    UNIMPLEMENTED();
  } else {
    LLVMValueRef call = GetLValueOf(not_throw.throwing_operation());
    Bind(node, call);
  }
}

void LLVMIRBuilder::HandleAbortCSADcheck(OpIndex node) {
  VisitAbortCSADcheck(node);
}

void LLVMIRBuilder::VisitAbortCSADcheck(OpIndex node) {
#ifdef DEBUG
  return;
#else
  UNIMPLEMENTED();
#endif // DEBUG
}

void LLVMIRBuilder::HandleDebugBreak(OpIndex node) {
  VisitDebugBreak(node);
}

void LLVMIRBuilder::VisitDebugBreak(OpIndex node) {
  LLVMTypeRef param_types[] = {};
  LLVMTypeRef asm_func_type = LLVMFunctionType(GetVoidT(), param_types, 0, 0);
  LLVMValueRef inline_asm = 
      LLVMGetInlineAsm(asm_func_type, "brk #0", strlen("brk #0"), "", 0, 1, 0, LLVMInlineAsmDialectATT, 0);

  auto call = LLVMBuildCall2(builder_, asm_func_type, inline_asm, nullptr, 0, "");
  SetGCLeafFunction(call);
}

void LLVMIRBuilder::HandleUnreachable(OpIndex node){
  VisitUnreachable(node);
}

void LLVMIRBuilder::VisitUnreachable(OpIndex node) {
  //Terminated
  LLVMBuildUnreachable(builder_);
}

void LLVMIRBuilder::HandleSwitch(OpIndex node) {
  VisitSwitch(node);
}

void LLVMIRBuilder::VisitSwitch(OpIndex node) {
  const turboshaft::Operation& op = graph_.Get(node);
  const SwitchOp& swtch = op.Cast<SwitchOp>();

  LLVMMetadataRef branch_weights = LLVMMDStringInContext2(context_, "branch_weights", 14);
  LLVMMetadataRef init_weight = LLVMValueAsMetadata(LLVMConstInt(GetInt32T(), 10, 0));
  LLVMMetadataRef false_weight = LLVMValueAsMetadata(LLVMConstInt(GetInt32T(), 2, 0));
  LLVMMetadataRef true_weight = LLVMValueAsMetadata(LLVMConstInt(GetInt32T(), 50, 0));
  std::vector<LLVMMetadataRef> mds(swtch.cases.size() + 1, init_weight);
  mds.insert(mds.begin(), branch_weights);

  LLVMValueRef value = GetLValueOf(swtch.input());
  if (LLVMGetTypeKind(LLVMTypeOf(value)) != LLVMIntegerTypeKind) {
    UNIMPLEMENTED();
  }

  LLVMBasicBlockRef default_bb = GetLBlockOf(swtch.default_case);
  LLVMValueRef llvm_swtch = LLVMBuildSwitch(builder_, value, default_bb, static_cast<uint32_t>(swtch.cases.size()));
  if (swtch.default_hint == BranchHint::kFalse) {
    mds[1] = false_weight;
  } else if (swtch.default_hint == BranchHint::kTrue) {
    mds[1] = true_weight;
  }

  for (size_t i = 0; i < swtch.cases.size(); ++i) {
    const SwitchOp::Case& c = swtch.cases[i];
    if (c.hint == BranchHint::kFalse) {
      mds[i + 2] = false_weight;
    } else if (c.hint == BranchHint::kTrue) {
      mds[i + 2] = true_weight;
    }
    LLVMAddCase(llvm_swtch, LLVMConstInt(GetInt32T(), c.value, false), GetLBlockOf(c.destination));
  }

  LLVMMetadataRef metadata = LLVMMDNodeInContext2(context_, mds.data(), mds.size());
  LLVMValueRef metadata_value = LLVMMetadataAsValue(context_, metadata);
  LLVMSetMetadata(llvm_swtch, LLVMGetMDKindID("prof", 4), metadata_value); // 4: length of "prof"

  Bind(node, llvm_swtch);
}

void LLVMIRBuilder::HandleOverflowCheckedBinop(OpIndex node) {
  VisitOverflowCheckedBinop(node);
}

void LLVMIRBuilder::VisitOverflowCheckedBinop(OpIndex node) {
  const turboshaft::Operation& op = graph_.Get(node);
  const auto& binop = op.Cast<OverflowCheckedBinopOp>();

  LLVMValueRef left = GetLValueOf(binop.left());
  LLVMValueRef right = GetLValueOf(binop.right());
  LLVMValueRef args[] = { left, right };
  LLVMValueRef intrinsic = nullptr;
  LLVMTypeRef intrinsic_type = nullptr;
  if (binop.rep == WordRepresentation::Word32()) {
    CHECK(LLVMTypeOf(left) == GetInt32T());
    CHECK(LLVMTypeOf(right) == GetInt32T());
    switch (binop.kind) {
      case OverflowCheckedBinopOp::Kind::kSignedAdd: {
        intrinsic = LLVMGetNamedFunction(module_, "llvm.sadd.with.overflow.i32");
        LLVMTypeRef params[] = { GetInt32T(), GetInt32T() };
        LLVMTypeRef strct[] = {GetInt32T(), GetInt1T() };
        LLVMTypeRef return_type = LLVMStructTypeInContext(context_, strct, 2, 0);
        intrinsic_type = LLVMFunctionType(return_type, params, 2, 0);
        if (!intrinsic) {
          intrinsic = LLVMAddFunction(module_, "llvm.sadd.with.overflow.i32", intrinsic_type);
        }
        break;
      }
      case OverflowCheckedBinopOp::Kind::kSignedMul: {
        intrinsic = LLVMGetNamedFunction(module_, "llvm.smul.with.overflow.i32");
        LLVMTypeRef params[] = { GetInt32T(), GetInt32T() };
        LLVMTypeRef strct[] = {GetInt32T(), GetInt1T() };
        LLVMTypeRef return_type = LLVMStructTypeInContext(context_, strct, 2, 0);
        intrinsic_type = LLVMFunctionType(return_type, params, 2, 0);
        if (!intrinsic) {
          intrinsic = LLVMAddFunction(module_, "llvm.smul.with.overflow.i32", intrinsic_type);
        }
        break;
      }
      case OverflowCheckedBinopOp::Kind::kSignedSub: {
        intrinsic = LLVMGetNamedFunction(module_, "llvm.ssub.with.overflow.i32");
        LLVMTypeRef params[] = { GetInt32T(), GetInt32T() };
        LLVMTypeRef strct[] = {GetInt32T(), GetInt1T() };
        LLVMTypeRef return_type = LLVMStructTypeInContext(context_, strct, 2, 0);
        intrinsic_type = LLVMFunctionType(return_type, params, 2, 0);
        if (!intrinsic) {
          intrinsic = LLVMAddFunction(module_, "llvm.ssub.with.overflow.i32", intrinsic_type);
        }
        break;
      }
    }
  } else {
    CHECK_EQ(binop.rep, WordRepresentation::Word64());
    CHECK(LLVMTypeOf(left) == GetInt64T());
    CHECK(LLVMTypeOf(right) == GetInt64T());
    switch (binop.kind) {
      case OverflowCheckedBinopOp::Kind::kSignedAdd: {
        intrinsic = LLVMGetNamedFunction(module_, "llvm.sadd.with.overflow.i64");
        LLVMTypeRef params[] = { GetInt64T(), GetInt64T() };
        LLVMTypeRef strct[] = {GetInt64T(), GetInt1T() };
        LLVMTypeRef return_type = LLVMStructTypeInContext(context_, strct, 2, 0);
        intrinsic_type = LLVMFunctionType(return_type, params, 2, 0);
        if (!intrinsic) {
          intrinsic = LLVMAddFunction(module_, "llvm.sadd.with.overflow.i64", intrinsic_type);
        }
        break;
      }
      case OverflowCheckedBinopOp::Kind::kSignedMul: {
        intrinsic = LLVMGetNamedFunction(module_, "llvm.smul.with.overflow.i64");
        LLVMTypeRef params[] = { GetInt64T(), GetInt64T() };
        LLVMTypeRef strct[] = {GetInt64T(), GetInt1T() };
        LLVMTypeRef return_type = LLVMStructTypeInContext(context_, strct, 2, 0);
        intrinsic_type = LLVMFunctionType(return_type, params, 2, 0);
        if (!intrinsic) {
          intrinsic = LLVMAddFunction(module_, "llvm.smul.with.overflow.i64", intrinsic_type);
        }
        break;
      }
      case OverflowCheckedBinopOp::Kind::kSignedSub: {
        intrinsic = LLVMGetNamedFunction(module_, "llvm.ssub.with.overflow.i64");
        LLVMTypeRef params[] = { GetInt64T(), GetInt64T() };
        LLVMTypeRef strct[] = {GetInt64T(), GetInt1T() };
        LLVMTypeRef return_type = LLVMStructTypeInContext(context_, strct, 2, 0);
        intrinsic_type = LLVMFunctionType(return_type, params, 2, 0);
        if (!intrinsic) {
          intrinsic = LLVMAddFunction(module_, "llvm.ssub.with.overflow.i64", intrinsic_type);
        }
        break;
      }
    }
  }

  LLVMValueRef result = LLVMBuildCall2(builder_, intrinsic_type, intrinsic, args, 2, "");
  LLVMValueRef llvm_res = LLVMBuildExtractValue(builder_, result, 0, NodeName(node).c_str());

  Bind(node, llvm_res);
}

void LLVMIRBuilder::HandleProjection(OpIndex node) {
  VisitProjection(node);
}

void LLVMIRBuilder::VisitProjection(OpIndex node) {
  const turboshaft::Operation& op = graph_.Get(node);
  const ProjectionOp& projection = op.Cast<ProjectionOp>();

  LLVMValueRef res = nullptr;
  const Operation& value_op = graph_.Get(projection.input());
  if (value_op.Is<OverflowCheckedBinopOp>()) {
    LLVMValueRef from = GetLValueOf(projection.input());
    LLVMValueRef overflow_from = LLVMGetOperand(from, 0);
    res = LLVMBuildExtractValue(builder_, overflow_from, projection.index, NodeName(node).c_str());
  } else {
    UNIMPLEMENTED();
  }

  Bind(node, res);
}

void LLVMIRBuilder::HandleTuple(OpIndex node) {
  VisitTuple(node);
}

void LLVMIRBuilder::VisitTuple(OpIndex node) {
  // turboshaft meta, no instructions needed
}

}  // namespace internal
}  // namespace v8