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

#ifndef ARK_AAFWK_BROWSER_HOST_ADAPTER_CAPI_H
#define ARK_AAFWK_BROWSER_HOST_ADAPTER_CAPI_H
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "base/include/ark_web_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_aafwk_browser_host_adapter_t {
    /**
     * @brief Base structure
    */
    ark_web_base_ref_counted_t base;

    void*(ARK_WEB_CALLBACK* get_surface_from_kernel)(
        struct _ark_aafwk_browser_host_adapter_t* self, int32_t surface_id);

    void(ARK_WEB_CALLBACK* destroy_surface_from_kernel)(
        struct _ark_aafwk_browser_host_adapter_t* self, int32_t surface_id);
} ark_aafwk_browser_host_adapter_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_AAFWK_BROWSER_HOST_ADAPTER_CAPI_H