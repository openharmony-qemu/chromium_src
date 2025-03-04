// Copyright 2019 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_JS_INJECTION_RENDERER_JS_COMMUNICATION_H_
#define COMPONENTS_JS_INJECTION_RENDERER_JS_COMMUNICATION_H_

#include <string>
#include <vector>

#include "arkweb/build/features/features.h"
#include "base/memory/weak_ptr.h"
#include "components/js_injection/common/interfaces.mojom.h"
#include "content/public/renderer/render_frame_observer.h"
#include "content/public/renderer/render_frame_observer_tracker.h"
#include "mojo/public/cpp/bindings/associated_receiver.h"
#include "mojo/public/cpp/bindings/associated_remote.h"
#include "third_party/blink/public/platform/web_string.h"

namespace content {
class RenderFrame;
}

namespace js_injection {

class JsBinding;

class JsCommunication
    : public mojom::JsCommunication,
      public content::RenderFrameObserver,
      public content::RenderFrameObserverTracker<JsCommunication> {
 public:
  explicit JsCommunication(content::RenderFrame* render_frame);

  JsCommunication(const JsCommunication&) = delete;
  JsCommunication& operator=(const JsCommunication&) = delete;

  ~JsCommunication() override;

  // mojom::JsCommunication implementation
  void SetJsObjects(
      std::vector<mojom::JsObjectPtr> js_object_ptrs,
      mojo::PendingAssociatedRemote<mojom::JsObjectsClient> client) override;
  void AddDocumentStartScript(
      mojom::DocumentStartJavaScriptPtr script_ptr) override;
#if BUILDFLAG(ARKWEB_JS_ON_DOCUMENT_END)
  void AddDocumentEndScript(
      mojom::DocumentEndJavaScriptPtr script_ptr) override;
#endif
#if BUILDFLAG(ARKWEB_JSPROXY)
  void AddHeadReadyScript(
      mojom::DocumentStartJavaScriptPtr script_ptr) override;
  void RemoveHeadReadyScript(int32_t script_id) override;
#endif
  void RemoveDocumentStartScript(int32_t script_id) override;
#if BUILDFLAG(ARKWEB_JS_ON_DOCUMENT_END)
  void RemoveDocumentEndScript(int32_t script_id) override;
#endif
  // RenderFrameObserver implementation
  void DidClearWindowObject() override;
  void WillReleaseScriptContext(v8::Local<v8::Context> context,
                                int32_t world_id) override;
  void OnDestruct() override;

  void RunScriptsAtDocumentStart();
#if BUILDFLAG(ARKWEB_JS_ON_DOCUMENT_END)
  void RunScriptsAtDocumentEnd();
#endif
#if BUILDFLAG(ARKWEB_JSPROXY)
  void RunScriptsAtHeadReady();
#endif
  mojom::JsToBrowserMessaging* GetJsToJavaMessage(
      const std::u16string& js_object_name);

 private:
  class JsObjectInfo;
  struct DocumentStartJavaScript;
  struct DocumentEndJavaScript;

  void BindPendingReceiver(
      mojo::PendingAssociatedReceiver<mojom::JsCommunication> pending_receiver);

  using JsObjectMap = std::map<std::u16string, std::unique_ptr<JsObjectInfo>>;
  JsObjectMap js_objects_;

  // In some cases DidClearWindowObject will be called twice in a row, we need
  // to prevent doing multiple injection in that case.
  bool inside_did_clear_window_object_ = false;

  std::vector<std::unique_ptr<DocumentStartJavaScript>> scripts_;
#if BUILDFLAG(ARKWEB_JS_ON_DOCUMENT_END)
  std::vector<std::unique_ptr<DocumentEndJavaScript>> document_end_scripts_;
#endif
#if BUILDFLAG(ARKWEB_JSPROXY)
  std::vector<std::unique_ptr<DocumentStartJavaScript>> head_ready_scripts_;
#endif

  std::vector<base::WeakPtr<JsBinding>> js_bindings_;

  // Associated with legacy IPC channel.
  mojo::AssociatedReceiver<mojom::JsCommunication> receiver_{this};
  mojo::AssociatedRemote<mojom::JsObjectsClient> client_remote_;

  base::WeakPtrFactory<JsCommunication> weak_ptr_factory_for_bindings_{this};
};

}  // namespace js_injection

#endif  // COMPONENTS_JS_INJECTION_RENDERER_JS_COMMUNICATION_H_
