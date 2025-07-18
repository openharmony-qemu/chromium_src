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

#include "arkweb/ohos_nweb/src/cef_delegate/nweb_extension_content_settings_cef_delegate.h"

#include <fuzzer/FuzzedDataProvider.h>

#include <cstring>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <stdlib.h>

#include "base/logging.h"
#include "cef/include/cef_browser.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "ohos_nweb/src/cef_delegate/nweb_extension_bookmarks_cef_delegate.h"
#include "ohos_nweb/src/cef_delegate/nweb_extension_content_settings_cef_delegate.h"

using namespace OHOS::NWeb;
using namespace base;

namespace OHOS::NWeb {
void MockGetCallbackImpl(const NWebExtensionContentSettingsDetail* detailParam,
                         const char* error) {
  (void)detailParam;
  (void)error;
}

void MockSetOrClearCallbackImpl(const char* error) {
  (void)error;
}

void FuzzNWebExtensionContentSettings(FuzzedDataProvider* fdp) {
  auto& delegate = NWebExtensionContentSettingsCefDelegate::GetInstance();

  ContentSettingsGetCallback getCallback = BindRepeating(&MockGetCallbackImpl);
  ContentSettingsSetCallback setCallback =
      BindRepeating(&MockSetOrClearCallbackImpl);
  ContentSettingsClearCallback clearCallback =
      BindRepeating(&MockSetOrClearCallbackImpl);

  NWebExtensionContentSettingsGetParam getParam{};
  getParam.extensionId = strdup(fdp->ConsumeRandomLengthString(100).c_str());
  getParam.primaryUrl = strdup(fdp->ConsumeRandomLengthString(200).c_str());
  getParam.secondaryUrl = strdup(fdp->ConsumeRandomLengthString(200).c_str());
  getParam.type = strdup(fdp->ConsumeRandomLengthString(50).c_str());

  bool genIncognito = fdp->ConsumeBool();
  if (genIncognito) {
    getParam.incognito = new bool(fdp->ConsumeBool());
  } else {
    getParam.incognito = nullptr;
  }

  delegate.OnGet(&getParam, getCallback);

  free(getParam.extensionId);
  free(getParam.primaryUrl);
  free(getParam.secondaryUrl);
  free(getParam.type);
  delete getParam.incognito;

  NWebExtensionContentSettingsSetParam setParam{};
  setParam.extensionId = strdup(fdp->ConsumeRandomLengthString(100).c_str());
  setParam.primaryPattern = strdup(fdp->ConsumeRandomLengthString(200).c_str());
  setParam.scope = strdup(fdp->ConsumeRandomLengthString(100).c_str());
  setParam.secondaryPattern =
      strdup(fdp->ConsumeRandomLengthString(200).c_str());
  setParam.contentSetting = strdup(fdp->ConsumeRandomLengthString(50).c_str());
  setParam.type = strdup(fdp->ConsumeRandomLengthString(50).c_str());

  delegate.OnSet(&setParam, setCallback);

  free(setParam.extensionId);
  free(setParam.primaryPattern);
  free(setParam.scope);
  free(setParam.secondaryPattern);
  free(setParam.contentSetting);
  free(setParam.type);

  NWebExtensionContentSettingsClearParam clearParam{};
  clearParam.extensionId = strdup(fdp->ConsumeRandomLengthString(100).c_str());
  clearParam.scope = strdup(fdp->ConsumeRandomLengthString(100).c_str());
  clearParam.type = strdup(fdp->ConsumeRandomLengthString(50).c_str());

  delegate.OnClear(&clearParam, clearCallback);

  free(clearParam.extensionId);
  free(clearParam.scope);
  free(clearParam.type);

  int requestId = fdp->ConsumeIntegralInRange<int>(1, 1000);

  NWebExtensionContentSettingsCallbackResult getResult{};
  getResult.requestId = requestId;
  getResult.error = strdup(fdp->ConsumeRandomLengthString(50).c_str());

  bool hasDetail = fdp->ConsumeBool();
  if (hasDetail) {
    getResult.detailParam = new NWebExtensionContentSettingsDetail{};
    getResult.detailParam->contentSetting =
        strdup(fdp->ConsumeRandomLengthString(100).c_str());
  } else {
    getResult.detailParam = nullptr;
  }

  delegate.OnGetCallback(&getResult);

  free(getResult.error);
  if (getResult.detailParam != nullptr) {
    free(getResult.detailParam->contentSetting);
    delete getResult.detailParam;
  }

  NWebExtensionContentSettingsCallbackSetResult setResult{};
  setResult.requestId = requestId;
  setResult.error = strdup(fdp->ConsumeRandomLengthString(50).c_str());

  delegate.OnSetCallback(&setResult);
  delegate.OnClearCallback(&setResult);

  free(setResult.error);

  int createRequestId = fdp->ConsumeIntegralInRange<int>(1, 1000);
  NWebExtensionContentSettingsCallbackResult createResult{};
  createResult.requestId = createRequestId;
  createResult.error = strdup(fdp->ConsumeRandomLengthString(50).c_str());
  createResult.detailParam = new NWebExtensionContentSettingsDetail{};
  createResult.detailParam->contentSetting =
      strdup(fdp->ConsumeRandomLengthString(100).c_str());

  free(createResult.error);
  free(createResult.detailParam->contentSetting);
  delete createResult.detailParam;
}

}  // namespace OHOS::NWeb

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  FuzzedDataProvider fdp(data, size);
  FuzzNWebExtensionContentSettings(&fdp);
  return 0;
}
