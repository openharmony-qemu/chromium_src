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

#include "media_codec_list_adapter_impl.h"

#include <unordered_map>

#include "nweb_log.h"

namespace OHOS::NWeb {

MediaCodecListAdapterImpl& MediaCodecListAdapterImpl::GetInstance()
{
    static MediaCodecListAdapterImpl instance;
    return instance;
}

void MediaCodecListAdapterImpl::TransToAdapterCapability(
    const CapabilityData* data, std::shared_ptr<CapabilityDataAdapterImpl>& adapterData)
{
    if (adapterData == nullptr) {
        WVLOG_E("MediaCodecEncoder adapterData is null");
        return;
    }

    if (data == nullptr) {
        WVLOG_E("MediaCodecEncoder data is null.");
        adapterData->SetMaxWidth(0);
        adapterData->SetMaxHeight(0);
        adapterData->SetMaxframeRate(0);
        return;
    }
    adapterData->SetMaxWidth(data->width.maxVal);
    adapterData->SetMaxHeight(data->height.maxVal);
    adapterData->SetMaxframeRate(data->frameRate.maxVal);
}

std::shared_ptr<CapabilityDataAdapter> MediaCodecListAdapterImpl::GetCodecCapability(
    const std::string& mime, const bool isEncoder)
{
    std::shared_ptr<CapabilityDataAdapterImpl> capabilityAdapter = std::make_shared<CapabilityDataAdapterImpl>();
    if (capabilityAdapter == nullptr) {
        WVLOG_E("new CapabilityDataAdapterImpl failed.");
        return nullptr;
    }

    if (avCodecList_ == nullptr) {
        avCodecList_ = AVCodecListFactory::CreateAVCodecList();
    }
    if (avCodecList_ == nullptr) {
        WVLOG_E("CreateAVCodecList failed.");
        return capabilityAdapter;
    }
    CapabilityData* capabilityData = avCodecList_->GetCapability(mime, true, AVCodecCategory::AVCODEC_NONE);
    TransToAdapterCapability(capabilityData, capabilityAdapter);
    return capabilityAdapter;
}
} // namespace OHOS::NWeb
