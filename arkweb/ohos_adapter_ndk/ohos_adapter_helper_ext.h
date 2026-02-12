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

#ifndef OHOS_ADAPTER_HELPER_EXT_H
#define OHOS_ADAPTER_HELPER_EXT_H

#include "arkweb/ohos_adapter_ndk/ability_runtime_adapter/native_ability_runtime_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/display_manager_adapter/native_display_manager_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/graphic_adapter/native_window_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/graphic_adapter/window_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/multimodalinputnew_adapter/mmi_new_adapter_impl.h"

namespace OHOS::NWeb {

class OhosAdapterHelperExt {
public:
    OhosAdapterHelperExt() = default;
    virtual ~OhosAdapterHelperExt() = default;

    static OhosAdapterHelperExt& GetInstance()
    {
        static OhosAdapterHelperExt instance;
        return instance;
    }

    static std::unique_ptr<IConsumerSurfaceAdapter> CreateConsumerNativeAdapter()
    {
      return std::make_unique<ConsumerNativeAdapterImpl>();
    }

    static WindowAdapter& GetWindowAdapterNdkInstance()
    {
      return WindowAdapterNdkImpl::GetInstance();
    }

    static std::unique_ptr<DisplayManagerAdapter> CreateDisplayMgrAdapter()
    {
      return std::make_unique<NativeDisplayManagerAdapterImpl>();
    }

    static std::unique_ptr<MMIAdapter> CreateMMIAdapter()
    {
      return std::make_unique<MMINewAdapterImpl>();
    }

    static std::unique_ptr<NativeAbilityRuntimeAdapterImpl> CreateAbilityRuntimeAdapter()
    {
      return std::make_unique<NativeAbilityRuntimeAdapterImpl>();
    }
};

} // namespace OHOS::NWeb

#endif // OHOS_ADAPTER_HELPER_EXT_H
