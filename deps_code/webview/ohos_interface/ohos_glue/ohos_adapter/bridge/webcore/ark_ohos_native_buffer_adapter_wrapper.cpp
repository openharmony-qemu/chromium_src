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

#include "base/include/ark_web_log_utils.h"
#include "ohos_adapter/bridge/ark_ohos_native_buffer_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_native_buffer_config_adapter_impl.h"
#include "base/bridge/ark_web_bridge_macros.h"
#define HILOG_TAG "NATIVE_BUFFER_ADAPTER"

namespace OHOS::ArkWeb {

ArkOhosNativeBufferAdapterWrapper::ArkOhosNativeBufferAdapterWrapper(ArkWebRefPtr<ArkOhosNativeBufferAdapter> ref)
    : ctocpp_(ref) {}


void ArkOhosNativeBufferAdapterWrapper::AcquireBuffer(void* buffer)
{
    ctocpp_->AcquireBuffer(buffer);
}

void ArkOhosNativeBufferAdapterWrapper::Release(void* buffer)
{
    ctocpp_->Release(buffer);
}

int ArkOhosNativeBufferAdapterWrapper::GetEGLBuffer(void* buffer, void** eglBuffer)
{
    return ctocpp_->GetEGLBuffer(buffer, eglBuffer);
}

int ArkOhosNativeBufferAdapterWrapper::FreeEGLBuffer(void* eglBuffer)
{
    return ctocpp_->FreeEGLBuffer(eglBuffer);
}

int ArkOhosNativeBufferAdapterWrapper::NativeBufferFromNativeWindowBuffer(void* nativeWindowBuffer, void** nativeBuffer)
{
    return ctocpp_->NativeBufferFromNativeWindowBuffer(nativeWindowBuffer, nativeBuffer);
}

uint32_t ArkOhosNativeBufferAdapterWrapper::GetSeqNum(void* nativeBuffer)
{
    return ctocpp_->GetSeqNum(nativeBuffer);
}

void ArkOhosNativeBufferAdapterWrapper::Allocate(const std::shared_ptr<NativeBufferConfigAdapter> bufferConfig,
    void** outBuffer)
{
    if (CHECK_SHARED_PTR_IS_NULL(bufferConfig)) {
        ctocpp_->Allocate(nullptr, outBuffer);
    } else {
        ctocpp_->Allocate(new ArkNativeBufferConfigAdapterImpl(bufferConfig), outBuffer);
    }
}

void ArkOhosNativeBufferAdapterWrapper::Describe(
    std::shared_ptr<OHOS::NWeb::NativeBufferConfigAdapter> bufferConfig,
    void* buffer)
{
    if (CHECK_SHARED_PTR_IS_NULL(bufferConfig)) {
        ctocpp_->Describe(nullptr, buffer);
    } else {
        ctocpp_->Describe(new ArkNativeBufferConfigAdapterImpl(bufferConfig), buffer);
    }
}

int ArkOhosNativeBufferAdapterWrapper::Lock(void* buffer,
    uint64_t usage, int32_t fence, void** out_virtual_address)
{
    return ctocpp_->Lock(buffer, usage, fence, out_virtual_address);
}

int ArkOhosNativeBufferAdapterWrapper::RecvHandleFromUnixSocket(int socketFd, void** outBuffer)
{
    return ctocpp_->RecvHandleFromUnixSocket(socketFd, outBuffer);
}

int ArkOhosNativeBufferAdapterWrapper::SendHandleToUnixSocket(const void* buffer, int socketFd)
{
    return ctocpp_->SendHandleToUnixSocket(buffer, socketFd);
}

int ArkOhosNativeBufferAdapterWrapper::Unlock(void* buffer, int32_t* fence)
{
    return ctocpp_->Unlock(buffer, fence);
}

int ArkOhosNativeBufferAdapterWrapper::FreeNativeBuffer(void* nativeBuffer)
{
    return ctocpp_->FreeNativeBuffer(nativeBuffer);
}
} // namespace OHOS::ArkWeb
