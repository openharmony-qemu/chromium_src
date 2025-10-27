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

#ifndef MEDIA_CODEC_LIST_ADAPTER_IMPL_H
#define MEDIA_CODEC_LIST_ADAPTER_IMPL_H

#if defined(NWEB_MEDIA_AVCODEC_ENABLE)
#include "avcodec_list.h"
#include "media_description.h"
#endif

#include "media_codec_adapter.h"
#include "capability_data_adapter_impl.h"

namespace OHOS::NWeb {

#if defined(NWEB_MEDIA_AVCODEC_ENABLE)
using namespace OHOS::MediaAVCodec;
#endif

class MediaCodecListAdapterImpl : public MediaCodecListAdapter {
public:
    static MediaCodecListAdapterImpl& GetInstance();

    MediaCodecListAdapterImpl() = default;

    ~MediaCodecListAdapterImpl() override = default;

    std::shared_ptr<CapabilityDataAdapter> GetCodecCapability(const std::string& mime, const bool isEncoder) override;

#if defined(NWEB_MEDIA_AVCODEC_ENABLE)
private:
    void TransToAdapterCapability(const CapabilityData* data, std::shared_ptr<CapabilityDataAdapterImpl>& adapterData);
    std::shared_ptr<AVCodecList> avCodecList_;
#endif
};

} // namespace OHOS::NWeb
#endif // MEDIA_CODEC_LIST_ADAPTER_IMPL_H
