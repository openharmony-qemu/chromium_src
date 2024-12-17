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

#include "third_party/blink/renderer/core/loader/native_loader.h"
#include "fuzzer/FuzzedDataProvider.h"
#include "third_party/blink/renderer/core/html/html_pluign_element.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/dom/document_init.h"
#include "third_party/blink/renderer/core/html/html_image_loader.h"
using namespace blink;

namespace OHOS {
class FuzzNativeLoader : public NativeLoader {
public:
    FuzzNativeLoader() : NativeLoader(nullptr) {}
    ~FuzzNativeLoader() override {}

    String DebugName() const override;
};

class CORE_EXPORT FuzzHTMLPluginElement final : public HTMLPlugInElement {
public:
    FuzzHTMLPluginElement(Document& document, const CreateElementFlags flags = CreateElementFlags())
        : HTMLPluginElement(html_names::kEmbedTag, document, flags) {}

    FrameOwnerElementType OwnerType() const override
    {
        return FrameOwnerElementType::kNone;
    }

    LayoutEmbeddedContent* ExistingLayoutEmbeddedContent() const override
    {
        return nullptr;
    }

    void UpdatePluginInternal() override {}
};

String FuzzNativeLoader::DebugName() const
{
    return "FuzzNativeLoader";
}

void NativeLoaderFuzzTest(const uint8_t* data, size_t size)
{
    DocumentInit init = DocumentInit::Create();
    if (init.GetExecutionContext() == nullptr) {
        return;
    }
    Document document(init);
    std::shared_ptr<HTMLPluginElement> element = std::make_shared<FuzzHTMLPluginElement>(document);
    NativeLoader* loader = element->NativeLoader();
    FuzzedDataProvider data_provider(data, size);
    int id = data_provider.ConsumeIntegralInRange<int>();
    int x = data_provider.ConsumeIntegralInRange<int>();
    int y = data_provider.ConsumeIntegralInRange<int>();
    int width = data_provider.ConsumeIntegralInRange<int>();
    int height = data_provider.ConsumeIntegralInRange<int>();
    blink::WebNativeClient::RectChangeCB rect_change_cb;
    loader->OnCreateNativeSurface(id, rect_change_cb);
    const gfx::Rect rect = gfx::Rect(x, y, width, height);
    loader->OnLayerRectChange(rect);
    bool visibility = data_provider.ConsumeBool();
    loader->OnLayerRectVisibilityChange(visibility);
}
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::NativeLoaderFuzzTest(data, size);
    return 0;
}