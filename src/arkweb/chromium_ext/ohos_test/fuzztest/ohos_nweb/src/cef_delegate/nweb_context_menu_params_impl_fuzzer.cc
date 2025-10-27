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

#include "arkweb/ohos_nweb/src/cef_delegate/nweb_touch_handle_state_impl.h"
#include "cef/libcef/common/drag_data_impl.h"
#include "ohos_nweb/src/cef_delegate/nweb_context_menu_params_impl.h"

#include <cstring>
#include <fuzzer/FuzzedDataProvider.h>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "ohos_nweb/include/nweb.h"
#include "ohos_nweb/src/nweb_impl.h"

using namespace OHOS::NWeb;

std::shared_ptr<NWebTouchHandleState> GetTouchHandleState(
    FuzzedDataProvider* fdp,
    NWebTouchHandleState::TouchHandleType type) {
    CefTouchHandleState state;
    state.touch_handle_id = fdp->ConsumeIntegralInRange<int>(0, 100);
    state.origin.x = fdp->ConsumeIntegralInRange<int>(0, 100);
    state.origin.y = fdp->ConsumeIntegralInRange<int>(0, 100);
    state.view_port.x = fdp->ConsumeIntegralInRange<int>(0, 100);
    state.view_port.y = fdp->ConsumeIntegralInRange<int>(0, 100);
    state.edge_height = fdp->ConsumeIntegralInRange<int>(0, 100);
    state.enabled = fdp->ConsumeBool();
    state.alpha = fdp->ConsumeIntegralInRange<int>(0, 100);
    if (type == NWebTouchHandleState::TouchHandleType::SELECTION_BEGIN_HANDLE) {
        state.orientation = CEF_HORIZONTAL_ALIGNMENT_LEFT;
    } else if (type ==
                NWebTouchHandleState::TouchHandleType::SELECTION_END_HANDLE) {
        state.orientation = CEF_HORIZONTAL_ALIGNMENT_RIGHT;
    } else {
        state.orientation = CEF_HORIZONTAL_ALIGNMENT_CENTER;
    }
    return std::make_shared<NWebTouchHandleStateImpl>(state);
}

class MyRunQuickMenuCallback : public CefRunQuickMenuCallback {
public:
    void Continue(int command_id, cef_event_flags_t event_flags) override {}

    void Cancel() override {}

    IMPLEMENT_REFCOUNTING(MyRunQuickMenuCallback);
};

void NWebQuickMenuParamsImplFuzzTest(FuzzedDataProvider* fdp) {
    int32_t x = fdp->ConsumeIntegralInRange<int32_t>(0, 256);
    int32_t y = fdp->ConsumeIntegralInRange<int32_t>(0, 256);
    int32_t width = fdp->ConsumeIntegralInRange<int32_t>(0, 256);
    int32_t height = fdp->ConsumeIntegralInRange<int32_t>(0, 256);
    int32_t flags = fdp->ConsumeIntegralInRange<int32_t>(0, 256);
    int32_t select_x = fdp->ConsumeIntegralInRange<int32_t>(0, 256);
    int32_t select_y = fdp->ConsumeIntegralInRange<int32_t>(0, 256);
    int32_t select_width = fdp->ConsumeIntegralInRange<int32_t>(0, 256);
    int32_t select_height = fdp->ConsumeIntegralInRange<int32_t>(0, 256);
    NWebQuickMenuParamsImpl impl(x, y, width, height, flags, select_x, select_y, select_width, select_height);

    impl.GetXCoord();
	impl.GetYCoord();
	impl.GetWidth();
	impl.GetHeight();
	impl.GetEditStateFlags();
	impl.GetSelectX();
	impl.GetSelectY();
	impl.GetSelectWidth();
	impl.GetSelectXHeight();

    int type = fdp->ConsumeIntegralInRange<int>(0, 3);
    std::shared_ptr<NWebTouchHandleState> state = GetTouchHandleState(fdp, static_cast<NWebTouchHandleState::TouchHandleType>(type));
    impl.SetTouchHandleState(state, static_cast<NWebTouchHandleState::TouchHandleType>(type));
    impl.GetTouchHandleState(static_cast<NWebTouchHandleState::TouchHandleType>(type));

    bool is_mouse_trigger = fdp->ConsumeBool();
    impl.SetIsMouseTrigger(is_mouse_trigger);
    bool is_long_press_actived = fdp->ConsumeBool();
    impl.SetIsLongPressActived(is_long_press_actived);
    impl.GetIsLongPressActived();

    CefRefPtr<CefRunQuickMenuCallback> callback = new MyRunQuickMenuCallback();
    NWebQuickMenuCallbackImpl backImpl(callback);
    int32_t commandId = fdp->ConsumeIntegralInRange<int32_t>(0, 256);
    int flagValue = fdp->ConsumeIntegralInRange<int>(0, 7);
    MenuEventFlags flag;
    if (flagValue == 0) {
        flag = OHOS::NWeb::EF_NONE;
    } else {
        flag = static_cast<OHOS::NWeb::MenuEventFlags>(1<<(flagValue - 1));
    }
    backImpl.Continue(commandId, flag);

    backImpl.Cancel();
}

class MyContextMenuParams : public CefContextMenuParamsExt {
public:
    void AddRef() const override {}

    bool Release() const override {}

    bool HasOneRef() const override {
        return true;
    }

    bool HasAtLeastOneRef() const override {
        return true;
    }

    int GetXCoord() override {
        return 0;
    }

    int GetYCoord() override {
        return 0;
    }

	TypeFlags GetTypeFlags() override {
        return CM_TYPEFLAG_SELECTION;
    }

	CefString GetLinkUrl() override {
        return "GetLinkUrl";
    }

	CefString GetUnfilteredLinkUrl() override {
        return "GetUnfilteredLinkUrl";
    }

	CefString GetSourceUrl() override {
        return "GetSourceUrl";
    }

	bool HasImageContents() override {
        return true;
    }

	CefString GetTitleText() override {
        return "GetTitleText";
    }

	CefString GetPageUrl() override {
        return "GetPageUrl";
    }

	CefString GetFrameUrl() override {
        return "GetFrameUrl";
    }

	CefString GetFrameCharset() override {
        return "GetFrameCharset";
    }

	MediaType GetMediaType() override {
        return CM_MEDIATYPE_FILE;
    }

	MediaStateFlags GetMediaStateFlags() override {
        return CM_MEDIAFLAG_CAN_PICTURE_IN_PICTURE;
    }

	CefString GetSelectionText() override {
        return "GetSelectionText";
    }

	CefString GetMisspelledWord() override {
        return "GetMisspelledWord";
    }

	bool GetDictionarySuggestions(std::vector<CefString>& suggestions) override {
        return true;
    }

	bool IsEditable() override {
        return true;
    }

	bool IsSpellCheckEnabled() override {
        return true;
    }

	EditStateFlags GetEditStateFlags() override {
        return CM_EDITFLAG_CAN_SELECT_ALL;
    }

	bool IsCustomMenu() override {
        return true;
    }

	InputFieldType GetInputFieldType() override {
        return CM_INPUTFIELDTYPE_OTHER;
    }

    SourceType GetSourceType() override {
        return CM_SOURCETYPE_ADJUST_SELECTION;
    }

#if BUILDFLAG(ARKWEB_DRAG_DROP)
  void GetImageRect(int& x, int& y, int& w, int& h) override {}

  bool IsAILink() override { return false; }
#endif
};

class MyContextMenuCallbackImpl : public CefRunContextMenuCallback {
public:
    void Continue(int command_id, cef_event_flags_t event_flags) override {}

    void Cancel() override {}

    IMPLEMENT_REFCOUNTING(MyContextMenuCallbackImpl);
};

void NWebContextMenuParamsImplFuzzTest(FuzzedDataProvider* fdp) {
    CefRefPtr<CefContextMenuParams> params = new MyContextMenuParams();
    float virutal_device_ratio = fdp->ConsumeFloatingPoint<float>();
    int32_t view_port_height = fdp->ConsumeIntegralInRange<int>(0, 100);
    NWebContextMenuParamsImpl impl(params, virutal_device_ratio, view_port_height);

    impl.GetXCoord();
	impl.GetYCoord();
	impl.GetContextMenuTypeFlags();
    impl.GetLinkUrl();
    impl.GetUnfilteredLinkUrl();
    impl.GetSourceUrl();
    impl.HasImageContents();
    impl.GetTitleText();
    impl.GetPageUrl();
    impl.GetMediaType();
    impl.IsEditable();
	impl.GetEditStateFlags();
    impl.GetInputFieldType();
    impl.GetSourceType();
    impl.GetSelectionText();
#if BUILDFLAG(ARKWEB_DRAG_DROP)
    int x = fdp->ConsumeIntegralInRange<int>(0, 100);
    int y = fdp->ConsumeIntegralInRange<int>(0, 100);
    int w = fdp->ConsumeIntegralInRange<int>(0, 100);
    int h = fdp->ConsumeIntegralInRange<int>(0, 100);
    impl.GetImageRect(x, y, w, h);
    impl.IsAILink();
#endif
    CefRefPtr<CefRunContextMenuCallback> callback = new MyContextMenuCallbackImpl();
    NWebContextMenuCallbackImpl backImpl(callback);
    int32_t commandId = fdp->ConsumeIntegralInRange<int32_t>(0, 256);
    int flagValue = fdp->ConsumeIntegralInRange<int>(0, 7);
    MenuEventFlags flag;
    if (flagValue == 0) {
        flag = OHOS::NWeb::EF_NONE;
    } else {
        flag = static_cast<OHOS::NWeb::MenuEventFlags>(1<<(flagValue - 1));
    }
    backImpl.Continue(commandId, flag);

    backImpl.Cancel();
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
   if (data == nullptr || size == 0) {
    return 0;
  }

  FuzzedDataProvider fdp(data, size);

  NWebQuickMenuParamsImplFuzzTest(&fdp);

  NWebContextMenuParamsImplFuzzTest(&fdp);
  
  return 0;
}