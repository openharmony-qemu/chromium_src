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

#ifndef OHOS_NATIVE_BUFFER_ADAPTER_IMPL_H
#define OHOS_NATIVE_BUFFER_ADAPTER_IMPL_H

#include "ohos_native_buffer_adapter.h"
#include "external_window.h"
#include "native_buffer.h"
#include "native_buffer_inner.h"
#include "surface_buffer.h"
#include "surface/window.h"

namespace OHOS::NWeb {
class OhosNativeBufferAdapterImpl : public OhosNativeBufferAdapter {
public:
    static OhosNativeBufferAdapter& GetInstance();

    OhosNativeBufferAdapterImpl();

    ~OhosNativeBufferAdapterImpl() override;

    void AcquireBuffer(void* buffer) override;

    void Release(void* buffer) override;

    int GetEGLBuffer(void* buffer, void** eglBuffer) override;

    int FreeEGLBuffer(void* eglBuffer) override;

    int NativeBufferFromNativeWindowBuffer(void* nativeWindowBuffer, void** nativeBuffer) override;

    uint32_t GetSeqNum(void* nativeBuffer) override;

    void Allocate(const std::shared_ptr<NativeBufferConfigAdapter> bufferConfig, void** outBuffer) override;

    void Describe(std::shared_ptr<NativeBufferConfigAdapter> bufferConfig, void* buffer) override;

    int Lock(void* buffer, uint64_t usage, int32_t fence, void** out_virtual_address) override;

    int RecvHandleFromUnixSocket(int socketFd, void** outBuffer) override;

    int SendHandleToUnixSocket(const void* buffer, int socketFd) override;

    int Unlock(void* buffer, int32_t* fence) override;

    int FreeNativeBuffer(void* nativeBuffer) override;

private:
    bool IsBufferLocked(OH_NativeBuffer* buffer) const;

    std::shared_ptr<std::mutex> mutex_;
    std::unordered_map<OH_NativeBuffer*, bool> lockedBuffers_;
};

} // namespace OHOS::NWeb

#endif // OHOS_NATIVE_BUFFER_ADAPTER_IMPL_H
