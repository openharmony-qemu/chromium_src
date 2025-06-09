/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ohos/adapter/common/shared_library.h"

#include <dlfcn.h>
#include <hilog/log.h>

namespace ohos::adapter::common {

const unsigned int LOG_PRINT_DOMAIN = 0xFF00;

SharedLibrary::NativeHandleType SharedLibrary::open(const char* path) noexcept {
  return dlopen(path, RTLD_NOW | RTLD_GLOBAL);
}

SharedLibrary::NativeSymbolType SharedLibrary::Symbol(
    SharedLibrary::NativeHandleType lib,
    const char* name) noexcept {
  return dlsym(lib, name);
}

void SharedLibrary::Close(SharedLibrary::NativeHandleType lib) noexcept {
  dlclose(lib);
}

std::string SharedLibrary::GetErrorInfo() noexcept {
  auto error = dlerror();
  return (error == nullptr) ? "Unknown error (dlerror failed)" : error;
}

SharedLibrary::SharedLibrary(SharedLibrary&& other) noexcept
    : handle_(other.handle_) {
  other.handle_ = nullptr;
}

SharedLibrary& SharedLibrary::operator=(SharedLibrary&& other) noexcept {
  if (this != &other) {
    std::swap(handle_, other.handle_);
  }
  return *this;
}

SharedLibrary::SharedLibrary(const char* dir_path, const char* lib_name) {
  if (!dir_path || !lib_name) {
    return;
  }

  std::string final_name = lib_name;
  std::string final_path = dir_path;

  final_name = LibSuffix::prefix + final_name + LibSuffix::suffix;
  if (!final_path.empty() &&
      final_path.find_last_of('/') != final_path.size() - 1) {
    final_path += '/';
  }
  handle_ = open((final_path + final_name).c_str());
  if (handle_ == nullptr) {
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "SharedLibrary",
                 "error= %{public}s", GetErrorInfo().c_str());
  }
}

bool SharedLibrary::HasSymbol(const char* symbol_name) const noexcept {
  if (!handle_ || !symbol_name) {
    return false;
  }
  return Symbol(handle_, symbol_name) != nullptr;
}

bool SharedLibrary::HasSymbol(const std::string& symbol) const noexcept {
  return HasSymbol(symbol.c_str());
}

SharedLibrary::NativeSymbolType SharedLibrary::GetSymbol(
    const char* symbol_name) const {
  if (symbol_name == nullptr || handle_ == nullptr) {
    return nullptr;
  }
  auto symbol = Symbol(handle_, symbol_name);
  return symbol;
}

SharedLibrary::~SharedLibrary() {
  if (handle_ != nullptr) {
    Close(handle_);
  }
}

SharedLibrary::SharedLibrary(const std::string& lib_name)
    : SharedLibrary("", lib_name.c_str()) {}

SharedLibrary::SharedLibrary(const char* lib_name)
    : SharedLibrary("", lib_name) {}

bool SharedLibrary::IsLoaded() {
  return handle_ != nullptr;
}

SharedLibrary::NativeHandleType SharedLibrary::GetNativeHandle() noexcept {
  return handle_;
}

SharedLibrary::NativeSymbolType SharedLibrary::GetSymbol(
    const std::string& symbol_name) const {
  return GetSymbol(symbol_name.c_str());
}
}  // namespace ohos::adapter
