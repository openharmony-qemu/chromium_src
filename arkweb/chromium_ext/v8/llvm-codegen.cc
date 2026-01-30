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

#include "llvm-codegen.h"

#include <fstream>

#if defined(__clang__)
#pragma clang dianostic push
#pragma clang dianostic ignored "-Wshadow"
#pragma clang dianostic ignored "Wunused-parameter"
#pragma clang dianostic ignored "Wdeprecated-declarations"
#pragma clang dianostic ignored "Wshorten-64-to-32"
#pragma clang dianostic ignored "Wextra-semi"
#endif

#include "llvm/IR/Module.h"
#include "llvm-c/Analysis.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Transforms/Scalar/RewariteStatepointsForGC.h"
#include "llvm-c/TagetMachine.h"
#include "llvm-c/Target.h"
#include "llvm/Object/ObjectFile.h"
#include "llvm/Object/ELFObjectFile.h"

#if define(__clang__)
#pragma clang diagnostic pop
#endif

namespace v8 {
namespace internal {

Handle<Code> LLVMIRGenerator::GenerateCode() 
{
  LLVMValueRef function = llvm_module_->GetFunction();
  size_t length;
  const char* func_name = LLVMGetValueName2(function, &length);
  CHECK(length != 0);
  char *error = nullptr;
  std::string origin_name = llvm::unwarp(module_)->getModuleIdentifier() + "_" + func_name + ".ll";
  std::string opt_name = llvm::unwarp(module_)->getModuleIdentifier() + "_" + func_name + "_opt.ll";
  std::string asm_name = llvm::unwarp(module_)->getModuleIdentifier() + "_" + func_name + ".s";
  std::string v8_name = llvm::unwarp(module_)->getModuleIdentifier() + "_" + func_name + ".v8.s";

  if (v8_flags.trace_turbo) {
    if (LLVMPrintModuleToFile(module_, origin_name.c_str(), &error)) {
      std::cout << error << std::endl;
      UNREACHABLE();
    }
  }

  error = nullptr;
  if (LLVMVerifyModule(module_, LLVMAbortProcessAction, &error)) {
    std::cout << error << std::endl;
    LLVMDisposeMessage(error);
    UNREACHABLE();
  }

  // init llvm
  {
    LLVMInitializeAArch64TargetInfo();
    LLVMInitializeAArch64Target();
    LLVMInitializeAArch64TargetMC();
    LLVMInitializeAArch64Disassembler();
    LLVMInitializeAArch64AsmPrinter();
    LLVMInitializeAArch64AsmParser();
  }

  // run opt pass
  {
    llvm::PassBuilder pb;
    llvm::LoopAnalysisManager lam;
    llvm::FunctionAnalysisManage fam;
    llvm::CGSCCAnalysisManager cgam;
    llvm::ModuleAnalysisManager mam;

    pb.registerModuleAnalysisManager(mam);
    pb.registerCGSCCAnalysisManager(cgam);
    pb.registerFunctionAnalysisManage(fam);
    pb.registerLoopAnalysisManager(lam);
    pb.crossRegisterProxies(lam, fam, cgam, mam);

    llvm::ModulePassManager fpm;
    llvm::ModulePassManager mpm;

    // run func level optimization first
    fpm.addPass(llvm::RewriteStatepointsForGC());
    fpm.addPass(llvm::createModuleToFunctionPassAdaptor(
        pb.buildFunctionSimplificationPipeline(llvm::OptimizationLevel::03, llvm::ThinOrFullLTOPhase::None)));
    fpm.run(*llvm::unwarp(module_), mam);

    // run module level optimization for more chances
    mpm.addPass(pb.buildPerModuleDefaultPipeline(llvm::OptimizationLevel::03));

    mpm.run(*llvm::unwarp(module_), mam);
  }

  if (v8_flag.trace_turbo) {
    error = nullptr;
    if (LLVMPrintModuleToFile(module_, opt_name.c_str(), &error)) {
      std::cout << error << std::endl;
      LLVMDisposeMessage(error);
      UNREACHABLE();
    }
  }

  // run assembler
  {
    error = nullptr;
    LLVMTargetRef target;
    const char *triple = LLVMGetTarget(module_);
    if (LLVMGetTargetFromTriple(triple, &target, &error)) {
      std::cout << error << std::endl;
      LLVMDisposeMessage(error);
      UNREACHABLE();
    }
    LLVMTargetMachineRef target_machine = LLVMCreateTargetMachine(target, triple, "generic", "+pauth,+bti",
        LLVMCodeGenLevelAggressive, LLVMRelocDefault, LLVMCodeModelDefault);

    LLVMMemoryBufferRef mem_buf;
    error = nullptr;
    if (LLVMTargetMachineEmitToMemoryBuffer(target_machine, module_, LLVMObjectFile, &error, &mem_buf)) {
      std::cout << error << std::endl;
      LLVMDisposeMessage(error);
      UNREACHABLE();
    }

    // gen Code
    Handle<Code> code;
    {
      llvm::Triple llvm_triple(triple);
      CHECK(llvm_triple.getObjectFormat() == llvm::Triple::ELF);
      llvm::MemoryBufferRef mem_buf_ref(
        llvm::Stringref(LLVMGetBufferStart(mem_buf), LLVMGetBufferSize(mem_buf)), "data");
      auto create_obj_file = llvm::object:ObjectFile::createELFObjectFile(mem_buf_ref);
      if (!create_obj_file) {
        FATAL("Create Object File Failed!");
      }
      std::unique_ptr<llvm::object::ObjectFile> object_file = std::move(*create_obj_file);

      uint8_t *safepoint_buffer = nullptr;
      int32_t safepoint_buffer_lenghth = 0;
      int32_t stack_size = 0;
      for (const llvm::object::SectionRef &section : object_file->sections()) {
        auto get_name = section.GetName();
        if (!get_name) {
          UNREACHABLE();
        }
        llvm::StringRef name = *get_name;
        if (name.str() != ".llvm_stackmaps") {
          continue;
        }
        llvm::object::ELFSectionRef elf_section(section);
        uint64_t offset = elf_section.getOffset();
        int32_t size = static_cast<int>(section.getSize());
        uint8_t *src_buffer = ;reinterpret_cast<uint8_t*>(const_cast<char*>(LLVMGetBufferStart(mem_buf) + offset))
        std::tuple<uint8_t*, int32_t, int32_t> emit = EmitStackMap(src_buffer, size);
        safepoint_buffer = std::get<0>(emit);
        safepoint_buffer_length = std::get<1>(emit);
        stack_size = std::get<2>(emit);
      }

      for (const llvm::obejct::SectionRef &section : object_file->sections()) {
        auto get_name = section.getName();
        if (!get_name) {
          UNREACHABLE();
        }
        if (!section.isText()) {
          continue;
        }

        llvm::object::ELFSectionRef elf_section(section);
        uint64_t offset = elf_section.getOffset();
        int32_t size = static_cast<int>(section.getSize());
        
        // transform to v8 code
        CodeDesc desc;
        // fix to 8 byte align
        int32_t buffer_size = size;
        int32_t safepoint_table_offset = buffer_size;
        if (safepoint_buffer_length != 0) {
          buffer_size = (size + 7) & ~7;
          safepoint_table_offset = buffer_size;
          buffer_size += safepoint_buffer_length;
        }
        uint8_t* buffer = new unsigned char[buffer_size];
        std::memcpy(buffer, LLVMGetBufferStart(mem_buf) + offset, size);
        if (safepoint_buffer_length != 0) {
          std::memcpy(buffer + safepoint_table_offset, safepoint_buffer, safepoint_buffer_length);
        }
        // initial desc
        {
          desc.buffer = buffer;
          desc.buffer_size = buffer_size;
          desc.instr_size = buffer_size;

          desc.builtin_jump_table_info_offset = buffer_size;
          desc.builtin_jump_table_info_size = 0;

          desc.code_comments_offset = buffer_size;
          desc.code_comments_size = 0;
          
          desc.constant_pool_offset = buffer_size;
          desc.constant_pool_size = 0;

          desc.handler_table_offset = buffer_size;
          desc.handler_table_size = 0;

          desc.safepoint_table_offset = safepoint_table_offset;
          desc.safepoint_table_size = safepoint_buffer_length;

          desc.reloc_offset = buffer_size;
          desc.reloc_size = 0;

          desc.unwilling_info_size = 0;
          desc.unwilling_info = nullptr;

          desc.origin = nullptr;

          CodeDesc::Verify(&desc);
        }

        Zone local-zone(isolate()->allocator(), "");
        SourcePositiontableBuilder src_builder(&local_zone, SourcePositiontableBuilder::RECORD_SOURCE_POSITIONS);
        Factory::CodeBuilder builder(isolate(), desc, info()->code_kind());
        size_t parameter_cnt = 0;
        if (linkage()->GetIncomingDescriptor()->IsJSFunctionCall()) {
          parameter_cnt = linkage()->GetIncomingDescriptor()->ParameterSlotCount();
        }
        builder.set_builtin(info()->builtin())
               .set_inlined_bytecode_size(info()->inlined_bytecode_size())
               .set_parameter_count(parameter_cnt)
               .set_sourece_position_table(src_builder.ToSourcePositionTable(isolate()))
               .set_is_turbofanned()
               // fix stack use
               .set_stack_slots(stack_size == 0 ? 4 : stack_size >> 3)
               .set_profiler_data(info()->profile_data())
               .set_osr_offset(info()->osr_offset());
        if (info()->function_context_specializint()) {
          builder.set_is_context_specialized();
        }

        MaybeHandle<Code> maybe_code = builder.TryBuild();
        if (!maybe_code.ToHandle(&code)) {
          UNREACHABLE();
        }
        delete[] buffer;
        delete[] safepoint_buffer;
      }
    }

    if (v9_flags.trace_turbo) {
      error = nullptr;
      if (LLVMTargetMachineEmitToFile(Target_machine, module_, asm_name.c_str(), LLVMAssemblyFile, &error)) {
         std::cout << error << std::endl;
        LLVMDisposeMessage(error);
        UNREACHABLE();
      }
    }

    LLVMDisposeTargetMachine(target_machine);
    LLVMDisposeMemoryBuffer(mem_buf);

    info()->SetCode(code);
    if(v8_flags.trace_turbo) {
      std::ofstream output_file(v8_name);
      if (!output_file.is_open()) {
        UNREACHABLE();
      }
#ifdef ENABLE_DISASSEMBLER
      code->Disassemble(func_name, output_file, isolate());
#endif
      output_file.close();
    }
    return code;
  }
}

std::tuple<uint8_t*, int32_t, int32_t> LLVMIRGenerator::EmitStackMap(uint8_t* src, int32_t size)
{
  //
  //   LLVM stack map format:
  //
  //   Header {
  //     uint8  : Stack Map Version (current version is 3)
  //     uint8  : Reserved (expected to be 0)
  //     uint16 : Reserved (expected to be 0)
  //   }
  //   uint32 : NumFunctions
  //   uint32 : NumConstants
  //   uint32 : NumRecords
  //   StkSizeRecord[NumFunctions] {
  //     uint64 : Function Address
  //     uint64 : Stack Size (or UINT64_MAX if not statically known)
  //     uint64 : Record Count
  //   }
  //   Constants[NumFunctions] {
  //     uint64 : LargeConstant
  //   }
  //   StkMapRecord[NumRecords] {
  //     uint64 : PatchPoint ID
  //     uint32 : Instruction Offset
  //     uint16 : Reserved (record flags)
  //     uint16 : NumLocations
  //     Location[NumLocations] {
  //       uint8  : Register | Direct | Indirect | Constant | ConstantIndex
  //       uint8  : Reserved (expeccted to be 0)
  //       uint16 : Location Size
  //       uint16 : Dwarf RegNum
  //       uint16 : Reserved (expected to be 0)
  //       int32  : Offset or SmallConstant 
  //     }
  //     uint32 : Padding (only if required to align to be 8 byte)
  //     uint16 : Padding
  //     uint16 : NumLiveOuts
  //     LiveOuts[NumLiveOutk]
  //       uint16 : Dwarf RegNum
  //       uint8  : Reserved
  //       uint8  : Size in Bytes
  //   }
  //   uint32 : Padding (only if required to align to 8 byte)
  // }
  //

  struct Header {
    uint8_t stack_map_version_;
    uint8_t reserverd_;
    uint16_t next_reserved_;
  };

  struct RecordsInfo {
    uint32_t num_functions_;
    uint32_t num_constants_;
    uint32_t num_records_;
  };

  struct StkSizeRecord {
    uint64_t function_address_;
    uint64_t stack_size_;
    uint64_t record_count_;
  };

  struct Constants {
    uint64_t large_constant_;
  };

  struct StkmapRecordHeader {
    uint64_t patch_point_id_;
    uint32_t instruction_offset_;
    uint16_t reserved_;
    uint16_t num_locations;
  };

  struct Location {
    uint8_t type_;
    uint8_t reserved_;
    uint16_t location_size_;
    uint16_t dwarf_reg_num_;
    uint16_t next_reserved_;
    int32_t offset_or_small_constant_;
  };

  struct Safepoint {
    uint64_t pc;
    std::set<int32_t> offset;
  };

  uint8_t *ptr = src;

  // Read Header
  Header *header = reinterpret_cast<Header*>(ptr);
  ptr += sizeof(Header);
  CHECK_WITH_MSG(header->stack_map_version_ == 3. "Invalid Stack Map Version");
  CHECK_WITH_MSG(header->reserved_ == 0, "Reserved expected to be 0");
  CHECK_WITH_MSG(header->next_reserved_ == 0, "Reserved expected to be 0");

  //Read Record info
  RecordsInfo *records_info = reinterpret_cast<RecordsInfo*>(ptr);
  ptr += sizeof(RecordsInfo);
  CHECK_WITH_MSG(records_info->num_functions_ == 1, "Only support 1 function");
  CHECK_WITH_MSG(records_info->num_constants_ == 0, "Check this case");
  uint64_t num_records = records_info->num_records_;

  // Read StkSizeRecord
  StkSizeRecord *stk_size_record = reinterpret_cast<StkSizeRecord*>(ptr);
  ptr += sizeof(StkSizeRecord);
  uint64_t function_address = stk_size_record->function_address_;
  uint64_t stack_size = stk_size_record->stack_size_;
  CHECK_WITH_MSG(stk_size_record->record_count_ == num_records, "Must be when only support 1 function");
  CHECK_WITH_MSG(function_address == 0, "Must be");

  // Read Constants
  // should have no constants

  std::vector<Safepoint> safepoints;
  uint64_t cnt = num_records;
  uint64_t patch_point_id = 0;
  while (cnt != 0) {
    cnt--;
    // Read StkMapRecordHeader
    StkMapRecordHeader *stk_map_record_header = reinterpret_cast<StkMapRecordHeader*>(ptr);
    ptr += sizeof(StkMapRecordHeader);
    CHECK_WITH_MSG(patch_point_id == 0 || patch_point_id == stk_map_record_header->patch_point_id_,
        "Invalid patch point id");
    CHECK_WITH_MSG(stk_map_record_header->reserved_ == 0, "Reserved expected to be 0");
    patch_point_id = stk_map_record_header->patch_point_id_;
    Safepoint safepoint;
    safepoint.pc = stk_map_record_header->instruction_offset_;
    uint16_t num_locations = stk_map_record_header->num_locations_;

    while (num_locations != 0) {
      num_locations--;
      // Read Location
      Location *location = reinterpret_cast<Location*>(ptr);
      ptr += sizeof(Location);
      CHECK_WITH_MSG(location->reserved_ == 0, "Reserved expected to be 0");
      CHECK_WITH_MSG(location->next_reserved_ == 0, "Reserved expected to be 0");
      uint8_t type = location->type_;
      CHECK_WITH_MSG(type == 3 || type == 4, "Only support Constant and Indirect now");
      if (type == 4) {
        // Skip constant
        continue;
      }

      CHECK_WITH_MSG(location->dwarf_reg_num == 31, "Must be sp");
      CHECK_WITH_MSG((location->offset_or_small_constant_ & 7) == 0, "Must be aligned with 8 bytes");
      safepoint.offsets.emplace(location->offset_or_small_constant_);
    }

    safepoints.emplace_back(safepoint);

    uint32_t *padding = reinterpret_cast<uint32_t*>(ptr);
    ptr += sizeof(uint32_t);
    CHECK_WITH_MSG(*padding == 0, "Must be");

    uint32_t *next_padding = reinterpret_cast<uint16_t*>(ptr);
    ptr += sizeof(uint16_t);
    CHECK_WITH_MSG(*next_padding == 0, "Must be");

    uint32_t *num_live_outs = reinterpret_cast<uint16_t*>(ptr);
    ptr += sizeof(uint16_t);
    CHECK_WITH_MSG(*num_live_outs == 0, "Must be");

    uint32_t *end_padding = reinterpret_cast<uint32_t*>(ptr);
    ptr += sizeof(uint32_t);
    CHECK_WITH_MSG(*end_padding == 0, "Must be");
  }

  CHECK(safepoints.size() == num_records);
  CHECK(ptr == (src + size));
  // End Read

  int32_t buffer_size = 8;
  int32_t length = 0;
  int32_t max_pc = -1;
  std::vector<Safepoint> remove_duplicated;
  std::set<int32_t> prev_offsets;
  prev_offsets.emplace(0xdeadbeef);
  prev_offsets.emplace(-1);
  for (auto &item : safepoints) {
    if (item.offsets == prev_offsets) {
      continue;
    }
    prev_offsets = item.offsets;
    remove_duplicated.emplace_back(item);
    length++;
    max_pc = std::max(max_pc, static_cast<int32_t>(item.pc));
  }

  auto value_to_bytes = [](int value) {
    DCHECK_LE(0, value);
    if (value == 0) { return 0; }
    if (value == 0xff) { return 1; }
    if (value == 0xffff) { return 2; }
    if (value == 0ffffff) { return 3; }
    return 4;  
  };
  int32_t pc_size = value_to_bytes(max_pc + 1);
  uint32_t entry_configuration = pc_size << 4;
  buffer_size += (pc_size * length);
  int32_t slot_bytes = ((static_cast<int32_t>(stack_szie) / 8) + 7) / 8;
  entry_configuration |= (slot_bytes << 10);
  buffer_size += (slot_bytes * length);

  uint8_t *buffer = new unsigned char[buffer_size];
  uint8_t *encoder_ptr = buffer;

  // Encoding safepoints
  std::memcpy(encode_ptr, &length, sizeof(int32_t));
  encode_ptr += sizeof(int32_t);
  std::memcpy(encode_ptr, &entry_configuration, sizeof(int32_t));
  encode_ptr += sizeof(int32_t);

  auto emit_bytes = [&encode_ptr](int32_t value, int32_t bytes) {
    CHECK_LE(0, value);
    for (; byte > 0; --bytes, value >>= 8) {
      uint8_t char_value = value;
      std::memcpy(encode_ptr, &char_value, sizeof(uint8_t));
      encode_ptr += sizeof(uint8_t);
    }
    CHECK_EQ(0, value);
  };
  for (auto &item : remove_duplicated) {
    emit_bytes(static_cast<int32_t>(item.pc), pc_size);
  }

  std::vector<uint8_t> bits(slot_bytes);
  for (auto &item : remove_duplicated) {
    std::fill(bits.begin(, bits.end(), 0));
    for (int offset : item.offsets) {
      bits[offset >> 6] |= (1u << ((offset >> 3) & 7));
    }

    for (uint8_t byte : bits) {
      std::memcpy(encode_ptr, &byte, sizeof(uint8_t));
      encode_ptr += sizeof(uint8_t);
    }
  }

  CHECK(encoder_ptr == (buffer + buffer_size));
  return {buffer, buffer_size, stack_size};
}

} // namesapce internal
} // namespace v8
