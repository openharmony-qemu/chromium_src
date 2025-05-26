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
 
#include "audio_buffer_adapter_impl.h"
 
namespace OHOS::NWeb {
 
uint8_t* AudioBufferAdapterImpl::GetBuffer()
{
    return buffer_;
}
 
int32_t AudioBufferAdapterImpl::GetLength()
{
    return length_;
}
 
int64_t AudioBufferAdapterImpl::GetTimestamp()
{
    return timestamp_;
}
 
AudioCaptureSourceTypeAdapter AudioBufferAdapterImpl::GetSourcetype()
{
    return sourcetype_;
}
 
void AudioBufferAdapterImpl::SetBuffer(uint8_t* buffer)
{
    buffer_ = buffer;
}
 
void AudioBufferAdapterImpl::SetLength(int32_t length)
{
    length_ = length;
}
 
void AudioBufferAdapterImpl::SetTimestamp(int64_t timestamp)
{
    timestamp_ = timestamp;
}
 
void AudioBufferAdapterImpl::SetSourcetype(AudioCaptureSourceTypeAdapter sourcetype)
{
    sourcetype_ = sourcetype;
}
 
}  // namespace OHOS::NWeb