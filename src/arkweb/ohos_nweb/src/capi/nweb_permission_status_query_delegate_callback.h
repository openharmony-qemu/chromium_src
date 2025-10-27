/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_NWEB_SRC_NWEB_PERMISSION_STATUS_QUERY_DELEGATE_CALLBACK_H
#define OHOS_NWEB_SRC_NWEB_PERMISSION_STATUS_QUERY_DELEGATE_CALLBACK_H

#include "arkweb/build/features/features.h"
#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "ohos_nweb_ex/public/capi/arkweb_permission_status_query.h"
#endif

struct NWebPermissionStatusQuery;

struct NWebPermissionStatusQueryDelegateCallback {
  void (*onPermissionStatusQuery)(struct NWebPermissionStatusQuery* query);
#if BUILDFLAG(ARKWEB_NWEB_EX)
  void (*onArkPermissionStatusQuery)(ArkWebPermissionStatusQuery* query);
#endif
};

#endif  // OHOS_NWEB_SRC_NWEB_PERMISSION_STATUS_QUERY_DELEGATE_CALLBACK_H
