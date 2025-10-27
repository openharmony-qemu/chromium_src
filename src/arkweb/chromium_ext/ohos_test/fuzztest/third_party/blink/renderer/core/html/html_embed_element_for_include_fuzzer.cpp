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
#include <fuzzer/FuzzedDataProvider.h>
#include <string.h>

#include "third_party/blink/renderer/core/dom/element.h"
#include "third_party/blink/renderer/core/html/html_embed_element.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/dom/document_init.h"
#include "third_party/blink/renderer/platform/wtf/text/atomic_string.h"

using namespace blink;

AtomicString CreateAlignedAtomicString(const char* str) {
    size_t len = std::strlen(str);
    const size_t alignment = 8;
    const size_t buffer_size = len + alignment;
    char* raw_mem = static_cast<char*>(std::malloc(buffer_size));
    if (!raw_mem) {
        return AtomicString();
    }
    uintptr_t raw_addr = reinterpret_cast<uintptr_t>(raw_mem);
    uintptr_t aligned_addr = (raw_addr + alignment - 1) & ~(alignment - 1);
    char* aligned_ptr = reinterpret_cast<char*>(aligned_addr);
    std::memcpy(aligned_ptr, str, len + 1);
    AtomicString result(aligned_ptr);
    std::free(raw_mem);
    return result;
}

Element::AttributeModificationParams CreateParams(const AtomicString& value) {
    return Element::AttributeModificationParams(
        html_names::kDirAttr,
        g_null_atom,
        value,
        Element::AttributeModificationReason::kDirectly
    );
}

void FuzzHtmlEmbedElementTest1(FuzzedDataProvider* fdp) {
    DocumentInit init = DocumentInit::Create();
    if (init.GetExecutionContext() == nullptr) {
        return;
    }
    Document document(init);
    HTMLEmbedElement embedElement(document);
    auto params = CreateParams(AtomicString(""));
    embedElement.NativeEmbedOverlay(params);
}

void FuzzHtmlEmbedElementTest2(FuzzedDataProvider* fdp) {
    DocumentInit init = DocumentInit::Create();
    if (init.GetExecutionContext() == nullptr) {
        return;
    }
    Document document(init);
    HTMLEmbedElement embedElement(document);
    AtomicString aligned_str = CreateAlignedAtomicString("display:overlay-infinity");
    auto params = CreateParams(aligned_str);
    embedElement.NativeEmbedOverlay(params);
}

void FuzzHtmlEmbedElementTest3(FuzzedDataProvider* fdp) {
    DocumentInit init = DocumentInit::Create();
    if (init.GetExecutionContext() == nullptr) {
        return;
    }
    Document document(init);
    HTMLEmbedElement embedElement(document);
    auto params = CreateParams(AtomicString("prefix display:overlay-infinity suffix"));
    embedElement.NativeEmbedOverlay(params);
}

void FuzzHtmlEmbedElementTest4(FuzzedDataProvider* fdp) {
    DocumentInit init = DocumentInit::Create();
    if (init.GetExecutionContext() == nullptr) {
        return;
    }
    Document document(init);
    HTMLEmbedElement embedElement(document);
    AtomicString aligned_str = CreateAlignedAtomicString("display:overlay");
    auto params = CreateParams(aligned_str);
    embedElement.NativeEmbedOverlay(params);
}

void FuzzHtmlEmbedElementTest5(FuzzedDataProvider* fdp) {
    DocumentInit init = DocumentInit::Create();
    if (init.GetExecutionContext() == nullptr) {
        return;
    }
    Document document(init);
    HTMLEmbedElement embedElement(document);
    AtomicString aligned_str = CreateAlignedAtomicString("prefix display:overlay suffix");
    auto params = CreateParams(aligned_str);
    embedElement.NativeEmbedOverlay(params);
}

void FuzzHtmlEmbedElementTest6(FuzzedDataProvider* fdp) {
    DocumentInit init = DocumentInit::Create();
    if (init.GetExecutionContext() == nullptr) {
        return;
    }
    Document document(init);
    HTMLEmbedElement embedElement(document);
    AtomicString aligned_str = CreateAlignedAtomicString((fdp->ConsumeRandomLengthString(256)).c_str());
    auto params = CreateParams(aligned_str);
    embedElement.NativeEmbedOverlay(params);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if((data == nullptr) || (size < sizeof(int32_t))) {
        return 0;
    }
    FuzzedDataProvider fdp(data, size);
    FuzzHtmlEmbedElementTest1(&fdp);
    FuzzHtmlEmbedElementTest2(&fdp);
    FuzzHtmlEmbedElementTest3(&fdp);
    FuzzHtmlEmbedElementTest4(&fdp);
    FuzzHtmlEmbedElementTest5(&fdp);
    FuzzHtmlEmbedElementTest6(&fdp);
    return 0;
}