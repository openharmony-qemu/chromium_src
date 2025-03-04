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

#ifndef NWEB_DATA_BASE_IMPL_H
#define NWEB_DATA_BASE_IMPL_H

#include "nweb_data_base.h"
#include "nweb_data_base_delegate.h"
#include "nweb_errors.h"

namespace OHOS::NWeb {

class NWebDataBaseImpl : public NWebDataBase {
 public:
  NWebDataBaseImpl();

  ~NWebDataBaseImpl() = default;

  bool ExistHttpAuthCredentials() override;

  void DeleteHttpAuthCredentials() override;

  void SaveHttpAuthCredentials(const std::string& host,
                               const std::string& realm,
                               const std::string& username,
                               const char* password) override;

  void GetHttpAuthCredentials(const std::string& host,
                              const std::string& realm,
                              std::string& username,
                              char* password,
                              uint32_t passwordSize) override;

  bool ExistPermissionByOrigin(const std::string& origin,
                               int type,
                               bool incognito) override;

  bool GetPermissionResultByOrigin(const std::string& origin,
                                   int type,
                                   bool& result,
                                   bool incognito) override;

  int SetPermissionByOrigin(const std::string& origin,
                            int type,
                            bool result,
                            bool incognito) override;

  int ClearPermissionByOrigin(const std::string& origin,
                              int type,
                              bool incognito) override;

  void ClearAllPermission(int type, bool incognito) override;

  std::vector<std::string> GetOriginsByPermission(int type,
                                                  bool incognito) override;

  bool ExistFormData() const;

  void ClearFormData();

 private:
  std::shared_ptr<NWebDataBaseDelegate> delegate_;
};

}  // namespace OHOS::NWeb
#endif
