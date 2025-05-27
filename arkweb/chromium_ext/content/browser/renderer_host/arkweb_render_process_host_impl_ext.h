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

#ifndef ARKWEB_RENDER_PROCESS_HOST_IMPL_EXT_H_
#define ARKWEB_RENDER_PROCESS_HOST_IMPL_EXT_H_

#include "content/browser/renderer_host/render_process_host_impl.h"
#include "content/common/child_process.mojom.h"
#include "content/public/browser/browser_context.h"

namespace content {

class RenderProcessHostImpl;

class ArkwebRenderProcessHostImplExt : public RenderProcessHostImpl {
 public:
  ArkwebRenderProcessHostImplExt(BrowserContext* browser_context,
                                 StoragePartitionImpl* storage_partition_impl,
                                 int flags);

  ArkwebRenderProcessHostImplExt* AsArkwebRenderProcessHostImplExt() override {
    return this;
  }

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_MODE)
  bool IsProcessBackgrounded() override;

  const base::TimeTicks& ProcessBackgroundTime() override;
#endif

#if BUILDFLAG(ARKWEB_THEME_FONT)
  void OnThemeFontChange() override;
#endif

#if BUILDFLAG(ARKWEB_RENDERER_ANR_DUMP)
  void dumpCurrentJavaScriptStackInMainThread(
      base::OnceCallback<void(const std::string&)> dump_callback) override;

  void InvokeRenderCrashDump() override;
#endif

 private:
};

}  // namespace content

#endif  // ARKWEB_RENDER_PROCESS_HOST_IMPL_EXT_H_
