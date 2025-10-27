/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef NWEB_WEB_STORAGE_ORIGIN_IMPL_H
#define NWEB_WEB_STORAGE_ORIGIN_IMPL_H

#include "nweb_web_storage.h"

namespace OHOS::NWeb {

class NWebWebStorageOriginImpl : public NWebWebStorageOrigin {
 public:
  NWebWebStorageOriginImpl() = default;
  ~NWebWebStorageOriginImpl() = default;

  void SetOrigin(const std::string& origin);
  void SetQuota(long quota);
  void SetUsage(long usage);
  std::string GetOrigin();
  long GetQuota();
  long GetUsage();

 private:
  std::string origin_;
  int64_t quota_ = 0;
  int64_t usage_ = 0;
};

}  // namespace OHOS::NWeb

#endif  // NWEB_WEB_STORAGE_ORIGIN_IMPL_H
