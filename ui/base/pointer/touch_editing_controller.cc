// Copyright 2013 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/base/pointer/touch_editing_controller.h"

namespace ui {

namespace {
TouchEditingControllerFactory* g_shared_instance = NULL;
}  // namespace

TouchEditingControllerDeprecated* TouchEditingControllerDeprecated::Create(
    TouchEditable* client_view) {
  if (g_shared_instance)
    return g_shared_instance->Create(client_view);
  return NULL;
}

// static
void TouchEditingControllerFactory::SetInstance(
    TouchEditingControllerFactory* instance) {
  g_shared_instance = instance;
}

#ifdef OHOS_CLIPBOARD
//static
int TouchEditable::ConvertMenuCommands(int ohos_command_id) {
  switch(ohos_command_id) {
    case QM_EDITFLAG_CAN_CUT:
      return kCut;
    case QM_EDITFLAG_CAN_COPY:
      return kCopy;
    case QM_EDITFLAG_CAN_PASTE:
      return kPaste;
    case QM_EDITFLAG_CAN_SELECT_ALL:
      return kSelectAll;
  }
  return QM_EDITFLAG_NONE;
}
#endif
}  // namespace ui
