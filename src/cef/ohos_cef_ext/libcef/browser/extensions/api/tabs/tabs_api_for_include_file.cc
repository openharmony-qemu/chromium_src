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

class WindowsCreateFunction : public ExtensionFunction {
  ~WindowsCreateFunction() override {}
  ResponseAction Run() override;
  DECLARE_EXTENSION_FUNCTION("windows.create", WINDOWS_CREATE)
 private:
  static void OnCreateWindow(const base::WeakPtr<WindowsCreateFunction>& function,
                             const std::optional<WebExtensionWindow>& window,
                             const std::optional<std::string>& error);

  bool call_create_window_ = false;
  base::WeakPtrFactory<WindowsCreateFunction> weak_ptr_factory_{this};
};

class WindowsUpdateFunction : public ExtensionFunction {
  ~WindowsUpdateFunction() override {}
  ResponseAction Run() override;
  DECLARE_EXTENSION_FUNCTION("windows.update", WINDOWS_UPDATE)
 private:
  static void OnUpdateWindow(const base::WeakPtr<WindowsUpdateFunction>& function,
                             const std::optional<WebExtensionWindow>& window,
                             const std::optional<std::string>& error);

  bool call_update_window_ = false;
  base::WeakPtrFactory<WindowsUpdateFunction> weak_ptr_factory_{this};
};

class WindowsRemoveFunction : public ExtensionFunction {
  ~WindowsRemoveFunction() override {}
  ResponseAction Run() override;
  DECLARE_EXTENSION_FUNCTION("windows.remove", WINDOWS_REMOVE)
 private:
  static void OnRemoveWindow(const base::WeakPtr<WindowsRemoveFunction>& function,
                             const std::optional<std::string>& error);

  bool call_remove_window_ = false;
  base::WeakPtrFactory<WindowsRemoveFunction> weak_ptr_factory_{this};
};

class TabsCreateFunction : public ExtensionFunction {
  ~TabsCreateFunction() override {}
  ResponseAction Run() override;
  DECLARE_EXTENSION_FUNCTION("tabs.create", TABS_CREATE)
  static void OnTabCreated(const base::WeakPtr<TabsCreateFunction>& function,
                           const NWebExtensionTab& tab,
                           std::optional<std::string>& error);
  static void CreateTabForExtension(std::string& url, content::BrowserContext* context);
  bool call_create_tab_ = false;
  base::WeakPtrFactory<TabsCreateFunction> weak_ptr_factory_{this};
};

class TabsDuplicateFunction : public ExtensionFunction {
  ~TabsDuplicateFunction() override {}
  ResponseAction Run() override;
  DECLARE_EXTENSION_FUNCTION("tabs.duplicate", TABS_DUPLICATE)
  static void OnTabDuplicated(const base::WeakPtr<TabsDuplicateFunction>& function,
                             NWebExtensionTab& tab,
                             std::optional<std::string>& error);
  bool call_duplicate_tab_ = false;
  base::WeakPtrFactory<TabsDuplicateFunction> weak_ptr_factory_{this};
};

class TabsHighlightFunction : public ExtensionFunction {
  ~TabsHighlightFunction() override {}
  ResponseAction Run() override;
  bool HighlightTab(TabStripModel* tabstrip,
                    ui::ListSelectionModel* selection,
                    std::optional<size_t>* active_index,
                    int index,
                    std::string* error);
  DECLARE_EXTENSION_FUNCTION("tabs.highlight", TABS_HIGHLIGHT)
  static void OnTabHighlighted(const base::WeakPtr<TabsHighlightFunction>& function,
                               WebExtensionWindow& window,
                               std::optional<std::string>& error);
  bool call_highlight_tab_ = false;
  base::WeakPtrFactory<TabsHighlightFunction> weak_ptr_factory_{this};
};

class TabsUpdateFunction : public ExtensionFunction {
 public:
  TabsUpdateFunction();

 protected:
  ~TabsUpdateFunction() override {}
  bool UpdateURL(const std::string& url,
                 int tab_id,
                 std::string* error);
  ResponseValue GetResult();

  raw_ptr<content::WebContents, DanglingUntriaged> web_contents_;

 private:
  ResponseAction Run() override;

  DECLARE_EXTENSION_FUNCTION("tabs.update", TABS_UPDATE)
  static void OnTabUpdated(const base::WeakPtr<TabsUpdateFunction>& function,
                           NWebExtensionTab& tab,
                           std::optional<std::string>& error);
  bool GetUpdateParams(int tab_id,
                       std::optional<api::tabs::Update::Params>&,
                       NWebExtensionTabUpdateProperties& update_properties);
  bool call_update_tab_ = false;
  std::string error_;
  base::WeakPtrFactory<TabsUpdateFunction> weak_ptr_factory_{this};
};

class TabsMoveFunction : public ExtensionFunction {
  ~TabsMoveFunction() override {}
  ResponseAction Run() override;
  bool MoveTab(int tab_id,
               int* new_index,
               base::Value::List& tab_values,
               const std::optional<int>& window_id,
               std::string* error);
  DECLARE_EXTENSION_FUNCTION("tabs.move", TABS_MOVE)
  static void OnTabMoved(const base::WeakPtr<TabsMoveFunction>& function,
                      const std::vector<NWebExtensionTab>& tab,
                      std::optional<std::string>& error);
  bool call_move_tab_ = false;
  base::WeakPtrFactory<TabsMoveFunction> weak_ptr_factory_{this};
};

class TabsRemoveFunction : public ExtensionFunction {
 public:
  TabsRemoveFunction();
  void TabDestroyed();

 private:
  class WebContentsDestroyedObserver;
  ~TabsRemoveFunction() override;
  ResponseAction Run() override;
  bool RemoveTab(int tab_id, std::string* error);

  int remaining_tabs_count_ = 0;
  bool triggered_all_tab_removals_ = false;
  std::vector<std::unique_ptr<WebContentsDestroyedObserver>>
      web_contents_destroyed_observers_;
  DECLARE_EXTENSION_FUNCTION("tabs.remove", TABS_REMOVE)
  static void OnTabRemoved(const base::WeakPtr<TabsRemoveFunction>& function,
                        std::optional<std::string>& error);
  bool call_remove_tab_ = false;
  base::WeakPtrFactory<TabsRemoveFunction> weak_ptr_factory_{this};
};

class TabsGroupFunction : public ExtensionFunction {
  ~TabsGroupFunction() override = default;
  ResponseAction Run() override;
  DECLARE_EXTENSION_FUNCTION("tabs.group", TABS_GROUP)
  static void OnTabGrouped(const base::WeakPtr<TabsGroupFunction>& function,
                           int group_id,
                           std::optional<std::string>& error);
  bool call_group_tab_ = false;
  base::WeakPtrFactory<TabsGroupFunction> weak_ptr_factory_{this};
};

class TabsUngroupFunction : public ExtensionFunction {
  ~TabsUngroupFunction() override = default;
  ResponseAction Run() override;
  bool UngroupTab(int tab_id, std::string* error);
  DECLARE_EXTENSION_FUNCTION("tabs.ungroup", TABS_UNGROUP)
  static void OnTabUngrouped(const base::WeakPtr<TabsUngroupFunction>& function,
                             std::optional<std::string>& error);
  bool call_ungroup_tab_ = false;
  base::WeakPtrFactory<TabsUngroupFunction> weak_ptr_factory_{this};
};

class TabsDiscardFunction : public ExtensionFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("tabs.discard", TABS_DISCARD)

  TabsDiscardFunction();

  TabsDiscardFunction(const TabsDiscardFunction&) = delete;
  TabsDiscardFunction& operator=(const TabsDiscardFunction&) = delete;

 private:
  ~TabsDiscardFunction() override;

  ExtensionFunction::ResponseAction Run() override;
  static void OnTabDiscarded(const base::WeakPtr<TabsDiscardFunction>& function,
                             NWebExtensionTab& tab,
                             std::optional<std::string>& error);
  bool call_discard_tab_ = false;
  base::WeakPtrFactory<TabsDiscardFunction> weak_ptr_factory_{this};
};
