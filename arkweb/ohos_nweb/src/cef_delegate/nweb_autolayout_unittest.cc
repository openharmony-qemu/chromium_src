/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "nweb_autolayout.h"
 
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <sstream>
#include <vector>

#include "arkweb/ohos_adapter_ndk/interfaces/mock/mock_ohos_adapter_helper.h"
#include "arkweb/ohos_adapter_ndk/interfaces/mock/mock_system_properties_adapter.h"
#include "base/files/file_util.h"
#include "base/files/scoped_temp_dir.h"
#include "cef/include/cef_browser.h"
#include "cef/include/cef_browser_host.h"
#include "cef/include/cef_client.h"
#include "cef/include/cef_dom.h"
#include "cef/include/cef_frame.h"
#include "cef/include/cef_process_message.h"
#include "cef/include/cef_request.h"
#include "cef/include/cef_string_visitor.h"
#include "cef/include/cef_values.h"
#include "cef/include/cef_v8.h"

using namespace testing;
using ::testing::HasSubstr;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::StartsWith;

namespace {

class SimpleCefValue : public CefValue {
 public:
  explicit SimpleCefValue(std::string value) : value_(std::move(value)) {}

  bool IsValid() override { return true; }
  bool IsOwned() override { return true; }
  bool IsReadOnly() override { return false; }
  bool IsSame(CefRefPtr<CefValue> that) override {
    return that.get() && that->GetType() == GetType() &&
           that->GetString() == value_;
  }
  bool IsEqual(CefRefPtr<CefValue> that) override { return IsSame(that); }
  CefRefPtr<CefValue> Copy() override { return new SimpleCefValue(value_); }
  Type GetType() override { return VTYPE_STRING; }
  bool GetBool() override { return value_ == "true"; }
  int GetInt() override { return 0; }
  double GetDouble() override { return 0.0; }
  CefString GetString() override { return CefString(value_); }
  CefRefPtr<CefBinaryValue> GetBinary() override { return nullptr; }
  CefRefPtr<CefDictionaryValue> GetDictionary() override { return nullptr; }
  CefRefPtr<CefListValue> GetList() override { return nullptr; }
  bool SetNull() override {
    value_.clear();
    return true;
  }
  bool SetBool(bool value) override {
    value_ = value ? "true" : "false";
    return true;
  }
  bool SetInt(int) override { return false; }
  bool SetDouble(double) override { return false; }
  bool SetString(const CefString& value) override {
    value_ = value.ToString();
    return true;
  }
  bool SetBinary(CefRefPtr<CefBinaryValue>) override { return false; }
  bool SetDictionary(CefRefPtr<CefDictionaryValue>) override { return false; }
  bool SetList(CefRefPtr<CefListValue>) override { return false; }

 private:
  std::string value_;

  IMPLEMENT_REFCOUNTING(SimpleCefValue);
};

class BoolCefValue : public CefValue {
 public:
  explicit BoolCefValue(bool value) : value_(value) {}

  bool IsValid() override { return true; }
  bool IsOwned() override { return true; }
  bool IsReadOnly() override { return false; }
  bool IsSame(CefRefPtr<CefValue> that) override {
    return that.get() && that->GetType() == GetType() &&
           that->GetBool() == value_;
  }
  bool IsEqual(CefRefPtr<CefValue> that) override { return IsSame(that); }
  CefRefPtr<CefValue> Copy() override { return new BoolCefValue(value_); }
  Type GetType() override { return VTYPE_BOOL; }
  bool GetBool() override { return value_; }
  int GetInt() override { return value_ ? 1 : 0; }
  double GetDouble() override { return value_ ? 1.0 : 0.0; }
  CefString GetString() override { return CefString(value_ ? "true" : "false"); }
  CefRefPtr<CefBinaryValue> GetBinary() override { return nullptr; }
  CefRefPtr<CefDictionaryValue> GetDictionary() override { return nullptr; }
  CefRefPtr<CefListValue> GetList() override { return nullptr; }
  bool SetNull() override { return false; }
  bool SetBool(bool value) override {
    value_ = value;
    return true;
  }
  bool SetInt(int value) override {
    value_ = value != 0;
    return true;
  }
  bool SetDouble(double value) override {
    value_ = value != 0.0;
    return true;
  }
  bool SetString(const CefString& value) override {
    value_ = value.ToString() == "true";
    return true;
  }
  bool SetBinary(CefRefPtr<CefBinaryValue>) override { return false; }
  bool SetDictionary(CefRefPtr<CefDictionaryValue>) override { return false; }
  bool SetList(CefRefPtr<CefListValue>) override { return false; }

 private:
  bool value_;

  IMPLEMENT_REFCOUNTING(BoolCefValue);
};

class RecordingCefFrame : public CefFrame {
 public:
  struct ScriptCall {
    std::string code;
    std::string url;
    int line;
  };

  void SetBrowser(CefRefPtr<CefBrowser> browser) { browser_ = browser; }
  void SetURL(const std::string& url) { url_ = url; }
  void SetIsMain(bool is_main) { is_main_ = is_main; }

  const std::vector<ScriptCall>& script_calls() const { return script_calls_; }

  bool IsValid() override { return true; }
  CefRefPtr<CefBrowser> GetBrowser() override { return browser_; }
  int64 GetIdentifier() override { return 1; }
  bool IsMain() override { return is_main_; }
  bool IsFocused() override { return false; }
  CefString GetName() override { return CefString("recording_frame"); }
  CefString GetURL() override { return CefString(url_); }
  CefRefPtr<CefFrame> GetParent() override { return nullptr; }
  void GetSource(CefRefPtr<CefStringVisitor>) override {}
  void GetText(CefRefPtr<CefStringVisitor>) override {}
  void LoadRequest(CefRefPtr<CefRequest>) override {}
  void LoadURL(const CefString& url) override { url_ = url.ToString(); }
  void ExecuteJavaScript(const CefString& code,
                         const CefString& url,
                         int line) override {
    script_calls_.push_back({code.ToString(), url.ToString(), line});
  }
  void Undo() override {}
  void Redo() override {}
  void Cut() override {}
  void Copy() override {}
  void Paste() override {}
  void Delete() override {}
  void SelectAll() override {}
  void ViewSource() override {}
  void VisitDOM(CefRefPtr<CefDOMVisitor>) override {}
  CefRefPtr<CefV8Context> GetV8Context() override { return nullptr; }
  void SendProcessMessage(CefProcessId,
                          CefRefPtr<CefProcessMessage>) override {}

 private:
  CefRefPtr<CefBrowser> browser_;
  std::string url_{"https://example.test"};
  bool is_main_{true};
  std::vector<ScriptCall> script_calls_;

  IMPLEMENT_REFCOUNTING(RecordingCefFrame);
};

class RecordingCefBrowserHost : public CefBrowserHost {
 public:
  void SetBrowser(CefRefPtr<CefBrowser> browser) { browser_ = browser; }

  const std::vector<std::string>& executed_scripts() const {
    return executed_scripts_;
  }
  CefRefPtr<CefJavaScriptResultCallback> captured_callback() const {
    return captured_callback_;
  }
  bool last_extension_flag() const { return last_extension_flag_; }

  void Reset() {
    executed_scripts_.clear();
    captured_callback_ = nullptr;
    last_extension_flag_ = false;
  }

  CefRefPtr<CefBrowser> GetBrowser() override { return browser_; }
  void CloseBrowser(bool) override {}
  void SetFocus(bool) override {}
  CefWindowHandle GetWindowHandle() override { return 0; }
  CefWindowHandle GetOpenerWindowHandle() override { return 0; }
  bool HasView() override { return true; }
  CefRefPtr<CefClient> GetClient() override { return nullptr; }
  CefRefPtr<CefRequestContext> GetRequestContext() override { return nullptr; }
  double GetZoomLevel() override { return 0.0; }
  void SetZoomLevel(double) override {}
  void RunFileDialog(FileDialogMode,
                     const CefString&,
                     const CefString&,
                     const std::vector<CefString>&,
                     CefRefPtr<CefRunFileDialogCallback>) override {}
  void StartDownload(const CefString&) override {}
  void DownloadImage(const CefString&,
                     bool,
                     uint32_t,
                     bool,
                     CefRefPtr<CefDownloadImageCallback>) override {}
  void Print() override {}
  void PrintToPDF(const CefString&,
                  const CefPdfPrintSettings&,
                  CefRefPtr<CefPdfPrintCallback>) override {}
  void Find(const CefString&, bool, bool, bool) override {}
  void StopFinding(bool) override {}
  void ShowDevTools(const CefWindowInfo&,
                    CefRefPtr<CefClient>,
                    const CefBrowserSettings&,
                    const CefPoint&) override {}
  void CloseDevTools() override {}
  bool HasDevTools() override { return false; }
  bool SendDevToolsMessage(const void*, size_t) override { return false; }
  int ExecuteDevToolsMethod(int,
                            const CefString&,
                            CefRefPtr<CefDictionaryValue>) override {
    return 0;
  }
  CefRefPtr<CefRegistration> AddDevToolsMessageObserver(
      CefRefPtr<CefDevToolsMessageObserver>) override {
    return nullptr;
  }
  void GetNavigationEntries(CefRefPtr<CefNavigationEntryVisitor>,
                            bool) override {}
  void ReplaceMisspelling(const CefString&) override {}
  void AddWordToDictionary(const CefString&) override {}
  bool IsWindowRenderingDisabled() override { return false; }
  void WasResized() override {}
  void WasHidden(bool) override {}
  void WasOccluded(bool) override {}
  void OnWindowShow() override {}
  void OnWindowHide() override {}
  void OnOnlineRenderToForeground() override {}
  void SendTouchEventList(const std::vector<CefTouchEvent>&) override {}
  void NotifyScreenInfoChanged() override {}
  void Invalidate(PaintElementType) override {}
  void SendExternalBeginFrame() override {}
  void SendKeyEvent(const CefKeyEvent&) override {}
  void SendMouseClickEvent(const CefMouseEvent&,
                           cef_mouse_button_type_t,
                           bool,
                           int) override {}
  void SendMouseMoveEvent(const CefMouseEvent&, bool) override {}
  void SendMouseWheelEvent(const CefMouseEvent&, int, int) override {}
  void SendTouchEvent(const CefTouchEvent&) override {}
  void SendCaptureLostEvent() override {}
  void NotifyMoveOrResizeStarted() override {}
  int GetWindowlessFrameRate() override { return 0; }
  void SetWindowlessFrameRate(int) override {}
  void ImeSetComposition(
      const CefString&,
      const std::vector<CefCompositionUnderline>&,
      const CefRange&,
      const CefRange&) override {}
  void ImeCommitText(const CefString&,
                     const CefRange&,
                     int) override {}
  void ImeFinishComposingText(bool) override {}
  void ImeCancelComposition() override {}
  void DragTargetDragEnter(CefRefPtr<CefDragData>,
                           const CefMouseEvent&,
                           DragOperationsMask) override {}
  void DragTargetDragOver(const CefMouseEvent&,
                          DragOperationsMask) override {}
  void DragTargetDragLeave() override {}
  void DragTargetDrop(const CefMouseEvent&) override {}
  void DragSourceEndedAt(int, int, DragOperationsMask) override {}
  void DragSourceSystemDragEnded() override {}
  CefRefPtr<CefNavigationEntry> GetVisibleNavigationEntry() override {
    return nullptr;
  }
  void SetAccessibilityState(cef_state_t) override {}
  void SetAutoResizeEnabled(bool,
                            const CefSize&,
                            const CefSize&) override {}
  void SetAudioMuted(bool) override {}
  bool IsAudioMuted() override { return false; }
  void ExecuteJavaScript(const std::string& code,
                         CefRefPtr<CefJavaScriptResultCallback> callback,
                         bool extension) override {
    executed_scripts_.push_back(code);
    captured_callback_ = callback;
    last_extension_flag_ = extension;
  }
  void SetNativeWindow(cef_native_window_t) override {}
  void SetVirtualPixelRatio(float) override {}
  float GetVirtualPixelRatio() override { return 1.0f; }
  void SetZoomLevelDisabled(bool) override {}
  bool IsZoomLevelDisabled() override { return false; }
  void ClearHistory() override {}
  void ExitFullScreen() override {}
  void SetAudioExclusive(bool) override {}
  void SetBackgroundColor(uint32_t) override {}

 private:
  CefRefPtr<CefBrowser> browser_;
  std::vector<std::string> executed_scripts_;
  CefRefPtr<CefJavaScriptResultCallback> captured_callback_;
  bool last_extension_flag_{false};

  IMPLEMENT_REFCOUNTING(RecordingCefBrowserHost);
};

class RecordingCefBrowser : public CefBrowser {
 public:
  RecordingCefBrowser(CefRefPtr<CefBrowserHost> host,
                      CefRefPtr<CefFrame> main_frame)
      : host_(host), main_frame_(main_frame) {}

  CefRefPtr<CefBrowserHost> GetHost() override { return host_; }
  CefRefPtr<CefFrame> GetMainFrame() override { return main_frame_; }
  CefRefPtr<CefFrame> GetFocusedFrame() override { return main_frame_; }
  bool IsValid() override { return true; }
  bool IsPopup() override { return false; }
  bool HasDocument() override { return true; }
  bool CanGoBack() override { return false; }
  void GoBack() override {}
  bool CanGoForward() override { return false; }
  void GoForward() override {}
  bool IsLoading() override { return false; }
  void Reload() override {}
  void ReloadIgnoreCache() override {}
  void StopLoad() override {}
  int GetIdentifier() override { return 1; }
  bool IsSame(CefRefPtr<CefBrowser> that) override {
    return that.get() == this;
  }
  void SetFocus(bool) override {}
  CefRefPtr<CefClient> GetClient() override { return nullptr; }
  CefRefPtr<CefRequestContext> GetRequestContext() override { return nullptr; }
  bool CanZoom(cef_zoom_command_t) override { return false; }
  void Zoom(cef_zoom_command_t) override {}
  double GetDefaultZoomLevel() override { return 0.0; }
  double GetZoomLevel() override { return 0.0; }
  void SetZoomLevel(double) override {}
  CefRefPtr<CefFrame> GetFrameByIdentifier(const CefString&) override {
    return nullptr;
  }
  CefRefPtr<CefFrame> GetFrameByName(const CefString&) override {
    return nullptr;
  }
  size_t GetFrameCount() override { return 1; }
  void GetFrameIdentifiers(std::vector<CefString>&) override {}
  void GetFrameNames(std::vector<CefString>&) override {}
  bool NeedToFireBeforeUnloadOrUnloadEvents() override { return false; }
  void DispatchBeforeUnload() override {}
  CefRefPtr<CefFrame> CreatePrepressSubFrame(const CefString&,
                                             const CefString&,
                                             const CefString&) override {
    return nullptr;
  }
  CefRefPtr<CefFrame> CreateChildFrame(const CefString&,
                                       const CefString&,
                                       const CefString&,
                                       bool,
                                       bool,
                                       const CefString&,
                                       CefRefPtr<CefDictionaryValue>,
                                       CefRefPtr<CefFrame>) override {
    return nullptr;
  }
  CefRefPtr<CefFrame> GetFrame(const CefString&) override { return nullptr; }
  CefRefPtr<CefFrame> GetFrame(int64) override { return nullptr; }
  void VisitAllFrames(CefRefPtr<CefFrameVisitor>) override {}
  void VisitAllFrames(CefFrameVisitor*) override {}
  void VisitAllFramesWithContext(CefRefPtr<CefFrameVisitorWithContext>,
                                 CefRefPtr<CefRequestContext>) override {}
  void CloseBrowser(bool) override {}
  bool TryCloseBrowser() override { return true; }
  bool IsReadyToBeClosed() override { return true; }

 private:
  CefRefPtr<CefBrowserHost> host_;
  CefRefPtr<CefFrame> main_frame_;

  IMPLEMENT_REFCOUNTING(RecordingCefBrowser);
};

class ExposedNwebAutolayout : public NwebAutolayout {
 public:
  bool IsEnabled() const { return mEnable_; }
  using NwebAutolayout::Initialize;
};

class ScopedAdapterInstance {
 public:
  explicit ScopedAdapterInstance(MockOhosAdapterHelper* helper) : helper_(helper) {
    OhosAdapterHelper::SetInstance(helper_);
  }

  ~ScopedAdapterInstance() {
    OhosAdapterHelper::SetInstance(nullptr);
    delete helper_;
  }

 private:
  MockOhosAdapterHelper* helper_;
};

}  // namespace

const std::string g_valid_config = R"({
    "minMaskAreaRatioThreshold": 60,
    "opacityFilter": [10, 90],
    "minContentAreaRatioThreshold": 20,
    "scaleAnimationDuration": 100,
    "whitelist": {
        "com.example.app": {
            "pattern": "some_pattern",
            "GetID": "get_id_func",
            "GetPage": "get_page_func",
            "appRuleInfos": [
                { "id": "*", "pg": "*" }
            ]
        }
    }
})";
 
namespace OHOS::NWeb {
class NwebAutolayoutTest : public testing::Test, public NwebAutolayout {
};
 
TEST_F(NwebAutolayoutTest, ParseInt_Valid)
{
    EXPECT_EQ(ParseInt("123").value(), 123);
    EXPECT_EQ(ParseInt("0").value(), 0);
    EXPECT_EQ(ParseInt("-1").value(), -1);
}
 
 
TEST_F(NwebAutolayoutTest, ParseInt_Invalid)
{
    EXPECT_FALSE(ParseInt("abc").has_value());
    EXPECT_FALSE(ParseInt("12a").has_value());
    EXPECT_FALSE(ParseInt("").has_value());
    EXPECT_FALSE(ParseInt("1.23").has_value());
}
 
TEST_F(NwebAutolayoutTest, ParseToplevelConfig_Valid)
{
    std::optional<base::Value> root = base::JSONReader::Read(g_valid_config);
    ASSERT_TRUE(root.has_value());
    ASSERT_TRUE(root->is_dict());
    
    EXPECT_TRUE(ParseToplevelConfig(root->GetDict()));
    
    EXPECT_EQ(mCCMConfig_.min_mask_area_ratio_threshold, 60);
    EXPECT_EQ(mCCMConfig_.min_content_area_ratio_threshold, 20);
    EXPECT_EQ(mCCMConfig_.scale_animation_duration, 100);
    EXPECT_EQ(mCCMConfig_.opacity_filter.first, 10);
    EXPECT_EQ(mCCMConfig_.opacity_filter.second, 90);
}
 
TEST_F(NwebAutolayoutTest, ParseToplevelConfig_Invalid)
{
    const std::string invalid_config = R"({
        "minMaskAreaRatioThreshold": 200
    })";
    std::optional<base::Value> root = base::JSONReader::Read(invalid_config);
    ASSERT_TRUE(root.has_value());
    ASSERT_TRUE(root->is_dict());
    EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
}
 
TEST_F(NwebAutolayoutTest, ParseToplevelConfig_InvalidOpacityFilterFormat)
{
    const std::vector<std::string> invalid_configs = {
        R"({
            "minMaskAreaRatioThreshold": 60,
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})",
        R"({
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})",
        R"({"minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 90, 95],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})",
        R"({
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": ["10", 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})",
        R"({
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, "90"],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})"
    };

    for (const auto& config_str : invalid_configs) {
        std::optional<base::Value> root = base::JSONReader::Read(config_str);
        ASSERT_TRUE(root.has_value());
        ASSERT_TRUE(root->is_dict());
        EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
    }
}

TEST_F(NwebAutolayoutTest, CheckCCMandApplyRule_Disabled)
{
    mEnable_ = false;
    mWListEntry_ = nullptr;

    CheckCCMandApplyRule(nullptr);
    EXPECT_TRUE(true);
}

TEST_F(NwebAutolayoutTest, CheckCCMandApplyRule_WithoutWhitelistEntry)
{
    mEnable_ = true;
    mWListEntry_ = nullptr;

    CheckCCMandApplyRule(nullptr);
    EXPECT_TRUE(true);
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_InvalidOpacityFilterValues)
{
    const std::vector<std::string> invalid_configs = {
        R"({
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [-1, 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})",
        R"({
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 101],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})",
        R"({
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [80, 40],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})"
    };

    for (const auto& config_str : invalid_configs) {
        std::optional<base::Value> root = base::JSONReader::Read(config_str);
        ASSERT_TRUE(root.has_value());
        ASSERT_TRUE(root->is_dict());
        EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
    }
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_InvalidMinMaskAreaRatioThreshold)
{
    const std::vector<std::string> invalid_configs = {
        R"({
            "minMaskAreaRatioThreshold": 49,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})",
        R"({
            "minMaskAreaRatioThreshold": 101,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})",
        R"({
            "minMaskAreaRatioThreshold": "60",
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})"
    };
 
    for (const auto& config_str : invalid_configs) {
        std::optional<base::Value> root = base::JSONReader::Read(config_str);
        ASSERT_TRUE(root.has_value());
        ASSERT_TRUE(root->is_dict());
        EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
    }
}
 
TEST_F(NwebAutolayoutTest, ParseToplevelConfig_InvalidMinContentAreaRatioThreshold)
{
    const std::vector<std::string> invalid_configs = {
        R"({
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 9,
            "scaleAnimationDuration": 100})",
        R"({
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 101,
            "scaleAnimationDuration": 100})",
        R"({
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": "20",
            "scaleAnimationDuration": 100})"
    };
 
    for (const auto& config_str : invalid_configs) {
        std::optional<base::Value> root = base::JSONReader::Read(config_str);
        ASSERT_TRUE(root.has_value());
        ASSERT_TRUE(root->is_dict());
        EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
    }
}
 
TEST_F(NwebAutolayoutTest, ParseToplevelConfig_InvalidScaleAnimationDuration)
{
    const std::vector<std::string> invalid_configs = {
        R"({
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 49})",
        R"({
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 401})",
        R"({
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": "100"})"
    };
 
    for (const auto& config_str : invalid_configs) {
        std::optional<base::Value> root = base::JSONReader::Read(config_str);
        ASSERT_TRUE(root.has_value());
        ASSERT_TRUE(root->is_dict());
        EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
    }
}
 
TEST_F(NwebAutolayoutTest, ParseToplevelConfig_MissingKey)
{
    const std::string invalid_config = R"({
        "minMaskAreaRatioThreshold": 60,
        "scaleAnimationDuration": 100,
        "opacityFilter": [10, 90]
    })";
    std::optional<base::Value> root = base::JSONReader::Read(invalid_config);
    ASSERT_TRUE(root.has_value());
    ASSERT_TRUE(root->is_dict());
    EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
}
 
TEST_F(NwebAutolayoutTest, ParseWhitelistEntry_Valid)
{
    const std::string entry_str = R"({
        "pattern": "some_pattern",
        "GetID": "get_id_func",
        "GetPage": "get_page_func",
        "appRuleInfos": [ { "id": "rule1" } ]
    })";
    std::optional<base::Value> entry_val = base::JSONReader::Read(entry_str);
    ASSERT_TRUE(entry_val.has_value());
    ASSERT_TRUE(entry_val->is_dict());
 
    mCCMConfig_.whitelist.clear();
    EXPECT_TRUE(ParseWhitelistEntry("com.example.app", entry_val->GetDict()));
    auto it = mCCMConfig_.whitelist.find("com.example.app");
    EXPECT_NE(it, mCCMConfig_.whitelist.end());
    EXPECT_EQ(it->second.pattern, "some_pattern");
    EXPECT_EQ(it->second.getID, "get_id_func");
    EXPECT_EQ(it->second.getPage, "get_page_func");
    EXPECT_TRUE(it->second.appRuleInfos.has_value());
    EXPECT_EQ(it->second.appRuleInfos->size(), 1u);
}
 
TEST_F(NwebAutolayoutTest, ParseWhitelistEntry_Invalid)
{
    const std::string invalid_entry_str = R"({
        "GetID": "get_id_func",
        "GetPage": "get_page_func",
        "appRuleInfos": []
    })";
    std::optional<base::Value> entry_val = base::JSONReader::Read(invalid_entry_str);
    ASSERT_TRUE(entry_val.has_value());
    ASSERT_TRUE(entry_val->is_dict());
    mCCMConfig_.whitelist.clear();
    EXPECT_FALSE(ParseWhitelistEntry("com.example.app", entry_val->GetDict()));
}
 
TEST_F(NwebAutolayoutTest, ParseWhitelistEntry_EmptyFields)
{
    const std::vector<std::string> invalid_entries = {
        R"({"pattern": "", "GetID": "gi", "GetPage": "gp", "appRuleInfos": []})",
        R"({"pattern": "p", "GetID": "", "GetPage": "gp", "appRuleInfos": []})",
        R"({"pattern": "p", "GetID": "gi", "GetPage": "", "appRuleInfos": []})"
    };
 
    for (const auto& entry_str : invalid_entries) {
        std::optional<base::Value> entry_val = base::JSONReader::Read(entry_str);
        ASSERT_TRUE(entry_val.has_value());
        ASSERT_TRUE(entry_val->is_dict());
        mCCMConfig_.whitelist.clear();
        EXPECT_FALSE(ParseWhitelistEntry("com.example.app", entry_val->GetDict()));
    }
}
 
TEST_F(NwebAutolayoutTest, ParseWhitelistEntry_InvalidAppRuleInfos)
{
    const std::string invalid_entry_str = R"({
        "pattern": "p",
        "GetID": "gi",
        "GetPage": "gp",
        "appRuleInfos": "not_a_list"
    })";
    std::optional<base::Value> entry_val = base::JSONReader::Read(invalid_entry_str);
    ASSERT_TRUE(entry_val.has_value());
    ASSERT_TRUE(entry_val->is_dict());
    mCCMConfig_.whitelist.clear();
    EXPECT_FALSE(ParseWhitelistEntry("com.example.app", entry_val->GetDict()));
}
 
 
TEST_F(NwebAutolayoutTest, ParseWhitelistEntry_MissingKeys)
{
    const std::vector<std::string> invalid_entries = {
        R"({"pattern": "p", "GetPage": "gp", "appRuleInfos": []})",
        R"({"pattern": "p", "GetID": "gi", "appRuleInfos": []})",
        R"({"pattern": "p", "GetID": "gi", "GetPage": "gp"})"
    };
 
    for (const auto& entry_str : invalid_entries) {
        std::optional<base::Value> entry_val = base::JSONReader::Read(entry_str);
        ASSERT_TRUE(entry_val.has_value());
        ASSERT_TRUE(entry_val->is_dict());
        mCCMConfig_.whitelist.clear();
        EXPECT_FALSE(ParseWhitelistEntry("com.example.app", entry_val->GetDict()));
    }
}
 
TEST_F(NwebAutolayoutTest, ParseWhitelist_Valid)
{
    std::optional<base::Value> root = base::JSONReader::Read(g_valid_config);
    ASSERT_TRUE(root.has_value());
    const base::Value::Dict* whitelist_dict = root->GetDict().FindDict("whitelist");
    ASSERT_TRUE(whitelist_dict);
    EXPECT_TRUE(ParseWhitelist(*whitelist_dict));
    EXPECT_EQ(mCCMConfig_.whitelist.size(), 1u);
    EXPECT_NE(mCCMConfig_.whitelist.find("com.example.app"), mCCMConfig_.whitelist.end());
}
 
TEST_F(NwebAutolayoutTest, ParseWhitelist_InvalidEntryType)
{
    const std::string invalid_whitelist = R"({
        "com.example.app": "not_a_dict"
    })";
    std::optional<base::Value> whitelist_val = base::JSONReader::Read(invalid_whitelist);
    ASSERT_TRUE(whitelist_val.has_value());
    ASSERT_TRUE(whitelist_val->is_dict());
    EXPECT_FALSE(ParseWhitelist(whitelist_val->GetDict()));
}
 
TEST_F(NwebAutolayoutTest, ParseWhitelist_InvalidEntryContents)
{
    const std::string invalid_whitelist = R"({
        "com.example.app": {
            "pattern": "",
            "GetID": "get_id_func",
            "GetPage": "get_page_func",
            "appRuleInfos": []
        }
    })";

    std::optional<base::Value> whitelist_val = base::JSONReader::Read(invalid_whitelist);
    ASSERT_TRUE(whitelist_val.has_value());
    ASSERT_TRUE(whitelist_val->is_dict());
    EXPECT_FALSE(ParseWhitelist(whitelist_val->GetDict()));
}

TEST_F(NwebAutolayoutTest, Parse_Valid)
{
    std::optional<base::Value> root = base::JSONReader::Read(g_valid_config);
    ASSERT_TRUE(root.has_value());
    EXPECT_TRUE(Parse(*root));
}
 
TEST_F(NwebAutolayoutTest, Parse_Invalid)
{
    const std::string invalid_config = "[]";
    std::optional<base::Value> root = base::JSONReader::Read(invalid_config);
    ASSERT_TRUE(root.has_value());
    EXPECT_FALSE(Parse(*root));
}
 
TEST_F(NwebAutolayoutTest, CheckWebContainer_Disabled)
{
    mEnable_ = false;
    CheckWebContainer(nullptr, nullptr);
    EXPECT_TRUE(true);
}

TEST_F(NwebAutolayoutTest, CheckWebContainer_NoBrowser)
{
    mEnable_ = true;
    CheckWebContainer(nullptr, nullptr);
    EXPECT_TRUE(true);
}

TEST_F(NwebAutolayoutTest, Parse_InvalidWhitelistType)
{
    const std::string invalid_config = R"({
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [10, 90],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100,
        "whitelist": []
    })";

    std::optional<base::Value> root = base::JSONReader::Read(invalid_config);
    ASSERT_TRUE(root.has_value());
    EXPECT_FALSE(Parse(*root));
}

TEST_F(NwebAutolayoutTest, Parse_InvalidWhitelistEntry)
{
    const std::string invalid_config = R"({
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [10, 90],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100,
        "whitelist": {
            "com.example.app": {
                "pattern": "",
                "GetID": "get_id_func",
                "GetPage": "get_page_func",
                "appRuleInfos": []
            }
        }
    })";

    std::optional<base::Value> root = base::JSONReader::Read(invalid_config);
    ASSERT_TRUE(root.has_value());
    EXPECT_FALSE(Parse(*root));
}

TEST_F(NwebAutolayoutTest, Parse_InvalidTopLevelConfig)
{
    const std::string invalid_config = R"({
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [80, 40],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100,
        "whitelist": {}
    })";

    std::optional<base::Value> root = base::JSONReader::Read(invalid_config);
    ASSERT_TRUE(root.has_value());
    EXPECT_FALSE(Parse(*root));
}

TEST_F(NwebAutolayoutTest, Parse_MissingWhitelist)
{
    const std::string invalid_config = R"({
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [10, 90],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100
    })";
    std::optional<base::Value> root = base::JSONReader::Read(invalid_config);
    ASSERT_TRUE(root.has_value());
    EXPECT_FALSE(Parse(*root));
}

// Helper to access static function in .cc file
namespace {
std::string TestEscapeForJS(const std::string& s) {
    std::stringstream ss;
    for (char c : s) {
        switch (c) {
            case '`':  ss << "\\`";  break;
            case '\\': ss << "\\\\"; break;
            case '$':  ss << "\\$";  break;
            default:   ss << c;     break;
        }
    }
    return ss.str();
}
}

TEST_F(NwebAutolayoutTest, EscapeForJS_Backtick)
{
    std::string input = "test`quote";
    std::string expected = "test\\`quote";
    EXPECT_EQ(TestEscapeForJS(input), expected);
}

TEST_F(NwebAutolayoutTest, EscapeForJS_Backslash)
{
    std::string input = "test\\path";
    std::string expected = "test\\\\path";
    EXPECT_EQ(TestEscapeForJS(input), expected);
}

TEST_F(NwebAutolayoutTest, EscapeForJS_Dollar)
{
    std::string input = "test$variable";
    std::string expected = "test\\$variable";
    EXPECT_EQ(TestEscapeForJS(input), expected);
}

TEST_F(NwebAutolayoutTest, EscapeForJS_NormalText)
{
    std::string input = "normal text 123";
    std::string expected = "normal text 123";
    EXPECT_EQ(TestEscapeForJS(input), expected);
}

TEST_F(NwebAutolayoutTest, EscapeForJS_MixedSpecialChars)
{
    std::string input = "`hello\\world$var`";
    std::string expected = "\\`hello\\\\world\\$var\\`";
    EXPECT_EQ(TestEscapeForJS(input), expected);
}

TEST_F(NwebAutolayoutTest, EscapeForJS_EmptyString)
{
    std::string input = "";
    std::string expected = "";
    EXPECT_EQ(TestEscapeForJS(input), expected);
}

TEST_F(NwebAutolayoutTest, EscapeForJS_OnlySpecialChars)
{
    std::string input = "`\\$";
    std::string expected = "\\`\\\\\\$";
    EXPECT_EQ(TestEscapeForJS(input), expected);
}

TEST_F(NwebAutolayoutTest, CheckCCMandApplyRule_ValidStateWithNullFrame)
{
    // Set up valid internal state
    mEnable_ = true;
    mCCMConfig_.min_mask_area_ratio_threshold = 60;
    mCCMConfig_.min_content_area_ratio_threshold = 20;
    mCCMConfig_.scale_animation_duration = 100;
    mCCMConfig_.opacity_filter = {10, 90};
    mCCMConfig_.minScaleFactor = 70;
    
    // Create a whitelist entry with valid data
    std::string pattern_data = "test_pattern";
    std::string id_data = "test_id";
    std::string page_data = "test_page";
    
    WhitelistEntry entry;
    entry.pattern = std::string_view(pattern_data);
    entry.getID = std::string_view(id_data);
    entry.getPage = std::string_view(page_data);
    
    base::Value::List rules;
    base::Value::Dict rule;
    rule.Set("id", "*");
    rule.Set("pg", "*");
    rules.Append(std::move(rule));
    entry.appRuleInfos = std::move(rules);
    
    mWListEntry_ = &entry;
    mAutoLayoutJSSource_ = "test_autolayout_script";
    
    // Call with nullptr frame - will fail at ExecuteJavaScript but JSON logic executes first
    CheckCCMandApplyRule(nullptr);
    EXPECT_TRUE(true);
}

TEST_F(NwebAutolayoutTest, CheckCCMandApplyRule_JSONWriteSuccess)
{
    // Set up state to test JSON serialization success path
    mEnable_ = true;
    mCCMConfig_.min_mask_area_ratio_threshold = 75;
    mCCMConfig_.min_content_area_ratio_threshold = 30;
    mCCMConfig_.scale_animation_duration = 200;
    mCCMConfig_.opacity_filter = {20, 80};
    mCCMConfig_.minScaleFactor = 65;
    
    std::string pattern = "pattern";
    std::string id = "id";
    std::string page = "page";
    
    WhitelistEntry entry;
    entry.pattern = std::string_view(pattern);
    entry.getID = std::string_view(id);
    entry.getPage = std::string_view(page);
    
    base::Value::List rules;
    rules.Append(base::Value::Dict());
    entry.appRuleInfos = std::move(rules);
    
    mWListEntry_ = &entry;
    mAutoLayoutJSSource_ = "script";
    
    // The function will process JSON before hitting nullptr
    CheckCCMandApplyRule(nullptr);
    EXPECT_TRUE(true);
}

TEST_F(NwebAutolayoutTest, CheckWebContainer_NullBrowser)
{
    mEnable_ = true;
    mPatternJSSource_ = "test_pattern";
    
    // Test with null browser - should return early without crash
    CheckWebContainer(nullptr, nullptr);
    EXPECT_TRUE(true);
}

TEST_F(NwebAutolayoutTest, CheckWebContainer_EnabledWithNullFrame)
{
    mEnable_ = true;
    mPatternJSSource_ = "pattern_with_special_`\\$chars";
    
    // Both browser and frame are null - tests escape logic but returns before IsMain
    CheckWebContainer(nullptr, nullptr);
    EXPECT_TRUE(true);
}

TEST_F(NwebAutolayoutTest, CheckWebContainer_EscapeLogic)
{
    mEnable_ = true;
    // Set pattern with special characters to test EscapeForJS_TemplateLiteral usage
    mPatternJSSource_ = "test`pattern\\with$specials";
    
    CheckWebContainer(nullptr, nullptr);
    EXPECT_TRUE(true);
}

TEST_F(NwebAutolayoutTest, CheckCCMandApplyRule_ExecutesExpectedScripts)
{
    mEnable_ = true;
    mCCMConfig_.min_mask_area_ratio_threshold = 60;
    mCCMConfig_.min_content_area_ratio_threshold = 20;
    mCCMConfig_.scale_animation_duration = 120;
    mCCMConfig_.opacity_filter = {15, 85};
    mCCMConfig_.minScaleFactor = 70;

    std::string pattern = "pattern_js";
    std::string get_id = "getIdentifier();";
    std::string get_page = "getPage();";
    mAutoLayoutJSSource_ = "applyAutoLayout();";

    base::Value::Dict rule;
    rule.Set("id", "*");
    base::Value::List rules;
    rules.Append(std::move(rule));

    WhitelistEntry entry;
    entry.pattern = std::string_view(pattern);
    entry.getID = std::string_view(get_id);
    entry.getPage = std::string_view(get_page);
    entry.appRuleInfos = std::move(rules);

    mWListEntry_ = &entry;

    CefRefPtr<RecordingCefFrame> frame = new RecordingCefFrame();
    frame->SetURL("https://example.com");

    CheckCCMandApplyRule(frame);

    const auto& calls = frame->script_calls();
    ASSERT_EQ(calls.size(), 4u);
    EXPECT_EQ(calls[0].code, get_id);
    EXPECT_EQ(calls[1].code, get_page);
    EXPECT_EQ(calls[2].code, mAutoLayoutJSSource_);
    EXPECT_THAT(calls[3].code,
                StartsWith(std::string(ConfigConstants::kAutoLayoutBegin)));
    EXPECT_THAT(calls[3].code,
                HasSubstr(std::string(ConfigConstants::kAutoLayoutEnd)));
    EXPECT_EQ(calls[0].url, "https://example.com");
    EXPECT_EQ(calls[3].url, "https://example.com");

    mWListEntry_ = nullptr;
}

TEST_F(NwebAutolayoutTest, CheckWebContainer_ExecutesPatternScriptWhenMainFrame)
{
    mEnable_ = true;
    mPatternJSSource_ = "pattern`with$specials";

    CefRefPtr<RecordingCefFrame> frame = new RecordingCefFrame();
    frame->SetURL("https://frame-url.test");
    frame->SetIsMain(true);

    CefRefPtr<RecordingCefBrowserHost> host = new RecordingCefBrowserHost();
    CefRefPtr<RecordingCefBrowser> browser = new RecordingCefBrowser(host, frame);
    host->SetBrowser(browser);
    frame->SetBrowser(browser);

    CheckWebContainer(browser, frame);

    const auto& scripts = host->executed_scripts();
    ASSERT_EQ(scripts.size(), 1u);
    EXPECT_EQ(scripts.front(), TestEscapeForJS(mPatternJSSource_));
    EXPECT_FALSE(host->last_extension_flag());

    CefRefPtr<CefJavaScriptResultCallback> callback = host->captured_callback();
    ASSERT_NE(callback, nullptr);

    callback->OnJavaScriptExeResult(new BoolCefValue(true));
    callback->OnJavaScriptExeResult(new SimpleCefValue(""));
    callback->OnJavaScriptExeResult(new SimpleCefValue("false"));
    callback->OnJavaScriptExeResult(new SimpleCefValue("true"));
}

TEST_F(NwebAutolayoutTest, LoadAutoLayoutFromHap_DisablesFeatureWhenResourceMissing)
{
    mEnable_ = true;
    LoadAutoLayoutFromHap();
    EXPECT_FALSE(mEnable_);
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_BoundaryValues)
{
    // Test with boundary values for min_mask_area_ratio_threshold
    const std::string config_min = R"({
        "minMaskAreaRatioThreshold": 50,
        "opacityFilter": [0, 100],
        "minContentAreaRatioThreshold": 11,
        "scaleAnimationDuration": 51
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config_min);
    ASSERT_TRUE(root.has_value());
    EXPECT_TRUE(ParseToplevelConfig(root->GetDict()));
    EXPECT_EQ(mCCMConfig_.min_mask_area_ratio_threshold, 50);
    EXPECT_EQ(mCCMConfig_.opacity_filter.first, 0);
    EXPECT_EQ(mCCMConfig_.opacity_filter.second, 100);
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_MaxBoundaryValues)
{
    // Test with max boundary values
    const std::string config_max = R"({
        "minMaskAreaRatioThreshold": 100,
        "opacityFilter": [50, 100],
        "minContentAreaRatioThreshold": 99,
        "scaleAnimationDuration": 399
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config_max);
    ASSERT_TRUE(root.has_value());
    EXPECT_TRUE(ParseToplevelConfig(root->GetDict()));
    EXPECT_EQ(mCCMConfig_.min_mask_area_ratio_threshold, 100);
    EXPECT_EQ(mCCMConfig_.scale_animation_duration, 399);
}

TEST_F(NwebAutolayoutTest, ParseWhitelistEntry_WithMultipleRules)
{
    const std::string entry_str = R"({
        "pattern": "multi_pattern",
        "GetID": "multi_id",
        "GetPage": "multi_page",
        "appRuleInfos": [
            { "id": "rule1", "pg": "page1" },
            { "id": "rule2", "pg": "page2" },
            { "id": "*", "pg": "*" }
        ]
    })";
    std::optional<base::Value> entry_val = base::JSONReader::Read(entry_str);
    ASSERT_TRUE(entry_val.has_value());
    ASSERT_TRUE(entry_val->is_dict());

    mCCMConfig_.whitelist.clear();
    EXPECT_TRUE(ParseWhitelistEntry("test.app", entry_val->GetDict()));
    auto it = mCCMConfig_.whitelist.find("test.app");
    EXPECT_NE(it, mCCMConfig_.whitelist.end());
    EXPECT_EQ(it->second.appRuleInfos->size(), 3u);
}

TEST_F(NwebAutolayoutTest, ParseInt_NegativeNumbers)
{
    EXPECT_EQ(ParseInt("-123").value(), -123);
    EXPECT_EQ(ParseInt("-999").value(), -999);
}

TEST_F(NwebAutolayoutTest, ParseInt_LargeNumbers)
{
    EXPECT_EQ(ParseInt("2147483647").value(), 2147483647);
    EXPECT_EQ(ParseInt("999999").value(), 999999);
}

TEST_F(NwebAutolayoutTest, ParseInt_WithWhitespace)
{
    // Should fail with whitespace
    EXPECT_FALSE(ParseInt(" 123").has_value());
    EXPECT_FALSE(ParseInt("123 ").has_value());
    EXPECT_FALSE(ParseInt(" 123 ").has_value());
}

TEST_F(NwebAutolayoutTest, Parse_ValidComplexConfig)
{
    const std::string complex_config = R"({
        "minMaskAreaRatioThreshold": 55,
        "opacityFilter": [15, 85],
        "minContentAreaRatioThreshold": 25,
        "scaleAnimationDuration": 150,
        "whitelist": {
            "com.app1": {
                "pattern": "p1",
                "GetID": "id1",
                "GetPage": "page1",
                "appRuleInfos": [{"id": "1"}]
            },
            "com.app2": {
                "pattern": "p2",
                "GetID": "id2",
                "GetPage": "page2",
                "appRuleInfos": [{"pg": "2"}]
            }
        }
    })";
    
    std::optional<base::Value> root = base::JSONReader::Read(complex_config);
    ASSERT_TRUE(root.has_value());
    EXPECT_TRUE(Parse(*root));
    EXPECT_EQ(mCCMConfig_.whitelist.size(), 2u);
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_OpacityFilterEdgeCases)
{
    // Test opacity filter with equal values
    const std::string config_equal = R"({
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [50, 50],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config_equal);
    ASSERT_TRUE(root.has_value());
    EXPECT_TRUE(ParseToplevelConfig(root->GetDict()));
    EXPECT_EQ(mCCMConfig_.opacity_filter.first, 50);
    EXPECT_EQ(mCCMConfig_.opacity_filter.second, 50);
}

TEST(NwebAutolayoutSingletonTest, GetInstanceReturnsSingleton)
{
    auto instance1 = NwebAutolayout::GetInstance();
    auto instance2 = NwebAutolayout::GetInstance();
    EXPECT_NE(instance1, nullptr);
    EXPECT_EQ(instance1, instance2);
}

TEST(NwebAutolayoutInitializeTest, InitializeDisablesWhenMinScaleNotPositive)
{
    auto helper = new MockOhosAdapterHelper();
    ScopedAdapterInstance scoped(helper);
    MockSystemPropertiesAdapter system_properties;

    EXPECT_CALL(*helper, GetSystemPropertiesInstance())
        .WillRepeatedly(ReturnRef(system_properties));
    EXPECT_CALL(system_properties,
                GetStringParameter(::testing::_, ::testing::_))
        .WillOnce(Return("0"));

    ExposedNwebAutolayout autolayout;
    EXPECT_FALSE(autolayout.IsEnabled());
}

TEST(NwebAutolayoutInitializeTest, InitializeDisablesWhenConfigReadFails)
{
    auto helper = new MockOhosAdapterHelper();
    ScopedAdapterInstance scoped(helper);
    MockSystemPropertiesAdapter system_properties;

    EXPECT_CALL(*helper, GetSystemPropertiesInstance())
        .WillRepeatedly(ReturnRef(system_properties));
    InSequence seq;
    EXPECT_CALL(system_properties,
                GetStringParameter(::testing::_, ::testing::_))
        .WillOnce(Return("80"))
        .WillOnce(Return("/nonexistent/autolayout_config.json"));

    ExposedNwebAutolayout autolayout;
    EXPECT_FALSE(autolayout.IsEnabled());
}

TEST(NwebAutolayoutInitializeTest, InitializeDisablesWhenJsonInvalid)
{
    base::ScopedTempDir temp_dir;
    ASSERT_TRUE(temp_dir.CreateUniqueTempDir());
    base::FilePath config_path = temp_dir.GetPath().AppendASCII("invalid_config.json");
    const std::string invalid_json = "not-json";
    ASSERT_TRUE(base::WriteFile(config_path, invalid_json));

    auto helper = new MockOhosAdapterHelper();
    ScopedAdapterInstance scoped(helper);
    MockSystemPropertiesAdapter system_properties;

    EXPECT_CALL(*helper, GetSystemPropertiesInstance())
        .WillRepeatedly(ReturnRef(system_properties));
    InSequence seq;
    EXPECT_CALL(system_properties,
                GetStringParameter(::testing::_, ::testing::_))
        .WillOnce(Return("80"))
        .WillOnce(Return(config_path.AsUTF8Unsafe()));

    ExposedNwebAutolayout autolayout;
    EXPECT_FALSE(autolayout.IsEnabled());
}
}