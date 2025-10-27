/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "nweb_delegate_adapter.h"

#include "arkweb/build/features/features.h"
#include "nweb_hilog.h"

#if defined(USE_CEF)
#include "cef_delegate/nweb_delegate.h"
#endif
namespace OHOS::NWeb {
// static
std::shared_ptr<NWebDelegateInterface> NWebDelegateAdapter::CreateNWebDelegate(
    int argc,
    const char* argv[],
    bool is_enhance_surface,
    void* window,
    bool popup
#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
    ,
    int nweb_id
#endif
#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
    ,
    bool incognito_mode
#endif
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
    ,
    const std::string& shared_render_process_token
#endif
) {
#if defined(USE_CEF)
  std::shared_ptr<NWebDelegate> delegate =
      std::make_shared<NWebDelegate>(argc, argv);

  if (delegate == nullptr || !delegate->Init(is_enhance_surface, window, popup
#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
                                             ,
                                             nweb_id
#endif
#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
                                             ,
                                             incognito_mode
#endif
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
                                             ,
                                             shared_render_process_token
#endif
                                             )) {
    WVLOG_I("FAIL to create nweb delegate instance");
    return nullptr;
  }

  return delegate;
#else
  WVLOG_E("NO IMPLEMENTATION");
  return nullptr;
#endif
}
}  // namespace OHOS::NWeb
