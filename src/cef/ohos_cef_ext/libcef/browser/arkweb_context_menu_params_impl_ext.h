// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_CONTEXT_MENU_PARAMS_IMPL_EXT_H_
#define CEF_LIBCEF_BROWSER_CONTEXT_MENU_PARAMS_IMPL_EXT_H_
#pragma once

#include "cef/libcef/browser/context_menu_params_impl.h"

class CefContextMenuParamsImpl;
class ArkWebCefContextMenuParamsImplExt : public CefContextMenuParamsImpl {
 public:
  explicit ArkWebCefContextMenuParamsImplExt(content::ContextMenuParams* value);

  ArkWebCefContextMenuParamsImplExt(const ArkWebCefContextMenuParamsImplExt&) =
      delete;
  ArkWebCefContextMenuParamsImplExt& operator=(
      const ArkWebCefContextMenuParamsImplExt&) = delete;
  ArkWebCefContextMenuParamsImplExt* AsArkWebCefContextMenuParamsImplExt()
      override {
    return this;
  }

#if BUILDFLAG(ARKWEB_CLIPBOARD)
  InputFieldType GetInputFieldType() override;
  SourceType GetSourceType() override;
  InputFieldType ConventInputField(blink::mojom::FormControlType formType);
#endif  // #if BUILDFLAG(ARKWEB_CLIPBOARD)

#if BUILDFLAG(ARKWEB_DRAG_DROP)
  void GetImageRect(int& x, int& y, int& w, int& h) override;
  bool IsAILink() override;
#endif
};
#endif  // CEF_LIBCEF_BROWSER_CONTEXT_MENU_PARAMS_IMPL_EXT_H_
