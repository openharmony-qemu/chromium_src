/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef SURFACE_ADAPTER_IMPL_H
#define SURFACE_ADAPTER_IMPL_H

#include "graphic_adapter.h"
#include "ibuffer_consumer_listener.h"
#include "iconsumer_surface.h"

namespace OHOS::NWeb {
class SurfaceBufferAdapterImpl : public SurfaceBufferAdapter {
public:
    SurfaceBufferAdapterImpl() = default;

    explicit SurfaceBufferAdapterImpl(sptr<SurfaceBuffer> buffer);

    ~SurfaceBufferAdapterImpl() override = default;

    int32_t GetFileDescriptor() override;

    int32_t GetWidth() override;

    int32_t GetHeight() override;

    int32_t GetStride() override;

    int32_t GetFormat() override;

    uint32_t GetSize() override;

    void* GetVirAddr() override;

    sptr<SurfaceBuffer>& GetBuffer();

private:
    sptr<SurfaceBuffer> buffer_ = nullptr;
};

class BufferConsumerListenerImpl : public IBufferConsumerListener {
public:
    BufferConsumerListenerImpl(
        wptr<IConsumerSurface> surface, std::shared_ptr<IBufferConsumerListenerAdapter> listener);

    ~BufferConsumerListenerImpl() override = default;

    void OnBufferAvailable() override;

private:
    wptr<IConsumerSurface> cSurface_ = nullptr;

    std::shared_ptr<IBufferConsumerListenerAdapter> listener_ = nullptr;
};

class ConsumerSurfaceAdapterImpl : public IConsumerSurfaceAdapter {
public:
    ConsumerSurfaceAdapterImpl();

    ~ConsumerSurfaceAdapterImpl() = default;

    int32_t RegisterConsumerListener(std::shared_ptr<IBufferConsumerListenerAdapter> listenerAdapter) override;

    int32_t ReleaseBuffer(std::shared_ptr<SurfaceBufferAdapter> bufferAdapter, int32_t fence) override;

    int32_t SetUserData(const std::string& key, const std::string& val) override;

    int32_t SetQueueSize(uint32_t queueSize) override;

    sptr<IConsumerSurface>& GetConsumerSurface();

private:
    sptr<IConsumerSurface> cSurface_ = nullptr;
};

class ProducerSurfaceAdapterImpl : public ProducerSurfaceAdapter {
public:
    explicit ProducerSurfaceAdapterImpl(sptr<Surface> surface);

    ~ProducerSurfaceAdapterImpl() = default;

    std::shared_ptr<SurfaceBufferAdapter> RequestBuffer(
        int32_t& fence, std::shared_ptr<BufferRequestConfigAdapter> config) override;

    int32_t FlushBuffer(std::shared_ptr<SurfaceBufferAdapter> bufferAdapter, int32_t fence,
        std::shared_ptr<BufferFlushConfigAdapter> flushConfigAdapter) override;

private:
    void TransToBufferConfig(
        const std::shared_ptr<BufferRequestConfigAdapter> configAdapter, BufferRequestConfig& config);
    GraphicColorGamut TransToGraphicColorGamut(const ColorGamutAdapter& colorGamut);
    GraphicTransformType TransToTransformType(const TransformTypeAdapter& type);
    sptr<Surface> surface_ = nullptr;
};
} // namespace OHOS::NWeb

#endif // SURFACE_ADAPTER_IMPL_H
