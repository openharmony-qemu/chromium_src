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

#ifndef CRX_FILE_CRX_KEY_SERVICE_H_
#define CRX_FILE_CRX_KEY_SERVICE_H_

#include <cstdint>
#include <vector>

#include "base/no_destructor.h"
#include "base/synchronization/lock.h"

namespace crx_file {

class CrxKeyService {
 public:
  static CrxKeyService* GetInstance();

  void SetPublisherKeys(const std::vector<std::vector<uint8_t>>& keys);
  std::vector<uint8_t> GetCrx2PublicKey();
  std::vector<std::vector<uint8_t>> GetCrx3PublicKeyHashes();

 private:
  friend class base::NoDestructor<CrxKeyService>;

  CrxKeyService();
  ~CrxKeyService();

  void LoadDefaultKeys();

  base::Lock lock_;
  std::vector<uint8_t> crx2_public_key_;
  std::vector<std::vector<uint8_t>> crx3_public_key_hashes_;
};

}  // namespace crx_file

#endif  // CRX_FILE_CRX_KEY_SERVICE_H_
