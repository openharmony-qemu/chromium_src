// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/shell_dialogs/shell_dialog_ohos.h"

#include "base/environment.h"
#include "base/no_destructor.h"
#include "base/notreached.h"
#include "build/chromeos_buildflags.h"
#include "ui/shell_dialogs/select_file_dialog_ohos.h"
#include "ui/shell_dialogs/select_file_policy.h"

namespace ui {

//FIXME: Specific functions will be adapted later.
SelectFileDialog* CreateSelectFileDialog(
    SelectFileDialog::Listener* listener,
    std::unique_ptr<SelectFilePolicy> policy) {
  return new SelectFileDialogOHOS(listener, std::move(policy));
}

}  // namespace ui
