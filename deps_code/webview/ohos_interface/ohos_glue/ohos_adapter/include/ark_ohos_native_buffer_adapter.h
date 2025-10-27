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

#ifndef ARK_OHOS_NATIVE_BUFFER_ADAPTER_H
#define ARK_OHOS_NATIVE_BUFFER_ADAPTER_H
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {
/*--ark web(source=webcore)--*/
class ArkNativeBufferConfigAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int GetBufferWidth() = 0;

    /*--ark web()--*/
    virtual int GetBufferHeight() = 0;

    /*--ark web()--*/
    virtual int GetBufferFormat() = 0;

    /*--ark web()--*/
    virtual int GetBufferUsage() = 0;

    /*--ark web()--*/
    virtual int GetBufferStride() = 0;

    /*--ark web()--*/
    virtual void SetBufferWidth(int width) = 0;

    /*--ark web()--*/
    virtual void SetBufferHeight(int height) = 0;

    /*--ark web()--*/
    virtual void SetBufferFormat(int format) = 0;

    /*--ark web()--*/
    virtual void SetBufferUsage(int usage) = 0;

    /*--ark web()--*/
    virtual void SetBufferStride(int stride) = 0;
};

/*--ark web(source=webview)--*/
class ArkOhosNativeBufferAdapter : public virtual ArkWebBaseRefCounted {
public:
    ArkOhosNativeBufferAdapter() = default;

    virtual ~ArkOhosNativeBufferAdapter() = default;

    /**
     * @Description: Adds the reference count of a OH_NativeBuffer.
     * @Input buffer: Indicates the pointer to a <b>OH_NativeBuffer</b> instance.
     * @Since 12005
     */
    /*--ark web()--*/
    virtual void AcquireBuffer(void* buffer) = 0;

    /**
     * @Description: Decreases the reference count of a OH_NativeBuffer and, when the reference count reaches 0, \n
     *               destroys this OH_NativeBuffer.
     * @Input buffer: Indicates the pointer to a <b>OH_NativeBuffer</b> instance.
     * @Since 12005
     */
    /*--ark web()--*/
    virtual void Release(void* buffer) = 0;

    /**
     * @Description: Get the egl buffer from the <b>OH_NativeBuffer</b> instance.
     * @Input buffer: Indicates the pointer to a <b>OH_NativeBuffer</b> instance.
     * @Output eglBuffer: Indicates the pointer to a egl buffer pointer.
     * @Return：Returns an error code, 0 is sucess, otherwise, failed.
     * @Since 12005
     */
    /*--ark web()--*/
    virtual int GetEGLBuffer(void* buffer, void** eglBuffer) = 0;

    /**
     * @Description: Free egl buffer.
     * @Input eglBuffer: Indicates the pointer to an egl buffer.
     * @Return：Returns an error code, 0 is sucess, otherwise, failed.
     * @Since 12005
     */
    /*--ark web()--*/
    virtual int FreeEGLBuffer(void* eglBuffer) = 0;

    /**
     * @Description: Converts an <b>OHNativeWindowBuffer</b> instance to an <b>OH_NativeBuffer</b>.
     * @Input nativeWindowBuffer: Indicates the pointer to a <b>OHNativeWindowBuffer</b> instance.
     * @Output nativeBuffer: Indicates the pointer to a <b>OH_NativeBuffer</b> pointer.
     * @Return：Returns an error code, 0 is sucess, otherwise, failed.
     * @Since 12005
     */
    /*--ark web()--*/
    virtual int NativeBufferFromNativeWindowBuffer(void* nativeWindowBuffer, void** nativeBuffer) = 0;

    /**
     * @Description: Get the sequence number of native buffer .
     * @Input nativeBuffer: Indicates the pointer to a <b>OH_NativeBuffer</b> instance.
     * @Return：Returns a sequence number.
     * @Since 12005
     */
    /*--ark web()--*/
    virtual uint32_t GetSeqNum(void* nativeBuffer) = 0;

    /**
     * @Description: allocate a native buffer and match with outbuffer.
     * @Input bufferConfig: Indicates the pointer to a ArkNativeBufferConfigAdapter instance.
     * @Output nativeBuffer: Indicates the pointer to a <b>OH_NativeBuffer</b> pointer.
     * @Since 14001
     */
    /*--ark web()--*/
    virtual void Allocate(const ArkWebRefPtr<ArkNativeBufferConfigAdapter> bufferConfig, void** outBuffer) = 0;

    /**
     * @Description: revise NativeBufferConfigAdapter describing the given buffer.
     * @Input nativeBuffer: Indicates the pointer to a <b>OH_NativeBuffer</b> instance.
     * @Output bufferConfig: Indicates the pointer to a ArkNativeBufferConfigAdapter instance.
     * @Since 14001
     */
    /*--ark web()--*/
    virtual void Describe(ArkWebRefPtr<ArkNativeBufferConfigAdapter> bufferConfig, void* buffer) = 0;

    /**
     * @Description: Locks the specified <b>OH_NativeBuffer</b> for access.
     * @Input nativeBuffer: Indicates the pointer to a <b>OH_NativeBuffer</b> instance.
     * @Input usage: Indicates usage of buffer.
     * @Input fence: Indicates gpu handle.
     * @Output out_virtual_address: Indicates the address of the <b>OH_NativeBuffer</b> in virtual memory;
     * @Return：Returns 0 on success, or -1 if the buffer is null or already locked.
     * @Since 14001
     */
    /*--ark web()--*/
    virtual int Lock(void* buffer,
        uint64_t usage, int32_t fence, void** out_virtual_address) = 0;
    
    /**
     * @Description: Receives a <b>OH_NativeBuffer</b> handle from a Unix socket.
     * @Input socketFd: The file descriptor of the Unix socket.
     * @Output nativeBuffer: Indicates the pointer to a <b>OH_NativeBuffer</b> pointer.
     * @Return：Always returns 0, indicating success or no operation performed.
     * @Since 14001
     */
    /*--ark web()--*/
    virtual int RecvHandleFromUnixSocket(int socketFd, void** outBuffer) = 0;

    /**
     * @Description: Sends a <b>OH_NativeBuffer</b> handle to a Unix socket.
     * @Input nativeBuffer: Indicates the pointer to a <b>OH_NativeBuffer</b> instance.
     * @Input socketFd: The file descriptor of the Unix socket.
     * @Return：Returns 0 if the buffer is valid, or -1 if the buffer is null.
     * @Since 14001
     */
    /*--ark web()--*/
    virtual int SendHandleToUnixSocket(const void* buffer, int socketFd) = 0;

    /**
     * @Description: Unlocks the specified <b>OH_NativeBuffer</b>.
     * @Input nativeBuffer: Indicates the pointer to a <b>OH_NativeBuffer</b> instance.
     * @Output fence: Indicates gpu handle.
     * @Return：Returns 0 on success, or -1 if the buffer is null or not locked.
     * @Since 14001
     */
    /*--ark web()--*/
    virtual int Unlock(void* buffer, int32_t* fence) = 0;

    /**
     * @Description: Frees the specified native buffer by unreferencing it.
     * @Input nativeBuffer: Indicates the pointer to a <b>OH_NativeBuffer</b> instance.
     * @Return：Returns 0 on success, or -1 if the native buffer is null.
     * @Since 14001
     */
    /*--ark web()--*/
    virtual int FreeNativeBuffer(void* nativeBuffer) = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_OHOS_NATIVE_BUFFER_ADAPTER_H
