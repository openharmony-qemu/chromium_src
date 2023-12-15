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

#ifndef NWEB_DATA_BASE_DELEGATE_H
#define NWEB_DATA_BASE_DELEGATE_H

#include <string>

#include "cef/include/cef_base.h"
#include "cef_data_base.h"
#include "nweb_errors.h"

namespace OHOS::NWeb {
class NWebDataBaseDelegate {
 public:
  NWebDataBaseDelegate() = default;

  ~NWebDataBaseDelegate() = default;

  bool ExistHttpAuthCredentials();

  void DeleteHttpAuthCredentials();

  void SaveHttpAuthCredentials(const std::string& host,
                               const std::string& realm,
                               const std::string& username,
                               const char* password);

  void GetHttpAuthCredentials(const std::string& host,
                              const std::string& realm,
                              std::string& username,
                              char* password,
                              uint32_t passwordSize);

  bool ExistPermissionByOrigin(const std::string& origin,
                               int type,
                               bool incognito);

  bool GetPermissionResultByOrigin(const std::string& origin,
                                   int type,
                                   bool& result,
                                   bool incognito);

  int SetPermissionByOrigin(const std::string& origin,
                            int type,
                            bool result,
                            bool incognito);

  int ClearPermissionByOrigin(const std::string& origin,
                              int type,
                              bool incognito);

  void ClearAllPermission(int type, bool incognito);

  std::vector<std::string> GetOriginsByPermission(int type,
                                                  bool incognito);

  bool ExistFormData();

  void ClearFormData();

 private:
  CefRefPtr<CefDataBase> GetGlobalCefDataBase();
  CefRefPtr<CefDataBase> data_base_ = nullptr;

//#if BUILDFLAG(IS_OHOS)
  CefRefPtr<CefDataBase> GetGlobalIncognitoCefDataBase();
  CefRefPtr<CefDataBase> incognito_data_base_ = nullptr;
//#endif

};
}  // namespace OHOS::NWeb
#endif
