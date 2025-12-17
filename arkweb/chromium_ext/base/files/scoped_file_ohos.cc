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

#include "base/files/scoped_file.h"

#include <cstdint>

#include "base/logging.h"

extern "C" {
void fdsan_exchange_owner_tag(int fd,
                              uint64_t expected_tag,
                              uint64_t new_tag)
    __attribute__((__weak__));
}

namespace base {
namespace internal {

static uint64_t ScopedFDToTag(const ScopedFD& owner) {
  return reinterpret_cast<uint64_t>(&owner);
}

// static
void ScopedFDCloseTraits::Acquire(const ScopedFD& owner, int fd) {
  if (fdsan_exchange_owner_tag) {
    fdsan_exchange_owner_tag(fd, 0, ScopedFDToTag(owner));
  } else {
    LOG(ERROR) << "Acquire fdsan_exchange_owner_tag is unavailable";
  }
}

// static
void ScopedFDCloseTraits::Release(const ScopedFD& owner, int fd) {
  if (fdsan_exchange_owner_tag) {
    fdsan_exchange_owner_tag(fd, ScopedFDToTag(owner), 0);
  } else {
    LOG(ERROR) << "Release fdsan_exchange_owner_tag is unavailable";
  }
}
}  // namespace internal
}  // namespace base
