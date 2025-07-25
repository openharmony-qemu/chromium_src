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

#include "ohos_nweb/src/nweb_web_storage_impl.h"

#include <fuzzer/FuzzedDataProvider.h>
#include <iostream>
#include <string>

#include "ohos_nweb/include/nweb.h"
#include "ohos_nweb/src/nweb_impl.h"

using namespace OHOS::NWeb;

class NWebLongValueCallbackTest : public NWebLongValueCallback {
public:
    NWebLongValueCallbackTest() = default;
    ~NWebLongValueCallbackTest() = default;

    void OnReceiveValue(long value) override {}
};

class NWebWebStorageOriginVectorValueCallbackTest : public NWebWebStorageOriginVectorValueCallback {
public:
    NWebWebStorageOriginVectorValueCallbackTest() = default;
    ~NWebWebStorageOriginVectorValueCallbackTest() = default;

    void OnReceiveValue(const std::vector<std::shared_ptr<NWebWebStorageOrigin>>& value) override {}
};

void FuzzTest001(FuzzedDataProvider* fdp) {
    NWebWebStorageImpl impl;

    std::string url = fdp->ConsumeRandomLengthString(128);
    std::string oldName = fdp->ConsumeRandomLengthString(128);
    std::string newNname = fdp->ConsumeRandomLengthString(128);
    std::string pd = fdp->ConsumeRandomLengthString(128);
    impl.ModifyPassword(url, oldName, newNname, pd);
}

void FuzzTest002(FuzzedDataProvider* fdp) {
    NWebStorageExtensionCallback* cb = new NWebStorageExtensionCallback;
    cb->struct_size = sizeof(NWebStorageExtensionCallback);
    cb->OnGetPassword = nullptr;
    cb->OnGetSavedPasswords = nullptr;
    std::shared_ptr<NWebStorageExtensionCallback> cb_ptr(cb);
    NWebWebStorageImpl impl;
    impl.PutWebStorageCallback(cb_ptr);
}

void FuzzTest003(FuzzedDataProvider* fdp) {
    NWebWebStorageImpl impl;
    std::shared_ptr<NWebLongValueCallback> cb
        = std::make_shared<NWebLongValueCallbackTest>();
    std::string origin = fdp->ConsumeRandomLengthString(128);

    impl.GetOriginUsage(origin, cb);
}

void FuzzTest004(FuzzedDataProvider* fdp) {
    NWebWebStorageImpl impl;
    std::shared_ptr<NWebLongValueCallback> cb = std::make_shared<NWebLongValueCallbackTest>();
    std::string origin = fdp->ConsumeRandomLengthString(128);

    impl.GetOriginQuota(origin, cb);
}

void FuzzTest005(FuzzedDataProvider* fdp) {
    NWebWebStorageImpl impl;
    
    impl.DeleteAllData(false);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (data == nullptr || size < sizeof(uint32_t)) {
    return;
  }
  FuzzedDataProvider fdp(data, size);

  FuzzTest001(&fdp);
  FuzzTest002(&fdp);
  FuzzTest003(&fdp);
  FuzzTest004(&fdp);
  FuzzTest005(&fdp);
  return 0;
}