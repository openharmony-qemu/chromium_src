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

#ifndef NWEB_DELEGATE_HELPER_H
#define NWEB_DELEGATE_HELPER_H

#include <memory>
#include "nweb_delegate_interface.h"
#include "nweb_export.h"

namespace OHOS::NWeb {
class OHOS_NWEB_EXPORT NWebDelegateAdapter {
 public:
  static std::shared_ptr<NWebDelegateInterface> CreateNWebDelegate(
      int argc,
      const char* argv[],
      bool is_enhance_surface,
      void* window,
      bool popup
#if defined(OHOS_EX_DOWNLOAD)
      ,
      int nweb_id
#endif
#if BUILDFLAG(IS_OHOS)
      ,
      bool incognito_mode
#endif
#if defined(OHOS_RENDER_PROCESS_SHARE)
      ,
      const std::string& shared_render_process_token
#endif
  );
};
}  // namespace OHOS::NWeb

#endif  // NWEB_DELEGATE_HELPER_H
