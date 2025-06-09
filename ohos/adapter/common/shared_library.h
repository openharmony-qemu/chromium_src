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

#ifndef OHOS_ADAPTER_COMMON_SHARED_LIBRARY_H_
#define OHOS_ADAPTER_COMMON_SHARED_LIBRARY_H_

#include <memory>
#include <string>

namespace ohos::adapter::common {

class SharedLibrary {
 public:
  struct LibSuffix {
    static constexpr const char* prefix = "lib";
    static constexpr const char* suffix = ".so";
  };
  using NativeHandleType = void*;
  using NativeSymbolType = void*;

  SharedLibrary(const SharedLibrary&) = delete;
  SharedLibrary& operator=(const SharedLibrary&) = delete;

  SharedLibrary(SharedLibrary&& other) noexcept;

  SharedLibrary& operator=(SharedLibrary&& other) noexcept;

  SharedLibrary(const char* dir_path, const char* lib_name);

  SharedLibrary(const std::string& dir_path, const std::string& lib_name)
      : SharedLibrary(dir_path.c_str(), lib_name.c_str()) {}

  SharedLibrary(const std::string& dir_path, const char* lib_name)
      : SharedLibrary(dir_path.c_str(), lib_name) {}

  SharedLibrary(const char* dir_path, const std::string& lib_name)
      : SharedLibrary(dir_path, lib_name.c_str()) {}

  explicit SharedLibrary(const std::string& lib_name);

  explicit SharedLibrary(const char* lib_name);

  ~SharedLibrary();

  NativeSymbolType GetSymbol(const char* symbol_name) const;

  NativeSymbolType GetSymbol(const std::string& symbol_name) const;

  bool IsLoaded();

  template <typename T>
  T* GetFunction(const char* symbol_name) const {
    return reinterpret_cast<T*>(GetSymbol(symbol_name));
  }

  template <typename T>
  T* GetFunction(const std::string& symbol_name) const {
    return GetFunction<T>(symbol_name.c_str());
  }

  template <typename T>
  T& GetVariable(const char* symbol_name) const {
    return *reinterpret_cast<T*>(GetSymbol(symbol_name));
  }

  template <typename T>
  T& GetVariable(const std::string& symbol_name) const {
    return GetVariable<T>(symbol_name.c_str());
  }

  bool HasSymbol(const char* symbol_name) const noexcept;

  bool HasSymbol(const std::string& symbol) const noexcept;

  NativeHandleType GetNativeHandle() noexcept;

 protected:
  NativeHandleType handle_{nullptr};

  static NativeHandleType open(const char* path) noexcept;

  static NativeSymbolType Symbol(NativeHandleType lib,
                                 const char* name) noexcept;

  static void Close(NativeHandleType lib) noexcept;

  static std::string GetErrorInfo() noexcept;
};
}  // namespace ohos::adapter::common

#endif  // OHOS_ADAPTER_COMMON_SHARED_LIBRARY_H_
