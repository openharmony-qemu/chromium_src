// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on ohos_password_autofill_data.h originally written by
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_AUTOFILL_CORE_COMMON_OHOS_PASSWORD_AUTOFILL_DATA_H_
#define COMPONENTS_AUTOFILL_CORE_COMMON_OHOS_PASSWORD_AUTOFILL_DATA_H_

#include <string>

#include "build/build_config.h"
#include "components/autofill/core/common/unique_ids.h"
#include "ui/gfx/geometry/rect_f.h"

namespace autofill {

namespace mojom {
enum class OhosInputElementType;
}

// Structure used for sending information from browser to renderer about on
// which fields password should be generated.
struct InputFillRequestData {
  FieldRendererId field_renderer_id;
  bool is_focused;
  mojom::OhosInputElementType type;
  gfx::RectF bounds;
  std::u16string value;
  std::u16string placeholder;
  std::string autocomplete_attr;
};

}  // namespace autofill

#endif  // COMPONENTS_AUTOFILL_CORE_COMMON_OHOS_PASSWORD_AUTOFILL_DATA_H_