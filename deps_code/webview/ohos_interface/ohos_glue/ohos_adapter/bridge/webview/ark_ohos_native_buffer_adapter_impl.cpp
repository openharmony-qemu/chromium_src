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

#include "ohos_adapter/bridge/ark_ohos_native_buffer_adapter_impl.h"
#include "ohos_adapter/bridge/ark_native_buffer_config_adapter_wrapper.h"
#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

using namespace OHOS::NWeb;

ArkOhosNativeBufferAdapterImpl::ArkOhosNativeBufferAdapterImpl(NWeb::OhosNativeBufferAdapter& ref)
    : real_(ref) {}

void ArkOhosNativeBufferAdapterImpl::AcquireBuffer(void* buffer)
{
    return real_.AcquireBuffer(buffer);
}

void ArkOhosNativeBufferAdapterImpl::Release(void* buffer)
{
    return real_.Release(buffer);
}

int ArkOhosNativeBufferAdapterImpl::GetEGLBuffer(void* buffer, void** eglBuffer)
{
    return real_.GetEGLBuffer(buffer, eglBuffer);
}

int ArkOhosNativeBufferAdapterImpl::FreeEGLBuffer(void* eglBuffer)
{
    return real_.FreeEGLBuffer(eglBuffer);
}

int ArkOhosNativeBufferAdapterImpl::NativeBufferFromNativeWindowBuffer(void* nativeWindowBuffer, void** nativeBuffer)
{
    return real_.NativeBufferFromNativeWindowBuffer(nativeWindowBuffer, nativeBuffer);
}

uint32_t ArkOhosNativeBufferAdapterImpl::GetSeqNum(void* nativeBuffer)
{
    return real_.GetSeqNum(nativeBuffer);
}

void ArkOhosNativeBufferAdapterImpl::Allocate(
    const ArkWebRefPtr<ArkNativeBufferConfigAdapter> bufferConfig, void** outBuffer)
{
    if (CHECK_REF_PTR_IS_NULL(bufferConfig)) {
        real_.Allocate(nullptr, outBuffer);
        return;
    }
    real_.Allocate(std::make_shared<ArkNativeBufferConfigAdapterWrapper>(bufferConfig), outBuffer);
}

void ArkOhosNativeBufferAdapterImpl::Describe(ArkWebRefPtr<ArkNativeBufferConfigAdapter> bufferConfig, void* buffer)
{
    if (CHECK_REF_PTR_IS_NULL(bufferConfig)) {
        real_.Describe(nullptr, buffer);
        return;
    }
    real_.Describe(std::make_shared<ArkNativeBufferConfigAdapterWrapper>(bufferConfig), buffer);
}

int ArkOhosNativeBufferAdapterImpl::Lock(void* buffer,
    uint64_t usage, int32_t fence, void** out_virtual_address)
{
    return real_.Lock(buffer, usage, fence, out_virtual_address);
}

int ArkOhosNativeBufferAdapterImpl::RecvHandleFromUnixSocket(int socketFd, void** outBuffer)
{
    return real_.RecvHandleFromUnixSocket(socketFd, outBuffer);
}

int32_t ArkOhosNativeBufferAdapterImpl::SendHandleToUnixSocket(const void* buffer, int socketFd)
{
    return real_.SendHandleToUnixSocket(buffer, socketFd);
}

int ArkOhosNativeBufferAdapterImpl::Unlock(void* buffer, int32_t* fence)
{
    return real_.Unlock(buffer, fence);
}

int ArkOhosNativeBufferAdapterImpl::FreeNativeBuffer(void* nativeBuffer)
{
    return real_.FreeNativeBuffer(nativeBuffer);
}
} // namespace OHOS::ArkWeb
