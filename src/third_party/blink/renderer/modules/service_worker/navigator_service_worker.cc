// Copyright 2014 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/blink/renderer/modules/service_worker/navigator_service_worker.h"

#include "arkweb/build/features/features.h"
#include "services/network/public/mojom/web_sandbox_flags.mojom-blink.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/execution_context/execution_context.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/navigator.h"
#include "third_party/blink/renderer/modules/service_worker/service_worker_container.h"
#include "third_party/blink/renderer/platform/bindings/script_state.h"
#include "third_party/blink/renderer/platform/instrumentation/use_counter.h"
#if BUILDFLAG(ARKWEB_ADVANCED_SECURITY_MODE)
#include "ohos_nweb/src/nweb_advanced_security.h"
#endif

namespace blink {

ServiceWorkerContainer* NavigatorServiceWorker::From(
    ExecutionContext& execution_context) {
  if (!execution_context.GetSecurityOrigin()->CanAccessServiceWorkers()) {
    return nullptr;
  }
  return ServiceWorkerContainer::From(execution_context);
}

// static
ServiceWorkerContainer* NavigatorServiceWorker::serviceWorker(
    ScriptState* script_state,
    Navigator&,
    ExceptionState& exception_state) {
#if BUILDFLAG(ARKWEB_ADVANCED_SECURITY_MODE)
  // block it on advanced security mode.
  bool isAdvancedSecurityMode = OHOS::NWeb::NWebAdvancedSecurityHelper::Inst().
        IsSecFeatureEnabled(OHOS::NWeb::NWebAdvancedSecurityHelper::Feature::ENABLE_SERVICEWORKER);
  if (isAdvancedSecurityMode) {
    return nullptr;
  }
#endif  // BUILDFLAG(ARKWEB_ADVANCED_SECURITY_MODE)
  ExecutionContext* execution_context = ExecutionContext::From(script_state);
  auto* container = From(*execution_context);
  if (!container) {
    if (execution_context->IsWindow()) {
      auto& window = To<LocalDOMWindow>(*execution_context);
      String error_message;
      if (window.IsSandboxed(network::mojom::blink::WebSandboxFlags::kOrigin)) {
        error_message =
            "Service worker is disabled because the context is sandboxed and "
            "lacks the 'allow-same-origin' flag.";
      } else {
        error_message =
            "Access to service workers is denied in this document origin.";
      }
      exception_state.ThrowSecurityError(error_message);
    }
    return nullptr;
  }

  if (execution_context->IsWindow() &&
      execution_context->GetSecurityOrigin()->IsLocal()) {
    UseCounter::Count(execution_context,
                      WebFeature::kFileAccessedServiceWorker);
  }

  return container;
}

}  // namespace blink
