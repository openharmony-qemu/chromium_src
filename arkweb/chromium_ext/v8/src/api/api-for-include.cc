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

#ifdef V8_ENABLE_OHOS_PERF_JIT
int v8::V8::CreateJSVMExtractor(uintptr_t& ptr, uint32_t pid) {
  jitparse::JSVMSymbolExtractor* exactor_ =
      new jitparse::JSVMSymbolExtractor(pid);
  if (exactor_->GetParser()) {
    ptr = reinterpret_cast<uintptr_t>(exactor_);
    return 0;
  } else {
    delete exactor_;
    return -1;
  }
}

void v8::V8::DeleteJSVMExtractor(uintptr_t ptr) {
  delete reinterpret_cast<jitparse::JSVMSymbolExtractor*>(ptr);
}

int v8::V8::GetJSVMCodeName(uintptr_t ptr,
                            uintptr_t pc,
                            std::string& codeName) {
  jitparse::JSVMSymbolExtractor* exactor_ =
      reinterpret_cast<jitparse::JSVMSymbolExtractor*>(ptr);
  if (exactor_->GetInstruction(pc, codeName)) {
    return 0;
  }
  return -1;
}
#endif  // V8_ENABLE_OHOS_PERF_JIT