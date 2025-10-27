// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
 
#ifndef CEF_LIBCEF_BROWSER_PERMISSION_ALLOY_ACCESS_QUERY_H_
#define CEF_LIBCEF_BROWSER_PERMISSION_ALLOY_ACCESS_QUERY_H_
 
#include <memory>
 
#include "include/cef_permission_status_query.h"
 
class AlloyAccessQuery : public CefAccessQuery {
 public:
  AlloyAccessQuery(const AlloyAccessQuery&) = delete;
  AlloyAccessQuery& operator=(const AlloyAccessQuery&) = delete;
 
  AlloyAccessQuery(const CefString& origin,
                   int resources,
                   cef_permission_status_query_callback_t callback);
  ~AlloyAccessQuery() override;
 
  CefString Origin() override;
 
  int ResourceAcessId() override;
 
  void ReportQueryResult(int32_t status) override;
 
private:
 CefString origin_;
 int resources_;
 cef_permission_status_query_callback_t callback_;
 
 IMPLEMENT_REFCOUNTING(AlloyAccessQuery);
};
 
#endif  // CEF_LIBCEF_BROWSER_PERMISSION_ALLOY_ACCESS_QUERY_H_