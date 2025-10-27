// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
 
#include "ohos_cef_ext/libcef/browser/permission/alloy_access_query.h"
#include "base/logging.h"
 
AlloyAccessQuery::AlloyAccessQuery(const CefString& origin,
                                   int resources,
                                   cef_permission_status_query_callback_t callback)
    : origin_(origin), resources_(resources), callback_(std::move(callback)) {}
 
AlloyAccessQuery::~AlloyAccessQuery() {
  if (!callback_.is_null()) {
    std::move(callback_).Run(0);
  }
}
 
CefString AlloyAccessQuery::Origin() {
  return origin_;
}
 
int AlloyAccessQuery::ResourceAcessId() {
  return resources_;
}
 
void AlloyAccessQuery::ReportQueryResult(int32_t status) {
  if (!callback_.is_null()) {
    std::move(callback_).Run(status);
  }
}