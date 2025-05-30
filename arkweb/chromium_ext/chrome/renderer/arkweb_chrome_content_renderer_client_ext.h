// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CHROME_RENDERER_ARKWEB_CHROME_CONTENT_RENDERER_CLIENT_EXT_H_
#define CHROME_RENDERER_ARKWEB_CHROME_CONTENT_RENDERER_CLIENT_EXT_H_

#include <stddef.h>

#include <map>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <vector>

#include "arkweb/build/features/features.h"
#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif
#include "base/gtest_prod_util.h"
#include "base/memory/scoped_refptr.h"
#include "base/task/single_thread_task_runner.h"
#include "build/build_config.h"
#include "chrome/common/media/webrtc_logging.mojom.h"
#include "chrome/services/speech/buildflags/buildflags.h"
#include "components/nacl/common/buildflags.h"
#include "components/safe_browsing/content/renderer/phishing_classifier/phishing_model_setter_impl.h"
#include "components/spellcheck/spellcheck_buildflags.h"
#include "content/public/renderer/content_renderer_client.h"
#include "content/public/renderer/render_thread.h"
#include "extensions/buildflags/buildflags.h"
#include "ipc/ipc_channel_proxy.h"
#include "media/base/key_systems_support_registration.h"
#include "media/media_buildflags.h"
#include "mojo/public/cpp/bindings/associated_remote.h"
#include "mojo/public/cpp/bindings/generic_pending_receiver.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "ppapi/buildflags/buildflags.h"
#include "printing/buildflags/buildflags.h"
#include "services/service_manager/public/cpp/local_interface_provider.h"
#include "third_party/blink/public/common/thread_safe_browser_interface_broker_proxy.h"
#include "third_party/blink/public/common/tokens/tokens.h"
#include "v8/include/v8-forward.h"

#if BUILDFLAG(IS_WIN)
#include "chrome/common/conflicts/remote_module_watcher_win.h"
#endif

#if BUILDFLAG(ENABLE_PLUGINS)
#include "chrome/common/plugin.mojom.h"
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
#include "cef/libcef/renderer/browser_impl.h"
#include "cef/libcef/renderer/render_manager.h"
#endif
#include "components/content_settings/renderer/content_settings_agent_impl.h"

class ArkWebChromeContentRendererClientExt
    : public ChromeContentRendererClient {
 public:
  ArkWebChromeContentRendererClientExt();

  ArkWebChromeContentRendererClientExt(
      const ArkWebChromeContentRendererClientExt&) = delete;
  ArkWebChromeContentRendererClientExt& operator=(
      const ArkWebChromeContentRendererClientExt&) = delete;

  ~ArkWebChromeContentRendererClientExt() override;
  ArkWebChromeContentRendererClientExt* AsArkWebChromeContentRendererClientExt()
      override {
    return this;
  }

#if BUILDFLAG(ARKWEB_NETWORK_BASE) && BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)
  void RenderFrameCreatedContentSettings(
      content_settings::ContentSettingsAgentImpl* content_settings,
      ChromeContentRendererClient* chrome_content_renderer_client);
#endif

#if BUILDFLAG(ARKWEB_ADBLOCK)
  void RenderFrameCreateSubresourceFilterAgent(
      content::RenderFrame* render_frame,
      raw_ptr<ChromeContentRendererClient> obj);

  void RenderFrameCreateSubresourceFilterAgentTriggerHide(
      content::RenderFrame* render_frame);

  bool GetAdBlockEnabledByFrame(content::RenderFrame* render_frame);

  void TriggerElementHidingInFrame(int routing_id) override;

  void TriggerUserElementHidingInFrame(int routing_id) override;
#endif
};

#endif  // CHROME_RENDERER_ARKWEB_CHROME_CONTENT_RENDERER_CLIENT_EXT_H_
