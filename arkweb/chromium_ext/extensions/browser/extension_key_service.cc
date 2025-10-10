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

#include "extensions/browser/extension_key_service.h"

#include "base/base64.h"
#include "base/logging.h"
#include "base/no_destructor.h"
#include "base/strings/string_number_conversions.h"
#include "base/synchronization/lock.h"
#include "crypto/sha2.h"
#include "extensions/common/constants.h"

namespace extensions {

std::string BytesToHex(const std::vector<uint8_t>& bytes) {
  return base::HexEncode(bytes.data(), bytes.size());
}

ExtensionKeyService* ExtensionKeyService::GetInstance() {
  static base::NoDestructor<ExtensionKeyService> instance;
  return instance.get();
}

ExtensionKeyService::ExtensionKeyService() {
  LoadDefaultKeys();
}

ExtensionKeyService::~ExtensionKeyService() = default;

void ExtensionKeyService::LoadDefaultKeys() {
  crx2_public_key_.clear();
  crx3_public_key_hashes_.clear();

  crx2_public_key_ =
      std::vector<uint8_t>(extensions::kWebstoreSignaturesPublicKey,
                           extensions::kWebstoreSignaturesPublicKey +
                               extensions::kWebstoreSignaturesPublicKeySize);

  auto hash_array =
      crypto::SHA256Hash(base::as_bytes(base::make_span(crx2_public_key_)));
  std::vector<uint8_t> hash_vector(hash_array.begin(), hash_array.end());
  crx3_public_key_hashes_.push_back(hash_vector);
}

void ExtensionKeyService::SetPublisherKeys(
    const std::vector<std::vector<uint8_t>>& keys) {
  base::AutoLock lock(lock_);
  crx2_public_key_.clear();
  crx3_public_key_hashes_.clear();

  if (keys.empty()) {
    LoadDefaultKeys();
    return;
  }

  for (const auto& key : keys) {
    if (crx2_public_key_.empty()) {
      crx2_public_key_ = key;
    }
    auto hash_array = crypto::SHA256Hash(base::as_bytes(base::make_span(key)));
    std::vector<uint8_t> hash_vector(hash_array.begin(), hash_array.end());
    crx3_public_key_hashes_.push_back(hash_vector);
  }
}

std::vector<uint8_t> ExtensionKeyService::GetCrx2PublicKey() {
  base::AutoLock lock(lock_);
  return crx2_public_key_;
}

std::vector<std::vector<uint8_t>>
ExtensionKeyService::GetCrx3PublicKeyHashes() {
  base::AutoLock lock(lock_);
  return crx3_public_key_hashes_;
}

}  // namespace extensions