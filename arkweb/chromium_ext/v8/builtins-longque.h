/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef V8_BUILTINS_BUILTINS_LONGQUE_H_
#define V8_BUILTINS_BUILTINS_LONGQUE_H_

#include "v8-internal.h"

namespace v8 {
namespace internal {
namespace longque {
extern const char* name;

// Each time the Longque API is changed, the kVersion needs to be increased.
constexpr int kVersion = 1;
static_assert(kVersion <= kSmiMaxValue);

enum PropertyNameFilterFlag : int {
  kSkipPrototypeChain = 1 << 0,    // Skip properties of prototype chain
  kSkipPrefixUnderscore = 1 << 1,  // Skip property name that starts with '_'
  kSkipPrefixDollar = 1 << 2,      // Skip property name that starts with '$'
  kSkipConstructor = 1 << 3,       // Skip 'constructor'
  // Update it when append new enum item
  kLastPropertyNameFilterFlag = kSkipConstructor,
};

static_assert(PropertyNameFilterFlag::kLastPropertyNameFilterFlag <=
              kSmiMaxValue);

extern const std::pair<const char*, int>* GetGlobalSmiConstants();
extern size_t GetGlobalSmiConstantsCount();
}  // namespace longque
}  // namespace internal
}  // namespace v8

#endif  //V8_BUILTINS_BUILTINS_LONGQUE_H_