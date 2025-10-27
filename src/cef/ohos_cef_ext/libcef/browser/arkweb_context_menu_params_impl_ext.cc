// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cef/ohos_cef_ext/libcef/browser/arkweb_context_menu_params_impl_ext.h"

#include "arkweb/build/features/features.h"
#include "base/logging.h"
#include "third_party/blink/public/mojom/context_menu/context_menu.mojom.h"

ArkWebCefContextMenuParamsImplExt::ArkWebCefContextMenuParamsImplExt(
    content::ContextMenuParams* value)
    : CefContextMenuParamsImpl(value) {}

#if BUILDFLAG(ARKWEB_CLIPBOARD)
ArkWebCefContextMenuParamsImplExt::InputFieldType
ArkWebCefContextMenuParamsImplExt::ConventInputField(
    blink::mojom::FormControlType formType) {
  switch (formType) {
    case blink::mojom::FormControlType::kInputText:
    case blink::mojom::FormControlType::kInputSearch:
    case blink::mojom::FormControlType::kInputEmail:
    case blink::mojom::FormControlType::kInputUrl:
      return CefContextMenuParamsImpl::InputFieldType::
          CM_INPUTFIELDTYPE_PLAINTEXT;
    case blink::mojom::FormControlType::kInputPassword:
      return CefContextMenuParamsImpl::InputFieldType::
          CM_INPUTFIELDTYPE_PASSWORD;
    case blink::mojom::FormControlType::kInputNumber:
      return CefContextMenuParamsImpl::InputFieldType::CM_INPUTFIELDTYPE_NUMBER;
    case blink::mojom::FormControlType::kInputTelephone:
      return CefContextMenuParamsImpl::InputFieldType::
          CM_INPUTFIELDTYPE_TELEPHONE;
    default:
      return CefContextMenuParamsImpl::InputFieldType::CM_INPUTFIELDTYPE_OTHER;
  }
}

ArkWebCefContextMenuParamsImplExt::InputFieldType
ArkWebCefContextMenuParamsImplExt::GetInputFieldType() {
  CEF_VALUE_VERIFY_RETURN(false, CM_INPUTFIELDTYPE_NONE);
  if (!const_value().form_control_type.has_value()) {
    return CM_INPUTFIELDTYPE_NONE;
  }
  return ConventInputField(const_value().form_control_type.value());
}

ArkWebCefContextMenuParamsImplExt::SourceType
ArkWebCefContextMenuParamsImplExt::GetSourceType() {
  CEF_VALUE_VERIFY_RETURN(false, CM_SOURCETYPE_NONE);
  return static_cast<SourceType>(const_value().source_type);
}
#endif  // #if BUILDFLAG(ARKWEB_CLIPBOARD)

#if BUILDFLAG(ARKWEB_DRAG_DROP)
void ArkWebCefContextMenuParamsImplExt::GetImageRect(int& x,
                                                     int& y,
                                                     int& w,
                                                     int& h) {
  x = static_cast<int32_t>(const_value().image_rect.x());
  y = static_cast<int32_t>(const_value().image_rect.y());
  w = static_cast<int32_t>(const_value().image_rect.width());
  h = static_cast<int32_t>(const_value().image_rect.height());
}

bool ArkWebCefContextMenuParamsImplExt::IsAILink() {
  return const_value().is_ai_link;
}
#endif
