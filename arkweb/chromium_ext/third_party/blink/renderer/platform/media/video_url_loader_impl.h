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

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_MEDIA_VIDEO_URL_LOADER_IMPL_H
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_MEDIA_VIDEO_URL_LOADER_IMPL_H

#include "base/memory/scoped_refptr.h"
#include "base/memory/weak_ptr.h"
#include "base/task/single_thread_task_runner.h"
#include "third_party/blink/public/web/web_associated_url_loader.h"
#include "third_party/blink/public/web/web_associated_url_loader_options.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/heap/garbage_collected.h"
#include "third_party/blink/renderer/platform/heap/member.h"
#include "third_party/blink/renderer/platform/heap/persistent.h"

namespace blink {

class ThreadableLoader;
class WebAssociatedURLLoaderClient;
class ExecutionContext;
class LocalFrame;

// This class is used to implement WebFrame::createAssociatedURLLoader.
class VideoURLLoaderImpl final : public WebAssociatedURLLoader {
 public:
  VideoURLLoaderImpl(ExecutionContext*,
                  const WebAssociatedURLLoaderOptions&,
                  LocalFrame*);
  VideoURLLoaderImpl(const VideoURLLoaderImpl&) = delete;
  VideoURLLoaderImpl& operator=(const VideoURLLoaderImpl&) = delete;
  ~VideoURLLoaderImpl() override;

  void LoadAsynchronously(const WebURLRequest&,
                          WebAssociatedURLLoaderClient*) override;

  void Load(const WebURLRequest&,
            base::WeakPtr<WebAssociatedURLLoaderClient>,
            std::string id,
            int64_t start) override;

  void Cancel() override;
  void SetDefersLoading(bool) override;
  void SetLoadingTaskRunner(base::SingleThreadTaskRunner*) override;
  void NotifyFinished() override;

  // Called by ClientAdapter to handle completion of loading.
  void ClientAdapterDone();

 private:
  class ClientAdapter;
  class Observer;
  class FrameProxy;

  void ContextDestroyed();
  void CancelLoader();
  void DisposeObserver();

  WebAssociatedURLLoaderClient* ReleaseClient() {
    WebAssociatedURLLoaderClient* client = client_;
    client_ = nullptr;
    return client;
  }

  WebAssociatedURLLoaderClient* client_;
  WebAssociatedURLLoaderOptions options_;

  // Converts ThreadableLoaderClient method calls into WebURLLoaderClient method
  // calls.
  Persistent<ClientAdapter> client_adapter_;
  Persistent<ThreadableLoader> loader_;

  // A ExecutionContextLifecycleObserver for cancelling |loader_| when the
  // context is detached.
  Persistent<Observer> observer_;

  Persistent<FrameProxy> frame_;

  base::WeakPtrFactory<VideoURLLoaderImpl> weak_factory_{this};
};

}  // namespace blink

#endif // THIRD_PARTY_BLINK_RENDERER_PLATFORM_MEDIA_VIDEO_URL_LOADER_IMPL_H