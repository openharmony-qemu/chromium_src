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

#include <fuzzer/FuzzedDataProvider.h>


#include <cstdlib>
#include <ctime>

#include <fuzzer/FuzzedDataProvider.h>

#include "arkweb/ohos_nweb/src/nweb_hilog.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "ohos_adapter_helper.h"
#define private public
#include "arkweb/ohos_adapter_ndk/media_avsession_adapter/media_avsession_adapter_impl.h"

using namespace OHOS::NWeb;

namespace OHOS {
constexpr int MAX_SET_NUMBER = 1000;

class MediaAVSessionCallbackAdapterMock : public MediaAVSessionCallbackAdapter {
public:
    MediaAVSessionCallbackAdapterMock() = default;
    void Play() {};
    void Pause() {};
    void Stop() {};
    void SeekTo(int64_t millisTime) {};
};

bool MediaAVSessionAdapterImplFuzzTest(FuzzedDataProvider* fdp)
{
    int64_t time = fdp->ConsumeIntegralInRange<int64_t>(0, MAX_SET_NUMBER);

    std::shared_ptr<MediaAVSessionKey> key = std::make_shared<MediaAVSessionKey>();
    key->Init();
    key->GetPID();
    key->GetType();
    key->ToString();

    std::shared_ptr<MediaAVSessionAdapterImpl> avSessionAdapter = std::make_shared<MediaAVSessionAdapterImpl>();

    auto avSessionKey = std::make_shared<MediaAVSessionKey>();
    avSessionAdapter->avSessionKey_ = avSessionKey;
    avSessionAdapter->avSessionKey_->Init();
    avSessionAdapter->avSession_ = nullptr;

    int32_t rawValue = fdp->ConsumeIntegralInRange<int32_t>(-1, 1);
    MediaAVSessionType type = static_cast<MediaAVSessionType>(rawValue);
    avSessionAdapter->CreateAVSession(type);
    avSessionAdapter->DestroyAVSession();

    auto mediaAVSessionCallbackAdapterMock = std::make_shared<MediaAVSessionCallbackAdapterMock>();
    avSessionAdapter->RegistCallback(mediaAVSessionCallbackAdapterMock);
    avSessionAdapter->IsActivated();
    avSessionAdapter->Activate();
    avSessionAdapter->DeActivate();

    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return 0;
    }

    FuzzedDataProvider fdp(data, size);
    OHOS::MediaAVSessionAdapterImplFuzzTest(&fdp);
    return 0;
}