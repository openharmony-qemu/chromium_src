/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "third_party/blink/renderer/core/html/forms/html_text_area_element.h"

#include "base/memory/scoped_refptr.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/renderer/core/dom/text.h"
#include "third_party/blink/renderer/core/page/focus_controller.h"
#include "third_party/blink/renderer/core/testing/core_unit_test_helper.h"
#include "third_party/blink/renderer/core/testing/mock_clipboard_host.h"
#include "third_party/blink/renderer/platform/bindings/exception_state.h"
#include "third_party/blink/renderer/platform/wtf/text/string_builder.h"

#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#include "base/command_line.h"

#include "third_party/blink/public/mojom/permissions/permission.mojom-blink.h"
#include "third_party/blink/renderer/core/frame/cached_permission_status.h"

namespace blink {

using mojom::blink::EmbeddedPermissionRequestDescriptor;
using mojom::blink::EmbeddedPermissionRequestDescriptorPtr;
using mojom::blink::PermissionDescriptor;
using mojom::blink::PermissionDescriptorPtr;
using mojom::blink::PermissionName;
using mojom::blink::PermissionObserver;
using mojom::blink::PermissionService;
using MojoPermissionStatus = mojom::blink::PermissionStatus;

namespace {
class MockPermissionService : public PermissionService {
public:
  explicit MockPermissionService() = default;
  ~MockPermissionService() override = default;

  void BindHandle(mojo::ScopedMessagePipeHandle handle) {
    receivers_.Add(this,
                   mojo::PendingReceiver<PermissionService>(std::move(handle)));
  }

  MojoPermissionStatus permission_ = MojoPermissionStatus::DENIED;

  // mojom::blink::PermissionService implementation
  void HasPermission(PermissionDescriptorPtr permission,
                     HasPermissionCallback callback) override {
    std::move(callback).Run(permission_);
  }

#if BUILDFLAG(ARKWEB_CLIPBOARD)
  void RequestPermissionSync(PermissionDescriptorPtr permission,
                             bool user_gesture,
                             RequestPermissionCallback callback) override {
    std::move(callback).Run(permission_);
  }
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD)

  void RegisterPageEmbeddedPermissionControl(
      Vector<PermissionDescriptorPtr> permissions,
      mojo::PendingRemote<mojom::blink::EmbeddedPermissionControlClient>
          pending_client) override {}
  void RequestPageEmbeddedPermission(
      EmbeddedPermissionRequestDescriptorPtr permissions,
      RequestPageEmbeddedPermissionCallback) override {}
  void RequestPermission(PermissionDescriptorPtr permission,
                         bool user_gesture,
                         RequestPermissionCallback) override {}
  void RequestPermissions(Vector<PermissionDescriptorPtr> permissions,
                          bool user_gesture,
                          RequestPermissionsCallback) override {}
  void RevokePermission(PermissionDescriptorPtr permission,
                        RevokePermissionCallback) override {}
  void AddPermissionObserver(
      PermissionDescriptorPtr permission,
      MojoPermissionStatus last_known_status,
      mojo::PendingRemote<PermissionObserver> observer) override {}
  void AddPageEmbeddedPermissionObserver(
      PermissionDescriptorPtr permission,
      MojoPermissionStatus last_known_status,
      mojo::PendingRemote<PermissionObserver> observer) override {}
  void NotifyEventListener(PermissionDescriptorPtr permission,
                           const String& event_type,
                           bool is_added) override {}

private:
  mojo::ReceiverSet<PermissionService> receivers_;
};
}  // namespace

class DocumentExecCommandUnitTest : public RenderingTest {
public:
  DocumentExecCommandUnitTest() {
    mock_permission_service_ = std::make_unique<MockPermissionService>();
  }

  void SetUp() override {
    RenderingTest::SetUp();
    clipboard_provider_ =
        std::make_unique<PageTestBase::MockClipboardHostProvider>(
            GetFrame().GetBrowserInterfaceBroker());
    GetFrame().GetBrowserInterfaceBroker().SetBinderForTesting(
        PermissionService::Name_,
        base::BindRepeating(&MockPermissionService::BindHandle,
                            base::Unretained(mock_permission_service_.get())));
  }

  void TearDown() override {
    GetFrame().GetBrowserInterfaceBroker().SetBinderForTesting(
        PermissionService::Name_, {});
    clipboard_provider_.reset();
    RenderingTest::TearDown();
  }

  void SetNWebExCommandLineSwitch(bool enabled) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
    if (enabled) {
      base::CommandLine::ForCurrentProcess()->AppendSwitch(
          switches::kEnableNwebEx);
    } else {
      base::CommandLine::ForCurrentProcess()->RemoveSwitch(
          switches::kEnableNwebEx);
    }
#endif  // BUILDFLAG(ARKWEB_NWEB_EX)
  }

protected:
  HTMLTextAreaElement& TestElement() {
    Element* element = GetDocument().getElementById(AtomicString("test"));
    DCHECK(element);
    return To<HTMLTextAreaElement>(*element);
  }

  mojom::blink::ClipboardHost* ClipboardHost() {
    return clipboard_provider_ ? clipboard_provider_->clipboard_host() : nullptr;
  }

  std::unique_ptr<MockPermissionService> mock_permission_service_;
  std::unique_ptr<PageTestBase::MockClipboardHostProvider> clipboard_provider_;
};

TEST_F(DocumentExecCommandUnitTest, CutAndPasteTest) {
#if BUILDFLAG(ARKWEB_CLIPBOARD) && BUILDFLAG(ARKWEB_NWEB_EX)
  SetNWebExCommandLineSwitch(true);

  Document& doc = GetDocument();
  SetBodyContent("<textarea id=test>foo\n</textarea>");
  HTMLTextAreaElement& textarea = TestElement();
  textarea.Focus();

  // Setup for clipboard commands.
  GetFrame().GetSettings()->SetJavaScriptCanAccessClipboard(true);
  GetFrame().GetSettings()->SetClipboardSitePermissionEnabled(false);
  GetFrame().GetSettings()->SetDOMPasteAllowed(true);

  doc.execCommand("selectall", false, "", ASSERT_NO_EXCEPTION);
  ASSERT_TRUE(doc.execCommand("cut", false, "", ASSERT_NO_EXCEPTION));

  // Paste text with the trailing \n. It removes the placeholder break element.
  ClipboardHost()->WriteText("foo\n");
  ASSERT_TRUE(doc.execCommand("paste", false, "", ASSERT_NO_EXCEPTION));
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD) && BUILDFLAG(ARKWEB_NWEB_EX)
}

TEST_F(DocumentExecCommandUnitTest, CopyInNotFocusedTest) {
#if BUILDFLAG(ARKWEB_CLIPBOARD) && BUILDFLAG(ARKWEB_NWEB_EX)
  SetNWebExCommandLineSwitch(true);

  Document& doc = GetDocument();
  SetBodyContent("<textarea id=test>foo\n</textarea>");
  HTMLTextAreaElement& textarea = TestElement();
  textarea.Focus();

  // Setup for clipboard commands.
  GetFrame().GetSettings()->SetJavaScriptCanAccessClipboard(true);
  GetFrame().GetSettings()->SetClipboardSitePermissionEnabled(true);

  GetFocusController().SetActive(false);
  GetFocusController().SetFocused(false);

  doc.execCommand("selectall", false, "", ASSERT_NO_EXCEPTION);
  ASSERT_FALSE(doc.execCommand("copy", false, "", ASSERT_NO_EXCEPTION));
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD) && BUILDFLAG(ARKWEB_NWEB_EX)
}

TEST_F(DocumentExecCommandUnitTest, CopyInFocusedTest) {
#if BUILDFLAG(ARKWEB_CLIPBOARD) && BUILDFLAG(ARKWEB_NWEB_EX)
  SetNWebExCommandLineSwitch(true);

  Document& doc = GetDocument();
  SetBodyContent("<textarea id=test>foo\n</textarea>");
  HTMLTextAreaElement& textarea = TestElement();
  textarea.Focus();
  GetFocusController().SetActive(true);
  GetFocusController().SetFocused(true);

  // Setup for clipboard commands.
  GetFrame().GetSettings()->SetJavaScriptCanAccessClipboard(true);
  GetFrame().GetSettings()->SetClipboardSitePermissionEnabled(true);
  doc.execCommand("selectall", false, "", ASSERT_NO_EXCEPTION);

  mock_permission_service_->permission_ = MojoPermissionStatus::DENIED;
  ASSERT_FALSE(doc.execCommand("copy", false, "", ASSERT_NO_EXCEPTION));

  mock_permission_service_->permission_ = MojoPermissionStatus::GRANTED;
  ASSERT_TRUE(doc.execCommand("copy", false, "", ASSERT_NO_EXCEPTION));
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD) && BUILDFLAG(ARKWEB_NWEB_EX)
}

}  // namespace blink
