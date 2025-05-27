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

#include "third_party/blink/renderer/core/html/html_plugin_element.h"

#include "fuzzer/FuzzedDataProvider.h"
#include "third_party/blink/public/common/browser_interface_broker_proxy.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/dom/document_init.h"
#include "third_party/blink/renderer/core/execution_context/execution_context.h"
#include "third_party/blink/renderer/core/inspector/inspector_audits_issue.h"

using namespace blink;
namespace blink {
class CORE_EXPORT FuzzHTMLPluginElement final : public HTMLPluginElement {
 public:
  FuzzHTMLPluginElement(Document& document,
                        const CreateElementFlags flags = CreateElementFlags())
      : HTMLPluginElement(html_names::kEmbedTag, document, flags) {}

  FrameOwnerElementType OwnerType() const override {
    return FrameOwnerElementType::kNone;
  }

  LayoutEmbeddedContent* ExistingLayoutEmbeddedContent() const override {
    return nullptr;
  }

  void UpdatePluginInternal() override {}
};

class CORE_EXPORT FuzzExecutionContext final : public ExecutionContext {
 public:
  FuzzExecutionContext() : ExecutionContext(nullptr, nullptr, false) {}
  const BrowserInterfaceBrokerProxy& GetBrowserInterfaceBroker()
      const override {
    return proxy;
  }
  void AddConsoleMessageImpl(ConsoleMessage*,
                             bool discard_duplicates) override {}
  const KURL& Url() const override { return kurl; }
  const KURL& BaseURL() const override { return kurl; }
  scoped_refptr<base::SingleThreadTaskRunner> GetTaskRunner(
      TaskType) const override {
    return nullptr;
  }
  void CountUse(mojom::WebFeature feature) override {}
  KURL CompleteURL(const String& url) const override { return kurl; }
  void DisableEval(const String& error_message) override {}
  void SetWasmEvalErrorMessage(const String& error_message) override {}
  String UserAgent() const override { return "test"; }
  HttpsState GetHttpsState() const override { return HttpsState::kNone; }
  ResourceFetcher* Fetcher() override { return nullptr; }
  void ExceptionThrown(ErrorEvent*) override {}
  void AddInspectorIssue(mojom::blink::InspectorIssueInfoPtr) override {}
  void AddInspectorIssue(AudistsIssue issue) override {}
  EventTarget* ErrorEventTarget() override { return nullptr; }
  FrameOrWorkerScheduler* GetScheduler() override { return nullptr; }
  bool CrossOriginIsolatedCapability() const override { return false; }
  bool IsIsolatedContext() const override { return false; }
  ukm::UkmRecorder* UkmRecorder() override { return nullptr; }
  ukm::SourceId UkmSourceId() const override { return ukm::kInvalidSourceId; }
  ExecutionContextToken GetExecutionContextToken() const override {
    return ExecutionContextToken();
  }

 private:
  BrowserInterfaceBrokerProxy proxy = BrowserInterfaceBrokerProxy();
  KURL kurl = KURL();
};

void HtmlPluginElementFuzzTest(const uint8_t* data, size_t size) {
  DocumentInit init = DocumentInit::Create();
  if (init.GetExecutionContext() == nullptr) {
    return;
  }
  FuzzExecutionContext execution_context = FuzzExecutionContext();
  init.ForTest(execution_context);
  Document document(init);
  std::shared_ptr<HTMLPluginElement> element =
      std::make_shared<FuzzHTMLPluginElement>(document);
  const char* key = reinterpret_cast<const char*>(data);
  element->CheckNativeType(key);
}
}  // namespace blink

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  HtmlPluginElementFuzzTest(data, size);
  return 0;
}
 