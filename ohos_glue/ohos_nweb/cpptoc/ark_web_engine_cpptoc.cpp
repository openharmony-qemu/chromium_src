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

#include "ohos_nweb/cpptoc/ark_web_engine_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"
#include "ohos_nweb/cpptoc/ark_web_cookie_manager_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_data_base_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_download_manager_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_nweb_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_web_storage_cpptoc.h"
#include "ohos_nweb/ctocpp/ark_web_engine_init_args_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_engine_prefetch_args_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_nweb_create_info_ctocpp.h"

namespace OHOS::ArkWeb {

ark_web_engine_t *ark_web_engine_get_instance() {
  // Execute
  ArkWebRefPtr<ArkWebEngine> _retval = ArkWebEngine::GetInstance();

  // Return type: refptr_same
  return ArkWebEngineCppToC::Invert(_retval);
}

namespace {

ark_web_nweb_t *ARK_WEB_CALLBACK ark_web_engine_create_nweb(
    struct _ark_web_engine_t *self, ark_web_nweb_create_info_t *create_info) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, NULL);

  // Execute
  ArkWebRefPtr<ArkWebNWeb> _retval = ArkWebEngineCppToC::Get(self)->CreateNWeb(
      ArkWebNWebCreateInfoCToCpp::Invert(create_info));

  // Return type: refptr_same
  return ArkWebNWebCppToC::Invert(_retval);
}

ark_web_nweb_t *ARK_WEB_CALLBACK
ark_web_engine_get_nweb(struct _ark_web_engine_t *self, int32_t nweb_id) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, NULL);

  // Execute
  ArkWebRefPtr<ArkWebNWeb> _retval =
      ArkWebEngineCppToC::Get(self)->GetNWeb(nweb_id);

  // Return type: refptr_same
  return ArkWebNWebCppToC::Invert(_retval);
}

ark_web_data_base_t *ARK_WEB_CALLBACK
ark_web_engine_get_data_base(struct _ark_web_engine_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, NULL);

  // Execute
  ArkWebRefPtr<ArkWebDataBase> _retval =
      ArkWebEngineCppToC::Get(self)->GetDataBase();

  // Return type: refptr_same
  return ArkWebDataBaseCppToC::Invert(_retval);
}

ark_web_web_storage_t *ARK_WEB_CALLBACK
ark_web_engine_get_web_storage(struct _ark_web_engine_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, NULL);

  // Execute
  ArkWebRefPtr<ArkWebWebStorage> _retval =
      ArkWebEngineCppToC::Get(self)->GetWebStorage();

  // Return type: refptr_same
  return ArkWebWebStorageCppToC::Invert(_retval);
}

ark_web_cookie_manager_t *ARK_WEB_CALLBACK
ark_web_engine_get_cookie_manager(struct _ark_web_engine_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, NULL);

  // Execute
  ArkWebRefPtr<ArkWebCookieManager> _retval =
      ArkWebEngineCppToC::Get(self)->GetCookieManager();

  // Return type: refptr_same
  return ArkWebCookieManagerCppToC::Invert(_retval);
}

ark_web_download_manager_t *ARK_WEB_CALLBACK
ark_web_engine_get_download_manager(struct _ark_web_engine_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, NULL);

  // Execute
  ArkWebRefPtr<ArkWebDownloadManager> _retval =
      ArkWebEngineCppToC::Get(self)->GetDownloadManager();

  // Return type: refptr_same
  return ArkWebDownloadManagerCppToC::Invert(_retval);
}

void ARK_WEB_CALLBACK ark_web_engine_set_web_tag(struct _ark_web_engine_t *self,
                                                 int32_t nweb_id,
                                                 const char *web_tag) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(web_tag, );

  // Execute
  ArkWebEngineCppToC::Get(self)->SetWebTag(nweb_id, web_tag);
}

void ARK_WEB_CALLBACK ark_web_engine_initialize_web_engine(
    struct _ark_web_engine_t *self, ark_web_engine_init_args_t *init_args) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebEngineCppToC::Get(self)->InitializeWebEngine(
      ArkWebEngineInitArgsCToCpp::Invert(init_args));
}

void ARK_WEB_CALLBACK ark_web_engine_prepare_for_page_load(
    struct _ark_web_engine_t *self, const ArkWebString *url,
    bool preconnectable, int32_t num_sockets) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(url, );

  // Execute
  ArkWebEngineCppToC::Get(self)->PrepareForPageLoad(*url, preconnectable,
                                                    num_sockets);
}

void ARK_WEB_CALLBACK ark_web_engine_set_web_debugging_access(
    struct _ark_web_engine_t *self, bool isEnableDebug) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebEngineCppToC::Get(self)->SetWebDebuggingAccess(isEnableDebug);
}

void ARK_WEB_CALLBACK
ark_web_engine_add_intelligent_tracking_prevention_bypassing_list(
    struct _ark_web_engine_t *self, const ArkWebStringVector *hosts) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(hosts, );

  // Execute
  ArkWebEngineCppToC::Get(self)->AddIntelligentTrackingPreventionBypassingList(
      *hosts);
}

void ARK_WEB_CALLBACK
ark_web_engine_remove_intelligent_tracking_prevention_bypassing_list(
    struct _ark_web_engine_t *self, const ArkWebStringVector *hosts) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(hosts, );

  // Execute
  ArkWebEngineCppToC::Get(self)
      ->RemoveIntelligentTrackingPreventionBypassingList(*hosts);
}

void ARK_WEB_CALLBACK
ark_web_engine_clear_intelligent_tracking_prevention_bypassing_list(
    struct _ark_web_engine_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebEngineCppToC::Get(self)
      ->ClearIntelligentTrackingPreventionBypassingList();
}

void ARK_WEB_CALLBACK
ark_web_engine_pause_all_timers(struct _ark_web_engine_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebEngineCppToC::Get(self)->PauseAllTimers();
}

void ARK_WEB_CALLBACK
ark_web_engine_resume_all_timers(struct _ark_web_engine_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebEngineCppToC::Get(self)->ResumeAllTimers();
}

void ARK_WEB_CALLBACK ark_web_engine_prefetch_resource(
    struct _ark_web_engine_t *self, ark_web_engine_prefetch_args_t **pre_args,
    const ArkWebStringMap *additional_http_headers,
    const ArkWebString *cache_key, const uint32_t *cache_valid_time) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(pre_args, );

  ARK_WEB_CPPTOC_CHECK_PARAM(additional_http_headers, );

  ARK_WEB_CPPTOC_CHECK_PARAM(cache_key, );

  ARK_WEB_CPPTOC_CHECK_PARAM(cache_valid_time, );

  // Translate param: pre_args; type: refptr_diff_byref
  ArkWebRefPtr<ArkWebEnginePrefetchArgs> pre_argsPtr;
  if (pre_args && *pre_args) {
    pre_argsPtr = ArkWebEnginePrefetchArgsCToCpp::Invert(*pre_args);
  }
  ArkWebEnginePrefetchArgs *pre_argsOrig = pre_argsPtr.get();

  // Execute
  ArkWebEngineCppToC::Get(self)->PrefetchResource(
      pre_argsPtr, *additional_http_headers, *cache_key, *cache_valid_time);

  // Restore param: pre_args; type: refptr_diff_byref
  if (pre_args) {
    if (pre_argsPtr.get()) {
      if (pre_argsPtr.get() != pre_argsOrig) {
        *pre_args = ArkWebEnginePrefetchArgsCToCpp::Revert(pre_argsPtr);
      }
    } else {
      *pre_args = nullptr;
    }
  }
}

void ARK_WEB_CALLBACK ark_web_engine_set_render_process_mode(
    struct _ark_web_engine_t *self, int32_t mode) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebEngineCppToC::Get(self)->SetRenderProcessMode(mode);
}

int32_t ARK_WEB_CALLBACK
ark_web_engine_get_render_process_mode(struct _ark_web_engine_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebEngineCppToC::Get(self)->GetRenderProcessMode();
}

void ARK_WEB_CALLBACK ark_web_engine_clear_prefetched_resource(
    struct _ark_web_engine_t *self, const ArkWebStringVector *cache_key_list) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(cache_key_list, );

  // Execute
  ArkWebEngineCppToC::Get(self)->ClearPrefetchedResource(*cache_key_list);
}

void ARK_WEB_CALLBACK ark_web_engine_warmup_service_worker(
    struct _ark_web_engine_t *self, const ArkWebString *url) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(url, );

  // Execute
  ArkWebEngineCppToC::Get(self)->WarmupServiceWorker(*url);
}

void ARK_WEB_CALLBACK ark_web_engine_set_host_ip(struct _ark_web_engine_t *self,
                                                 const ArkWebString *hostName,
                                                 const ArkWebString *address,
                                                 int32_t aliveTime) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(hostName, );

  ARK_WEB_CPPTOC_CHECK_PARAM(address, );

  // Execute
  ArkWebEngineCppToC::Get(self)->SetHostIP(*hostName, *address, aliveTime);
}

void ARK_WEB_CALLBACK ark_web_engine_clear_host_ip(
    struct _ark_web_engine_t *self, const ArkWebString *hostName) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(hostName, );

  // Execute
  ArkWebEngineCppToC::Get(self)->ClearHostIP(*hostName);
}

} // namespace

ArkWebEngineCppToC::ArkWebEngineCppToC() {
  GetStruct()->create_nweb = ark_web_engine_create_nweb;
  GetStruct()->get_nweb = ark_web_engine_get_nweb;
  GetStruct()->get_data_base = ark_web_engine_get_data_base;
  GetStruct()->get_web_storage = ark_web_engine_get_web_storage;
  GetStruct()->get_cookie_manager = ark_web_engine_get_cookie_manager;
  GetStruct()->get_download_manager = ark_web_engine_get_download_manager;
  GetStruct()->set_web_tag = ark_web_engine_set_web_tag;
  GetStruct()->initialize_web_engine = ark_web_engine_initialize_web_engine;
  GetStruct()->prepare_for_page_load = ark_web_engine_prepare_for_page_load;
  GetStruct()->set_web_debugging_access =
      ark_web_engine_set_web_debugging_access;
  GetStruct()->add_intelligent_tracking_prevention_bypassing_list =
      ark_web_engine_add_intelligent_tracking_prevention_bypassing_list;
  GetStruct()->remove_intelligent_tracking_prevention_bypassing_list =
      ark_web_engine_remove_intelligent_tracking_prevention_bypassing_list;
  GetStruct()->clear_intelligent_tracking_prevention_bypassing_list =
      ark_web_engine_clear_intelligent_tracking_prevention_bypassing_list;
  GetStruct()->pause_all_timers = ark_web_engine_pause_all_timers;
  GetStruct()->resume_all_timers = ark_web_engine_resume_all_timers;
  GetStruct()->prefetch_resource = ark_web_engine_prefetch_resource;
  GetStruct()->set_render_process_mode = ark_web_engine_set_render_process_mode;
  GetStruct()->get_render_process_mode = ark_web_engine_get_render_process_mode;
  GetStruct()->clear_prefetched_resource =
      ark_web_engine_clear_prefetched_resource;
  GetStruct()->warmup_service_worker = ark_web_engine_warmup_service_worker;
  GetStruct()->set_host_ip = ark_web_engine_set_host_ip;
  GetStruct()->clear_host_ip = ark_web_engine_clear_host_ip;
}

ArkWebEngineCppToC::~ArkWebEngineCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<ArkWebEngineCppToC, ArkWebEngine,
                                        ark_web_engine_t>::kBridgeType =
    ARK_WEB_ENGINE;

} // namespace OHOS::ArkWeb

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

ARK_WEB_EXPORT ark_web_engine_t *ark_web_engine_get_instance_static() {
  ARK_WEB_CPPTOC_DV_LOG();

  return OHOS::ArkWeb::ark_web_engine_get_instance();
}

#ifdef __cplusplus
}
#endif // __cplusplus
