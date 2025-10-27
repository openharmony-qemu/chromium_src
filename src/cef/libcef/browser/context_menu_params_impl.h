// Copyright (c) 2012 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_CONTEXT_MENU_PARAMS_IMPL_H_
#define CEF_LIBCEF_BROWSER_CONTEXT_MENU_PARAMS_IMPL_H_
#pragma once

#include "cef/include/cef_context_menu_handler.h"
#include "cef/libcef/common/value_base.h"
#include "content/public/browser/context_menu_params.h"

#if BUILDFLAG(IS_ARKWEB)
class ArkWebCefContextMenuParamsImplExt;
#endif
// CefContextMenuParams implementation. This class is not thread safe.
class CefContextMenuParamsImpl
    : public CefValueBase<CefContextMenuParamsExt, content::ContextMenuParams> {
 public:
  explicit CefContextMenuParamsImpl(content::ContextMenuParams* value);

  CefContextMenuParamsImpl(const CefContextMenuParamsImpl&) = delete;
  CefContextMenuParamsImpl& operator=(const CefContextMenuParamsImpl&) = delete;
 #if BUILDFLAG(IS_ARKWEB)
  virtual ArkWebCefContextMenuParamsImplExt* AsArkWebCefContextMenuParamsImplExt() {
    return nullptr;
  }
  friend class ArkWebCefContextMenuParamsImplExt;
#endif  // BUILDFLAG(IS_ARKWEB)
  // CefContextMenuParams methods.
  int GetXCoord() override;
  int GetYCoord() override;
  TypeFlags GetTypeFlags() override;
  CefString GetLinkUrl() override;
  CefString GetUnfilteredLinkUrl() override;
  CefString GetSourceUrl() override;
  bool HasImageContents() override;
  CefString GetTitleText() override;
  CefString GetPageUrl() override;
  CefString GetFrameUrl() override;
  CefString GetFrameCharset() override;
  MediaType GetMediaType() override;
  MediaStateFlags GetMediaStateFlags() override;
  CefString GetSelectionText() override;
  CefString GetMisspelledWord() override;
  bool GetDictionarySuggestions(std::vector<CefString>& suggestions) override;
  bool IsEditable() override;
  bool IsSpellCheckEnabled() override;
  EditStateFlags GetEditStateFlags() override;
  bool IsCustomMenu() override;

#if BUILDFLAG(ARKWEB_CLIPBOARD)
  InputFieldType GetInputFieldType() override { return CM_INPUTFIELDTYPE_NONE; }
  SourceType GetSourceType() override { return CM_SOURCETYPE_NONE; }
  InputFieldType ConventInputField(blink::mojom::FormControlType formType) { return CM_INPUTFIELDTYPE_NONE; }
#endif  // #if BUILDFLAG(ARKWEB_CLIPBOARD)

#if BUILDFLAG(ARKWEB_DRAG_DROP)
  void GetImageRect(int& x, int& y, int& w, int& h) override {}
  bool IsAILink() override { return false; }
#endif
};

#if BUILDFLAG(IS_ARKWEB)
#include "cef/ohos_cef_ext/libcef/browser/arkweb_context_menu_params_impl_ext.h"
#endif
#endif  // CEF_LIBCEF_BROWSER_CONTEXT_MENU_PARAMS_IMPL_H_
