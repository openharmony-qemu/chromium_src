# Copyright (c) 2025 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import errno
import os
import subprocess
import sys

USE_PTY = "linux" in sys.platform
if USE_PTY:
    import pty 


def _call_with_output(cmd:str):
    sys.stderr.write(f"# {cmd}\n")
    sys.stderr.flush()
    parent, child = pty.openty()
    p = subprocess.Popen(cmd, shell=True, stdin=child, stdout=child, stderr=child, close_fds=True)
    os.close(child)
    output = []
    try:
        while True:
            try:
                data = os.read(parent, 512).decode('utf-8', errors='replace')
            except OSError as e:
                if e.errno != errno.EIO:
                    raise
                break
            else:
                if not data:
                    break
                sys.stderr.write(data)
                sys.stderr.flush()
                output.append(data)
    finally:
        os.close(parent)
        p.wait()
    return p.returncode


def main():
    is_debug = sys.argv[1]
    build_dir = "build_debug" if f'{is_debug}'.lower() == 'true' else "build_release"
    tools_dir =  "build_tools_debug" if f'{is_debug}'.lower() == 'true' else "build_tools_release"
    de_or_re = "Debug" if f'{is_debug}'.lower() == 'true' else "Release"
    v8_path = os.path.dirname(os.path.realpath(__file__))
    obs_path = f"{v8_path}/../../../third_party/llvm-for-jsvm/llvm/"
    make_tools = "make llvm-tblgen -j32"
    make_targets = ("make -j32 "
                    "LLVMAArch64AsmParser "
                    "LLVMAArch64CodeGen "
                    "LLVMAArch64Desc "
                    "LLVMAArch64Disassembler "
                    "LLVMAArch64Info "
                    "LLVMAArch64Utils "
                    "LLVMAggressiveInstCombine "
                    "LLVMAnalysis "
                    "LLVMAsmParser "
                    "LLVMAsmPrinter "
                    "LLVMBinaryFormat "
                    "LLVMBitReader "
                    "LLVMBitstreamReader "
                    "LLVMBitWriter "
                    "LLVMCFGuard "
                    "LLVMCodeGen "
                    "LLVMCodeGenData "
                    "LLVMCodeGenTypes "
                    "LLVMCore "
                    "LLVMCoroutines "
                    "LLVMCoverage "
                    "LLVMDebugInfoBTF "
                    "LLVMDebugInfoCodeView "
                    "LLVMDebugInfod "
                    "LLVMDebugInfoDWARF "
                    "LLVMDebugInfoGSYM "
                    "LLVMDebugInfoLogicalView "
                    "LLVMDebugInfoMSF "
                    "LLVMDebugInfoPDB "
                    "LLVMDemangle "
                    "LLVMExecutionEngine "
                    "LLVMExtensions "
                    "LLVMFileCheck "
                    "LLVMFrontendDriver "
                    "LLVMFrontendHLSL "
                    "LLVMFrontendOffloading "
                    "LLVMFrontendOpenACC "
                    "LLVMFrontendOpenMP "
                    "LLVMFuzzCLI "
                    "LLVMFuzzMutate "
                    "LLVMGlobalISel "
                    "LLVMHipStdPar "
                    "LLVMInstCombine "
                    "LLVMInstrumentation "
                    "LLVMInterfaceStub "
                    "LLVMInterpreter "
                    "LLVMipo "
                    "LLVMIRPrinter "
                    "LLVMIRReader "
                    "LLVMJITLink "
                    "LLVMLibDriver "
                    "LLVMLinker "
                    "LLVMLTO "
                    "LLVMMC "
                    "LLVMMCA "
                    "LLVMMCDisassembler "
                    "LLVMMCJIT "
                    "LLVMMCParser "
                    "LLVMMIRParser " 
                    "LLVMObjCARCOpts "
                    "LLVMObjCopy "
                    "LLVMObject "
                    "LLVMObjectYAML "
                    "LLVMOption "
                    "LLVMPasses "
                    "LLVMProfileData "
                    "LLVMRemarks "
                    "LLVMRuntimeDyld "
                    "LLVMSandboxIR "
                    "LLVMScalarOpts " 
                    "LLVMSelectionDAG "
                    "LLVMSupport "
                    "LLVMSymbolize "
                    "LLVMTableGen "
                    "LLVMTableGenBasic "
                    "LLVMTableGenCommon "
                    "LLVMTarget "
                    "LLVMTargetParser "
                    "LLVMTextAPI "
                    "LLVMTextAPIBinaryReader "
                    "LLVMTransformUtils "
                    "LLVMVectorize ")
    if os.path.exists(f'{obs_path}{build_dir}'):
        cmd = f"cd {obs_path}{build_dir} && {make_targets}"
    else:
        cmake_tools = f"cmake -S ./ -B {tools_dir} -DCMAKE_BUILD_TYPE={de_or_re} -DLLVM_TARGET_TO_BUILD=AArch64 -DLLVM_INCLUDE_UTILS=OFF -DLLVM_INCLUDE_TESTS=OFF -DLLVM_INCLUDE_TOOLS=OFF -DLLVM_INCLUDE_RUNTIME=OFF -DLLVM_INCLUDE_EXAMPLES=OFF -DLLVM_INCLUDE_BENCHMARKS=OFF -DLLVM_ENABLE_OCAMLDOC=OFF -DLLVM_ENABLE_BINDINGS=OFF -DLLVM_INCLUDE_DOCS=OFF -DLLVM_USE_SPLIT_DWARF=ON"
        cmake_targets = (f"cmake -S ./ -B {build_dir} " 
                    "-DCMAKE_BUILD_TYPE={de_or_re} " 
                    "-DLLVM_TARGET_TO_BUILD=AArch64 "
                    "-DLLVM_INCLUDE_UTILS=OFF "
                    "-DLLVM_INCLUDE_TESTS=OFF "
                    "-DLLVM_INCLUDE_TOOLS=OFF "
                    "-DLLVM_INCLUDE_RUNTIME=OFF "
                    "-DLLVM_INCLUDE_EXAMPLES=OFF "
                    "-DLLVM_INCLUDE_BENCHMARKS=OFF "
                    "-DLLVM_ENABLE_OCAMLDOC=OFF " 
                    "-DLLVM_ENABLE_BINDINGS=OFF "
                    "-DLLVM_INCLUDE_DOCS=OFF "
                    "-DLLVM_USE_SPLIT_DWARF=ON "
                    f"-DLLVM_NATIVE_TOOL_DIR=`pwd`/{tools_dir}/bin "
                    "-DCMAKE_SYSROOT=`pwd`/../../../build/linux/debian_bullseye_amd64-sysroot "
                    "-DCMAKE_C_COMPILER=`pwd`/../../../ohos_sdk/openharmony/native/llvm/bin/clang "
                    "-DCMAKE_CXX_COMPILER=`pwd`/../../../ohos_sdk/openharmony/native/llvm/bin/clang++ "
                    "-DCMAKE_ASM_COMPILER=`pwd`/../../../ohos_sdk/openharmony/native/llvm/bin/clang "
                    f"-DCMAKE_CXX_FLAGS=\"${{CMAKE_CXX_FLAGS}} "
                        "-I`pwd`/../../../buildtools/third_party/libc++"
                        "-isystem`pwd`/../../../third_party/libc++/src/include "
                        "-isystem`pwd`/../../../third_party/libc++api/src/include "
                        "-D_LIBCPP_HARDENING_MODE=_LIBCPP_HARDING_MODE_EXTENSIVE "
                        "-nostdinc++ \" ")
        cmd = f"cd {obs_path} && {cmake_tools} && cd {tools_dir} && {make_tools} && cd {obs_path} && {cmake_targets} && cd {build_dir} && {make_targets}"
    return _call_with_output(cmd)

if __name__ == '__main__':
    return_code = main()
    sys.exit(return_code)  