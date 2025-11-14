/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#include "base/command_line.h"
#include "third_party/blink/renderer/core/editing/commands/clipboard_commands.h"
#include "third_party/blink/renderer/core/editing/commands/editing_command_type.h"
#include "third_party/blink/renderer/core/execution_context/execution_context.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/renderer/core/frame/settings.h"

#if !defined(COMPONENT_BUILD)
#include "third_party/blink/renderer/modules/permissions/permission_utils.h"
#else
#include "third_party/blink/public/mojom/permissions/permission.mojom-blink.h"
#endif  // !defined(COMPONENT_BUILD)
#endif  // BUILDFLAG(ARKWEB_NWEB_EX)

namespace blink {
#if defined(COMPONENT_BUILD)
namespace {
mojom::blink::PermissionDescriptorPtr CreateClipboardPermissionDescriptor(
    mojom::blink::PermissionName name,
    bool has_user_gesture,
    bool will_be_sanitized) {
  auto descriptor = mojom::blink::PermissionDescriptor::New();
  descriptor->name = name;
  auto clipboard_extension = mojom::blink::ClipboardPermissionDescriptor::New(
      has_user_gesture, will_be_sanitized);
  descriptor->extension =
      mojom::blink::PermissionDescriptorExtension::NewClipboard(
          std::move(clipboard_extension));
  return descriptor;
}
}
#endif  // defined(COMPONENT_BUILD)
bool Document::ValidateClipboardPreconditions(
    const EditorCommand& command,
    const String& checked_value) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  bool is_browser = base::CommandLine::ForCurrentProcess()->HasSwitch(::switches::kEnableNwebEx);
  if (!is_browser) {
    return command.Execute(checked_value);
  }
  auto command_type = command.GetCommandType();
  if (command_type != EditingCommandType::kCopy &&
      command_type != EditingCommandType::kCut &&
      command_type != EditingCommandType::kPaste) {
    return command.Execute(checked_value);
  }
  ExecutionContext* context = GetExecutionContext();
  if (!context) {
    return command.Execute(checked_value);
  }
  LocalDOMWindow& window = *To<LocalDOMWindow>(context);
  LocalFrame* local_frame = window.GetFrame();
  if (!local_frame || !local_frame->GetSettings() ||
      !local_frame->GetSettings()->GetClipboardSitePermissionEnabled()) {
    LOG(INFO) << "clipboard site permission not enabled";
    return command.Execute(checked_value);
  }

  bool can_execute = command.CanExecute();
  if (!can_execute) {
    return false;
  }

  if (!window.document()->hasFocus()) {
    LOG(WARNING) << "Document is not focused.";
    return false;
  }

  // Grant permission by-default if extension has read/write permissions.
  if (local_frame->GetContentSettingsClient() &&
      local_frame->GetContentSettingsClient()->AllowWriteToClipboard()) {
    return command.Execute(checked_value);
  }

  if (ClipboardCommands::IsExecutingCutOrCopy(*context)) {
    LOG(DEBUG) << "Is executing cut or copy.";
    return command.Execute(checked_value);
  }

  mojom::blink::PermissionService* premission_service = GetPermissionService(context);
  if (!premission_service) {
    LOG(ERROR) << "Permission Service could not connect.";
    return false;
  }
  auto permission = (command_type == EditingCommandType::kPaste ?
      mojom::blink::PermissionName::CLIPBOARD_READ : mojom::blink::PermissionName::CLIPBOARD_WRITE);
  bool has_transient_user_activation = LocalFrame::HasTransientUserActivation(local_frame);
  auto permission_descriptor = CreateClipboardPermissionDescriptor(permission,
      /*has_user_gesture=*/has_transient_user_activation, /*will_be_sanitized=*/false);

  mojom::blink::PermissionStatus out_status = mojom::blink::PermissionStatus::DENIED;
  bool success = premission_service->RequestPermissionSync(
      std::move(permission_descriptor),
      /*user_gesture=*/has_transient_user_activation, &out_status);
  LOG(DEBUG) << "RequestPermissionSync success: " << success
             << ", is granted: " << (out_status == mojom::blink::PermissionStatus::GRANTED);
  if (success && out_status == mojom::blink::PermissionStatus::GRANTED) {
    return command.Execute(checked_value);
  }
  return false;
#else
  return command.Execute(checked_value);
#endif  // ARKWEB_NWEB_EX
}
}  // namespace blink
