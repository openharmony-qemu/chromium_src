/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "ohos_adapter/bridge/ark_media_avsession_callback_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {
ArkMediaAVSessionCallbackAdapterWrapper::ArkMediaAVSessionCallbackAdapterWrapper(
    ArkWebRefPtr<ArkMediaAVSessionCallbackAdapter> ref)
    : ctocpp_(ref)
{}

void ArkMediaAVSessionCallbackAdapterWrapper::Play()
{
    ctocpp_->Play();
}

void ArkMediaAVSessionCallbackAdapterWrapper::Pause()
{
    ctocpp_->Pause();
}

void ArkMediaAVSessionCallbackAdapterWrapper::Stop()
{
    ctocpp_->Stop();
}

void ArkMediaAVSessionCallbackAdapterWrapper::SeekTo(int64_t millisTime)
{
    ctocpp_->SeekTo(millisTime);
}

} // namespace OHOS::ArkWeb
