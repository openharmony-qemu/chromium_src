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

class PermissionsRequestFunction : public ExtensionFunction {
 public:
  // An action to take for a permissions prompt, if any. This allows tests to
  // override prompt behavior.
  enum class DialogAction {
    // The dialog will show normally.
    kDefault,
    // The dialog will not show and the grant will be auto-accepted.
    kAutoConfirm,
    // The dialog will not show and the grant will be auto-rejected.
    kAutoReject,
    // The dialog will not show and the grant can be resolved via
    // the `ResolvePendingDialogForTests()` method.
    kProgrammatic,
  };

  DECLARE_EXTENSION_FUNCTION("permissions.request", PERMISSIONS_REQUEST)

  PermissionsRequestFunction();

  PermissionsRequestFunction(const PermissionsRequestFunction&) = delete;
  PermissionsRequestFunction& operator=(const PermissionsRequestFunction&) =
      delete;

  // FOR TESTS ONLY to bypass the confirmation UI.
  [[nodiscard]] static base::AutoReset<DialogAction> SetDialogActionForTests(
      DialogAction dialog_action);

  // The callback fired when the `DialogAction` is `kProgrammatic`.
  using ShowDialogCallback = base::RepeatingCallback<void(gfx::NativeWindow)>;

  [[nodiscard]] static base::AutoReset<ShowDialogCallback*>
  SetShowDialogCallbackForTests(ShowDialogCallback* callback);

  static void ResolvePendingDialogForTests(bool accept_dialog);
  static void SetIgnoreUserGestureForTests(bool ignore);

  // Returns the set of permissions that the user was prompted for, if any.
  std::unique_ptr<const PermissionSet> TakePromptedPermissionsForTesting();

 protected:
  ~PermissionsRequestFunction() override;

  // ExtensionFunction:
  ResponseAction Run() override;
  bool ShouldKeepWorkerAliveIndefinitely() override;

 private:
  void OnInstallPromptDone(ExtensionInstallPrompt::DoneCallbackPayload payload);
  void OnRuntimePermissionsGranted();
  void OnOptionalPermissionsGranted();
  void RespondIfRequestsFinished();

  void ShowPrompt(std::unique_ptr<ExtensionInstallPromptShowParams> show_params,
                  ExtensionInstallPrompt::DoneCallback done_callback,
                  std::unique_ptr<ExtensionInstallPrompt::Prompt> prompt);

  void OnShowPrompt(int action, const std::string& error);

  void OnGetPromptData(NWebExtensionPromptData* data);

  std::unique_ptr<ExtensionInstallPrompt> install_ui_;

  std::unique_ptr<ExtensionInstallPrompt::Prompt> prompt_;

  // Requested permissions that are currently withheld.
  std::unique_ptr<const PermissionSet> requested_withheld_;
  // Requested permissions that are currently optional, and not granted.
  std::unique_ptr<const PermissionSet> requested_optional_;

  bool requesting_withheld_permissions_ = false;
  bool requesting_optional_permissions_ = false;

  // The permissions, if any, that Chrome would prompt the user for. This will
  // be recorded if and only if the prompt is being bypassed for a test (see
  // also SetAutoConfirmForTests()).
  std::unique_ptr<const PermissionSet> prompted_permissions_for_testing_;

  base::WeakPtrFactory<PermissionsRequestFunction> weak_ptr_factory_{this};
};
