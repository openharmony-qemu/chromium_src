// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_OHOS_CEF_EXT_LIBCEF_BROWSER_LOAD_COMMITTED_DETAILS_IMPL_H_
#define CEF_OHOS_CEF_EXT_LIBCEF_BROWSER_LOAD_COMMITTED_DETAILS_IMPL_H_
#pragma once

#include "content/public/browser/navigation_details.h"
#include "include/cef_load_committed_details.h"
#include "libcef/common/value_base.h"
#include "ohos_cef_ext/libcef/browser/load_committed_details_impl.h"

// CefLoadCommittedDetails implementation
class CefLoadCommittedDetailsImpl : public CefLoadCommittedDetails {
 public:
  explicit CefLoadCommittedDetailsImpl(const std::string& current_commit_url,
                                       NavigationType type,
                                       bool is_main_frame,
                                       bool is_same_document,
                                       bool did_replace_entry);

  CefLoadCommittedDetailsImpl(const CefLoadCommittedDetailsImpl&) = delete;
  CefLoadCommittedDetailsImpl& operator=(const CefLoadCommittedDetailsImpl&) =
      delete;

  // CefLoadCommittedDetailsImpl methods.
  bool IsMainFrame() override;
  bool IsSameDocument() override;
  bool DidReplaceEntry() override;
  NavigationType GetNavigationType() override;
  CefString GetCurrentURL() override;

  static NavigationType ConvertToCefLoadCommittedDetailsType(
      const content::LoadCommittedDetails& details);

 private:
  CefString current_commit_url_;
  NavigationType type_;
  bool is_main_frame_;
  bool is_same_document_;
  bool did_replace_entry_;

  IMPLEMENT_REFCOUNTING(CefLoadCommittedDetailsImpl);
};

#endif  // CEF_OHOS_CEF_EXT_LIBCEF_BROWSER_LOAD_COMMITTED_DETAILS_IMPL_H_
