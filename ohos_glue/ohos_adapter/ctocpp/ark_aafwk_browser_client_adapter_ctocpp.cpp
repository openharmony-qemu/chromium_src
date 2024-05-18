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

#include "ark_aafwk_browser_client_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
void* ArkAafwkBrowserClientAdapterCToCpp::QueryRenderSurface(int32_t surface_id)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);
    ark_aafwk_browser_client_adapter_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);
    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, query_render_surface, nullptr);

    // Excute
    return _struct->query_render_surface(_struct, surface_id);
}

ARK_WEB_NO_SANITIZE
void ArkAafwkBrowserClientAdapterCToCpp::ReportThread(
    int32_t status, int32_t process_id,
    int32_t thread_id, int32_t role)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);
    ark_aafwk_browser_client_adapter_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );
    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, report_thread, );
    // Excute
    return _struct->report_thread(_struct, status, process_id, thread_id, role);
}

ARK_WEB_NO_SANITIZE
void ArkAafwkBrowserClientAdapterCToCpp::PassSurface(int64_t surface_id)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);
    ark_aafwk_browser_client_adapter_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );
    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, pass_surface, );

    // Excute
    return _struct->pass_surface(_struct, surface_id);
}

ARK_WEB_NO_SANITIZE
void ArkAafwkBrowserClientAdapterCToCpp::DestroyRenderSurface(int32_t surface_id)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);
    ark_aafwk_browser_client_adapter_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );
    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, destroy_render_surface, );

    // Excute
    return _struct->destroy_render_surface(_struct, surface_id);
}

ArkAafwkBrowserClientAdapterCToCpp::ArkAafwkBrowserClientAdapterCToCpp() {}

ArkAafwkBrowserClientAdapterCToCpp::~ArkAafwkBrowserClientAdapterCToCpp() {}

template <>
ArkWebBridgeType ArkWebCToCppRefCounted<
    ArkAafwkBrowserClientAdapterCToCpp, ArkAafwkBrowserClientAdapter,
    ark_aafwk_browser_client_adapter_t>::kBridgeType =
    ARK_AAFWK_BROWSER_CLIENT_ADAPTER;

} // namespace OHOS::ArkWeb