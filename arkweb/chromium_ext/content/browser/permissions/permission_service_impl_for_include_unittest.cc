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

#include "content/browser/permissions/permission_service_context.h"

#include <memory>

#include "base/memory/ptr_util.h"
#include "base/memory/raw_ptr.h"
#include "base/test/mock_callback.h"
#include "content/browser/permissions/permission_controller_impl.h"
#include "content/browser/permissions/permission_service_impl.h"
#include "content/browser/renderer_host/render_frame_host_impl.h"
#include "content/public/browser/permission_controller_delegate.h"
#include "content/public/browser/weak_document_ptr.h"
#include "content/public/test/browser_task_environment.h"
#include "content/public/test/mock_permission_manager.h"
#include "content/public/test/navigation_simulator.h"
#include "content/public/test/test_browser_context.h"
#include "content/public/test/test_renderer_host.h"
#include "content/public/test/web_contents_tester.h"
#include "content/test/test_render_frame_host.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/public/common/permissions/permission_utils.h"
#include "third_party/blink/public/mojom/permissions/permission.mojom.h"
#include "url/origin.h"

#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#include "base/command_line.h"

using blink::PermissionType;

namespace content {

namespace {
using ::testing::Unused;
using OverrideStatus = PermissionControllerImpl::OverrideStatus;
using RequestsCallback =
    base::OnceCallback<void(const std::vector<PermissionStatus>&)>;
using PermissionStatusCallback =
    base::OnceCallback<void(PermissionStatus)>;

constexpr char kTestUrl[] = "https://www.example.com";

blink::mojom::PermissionDescriptorPtr
CreateClipboardPermissionDescriptor(blink::PermissionType permission_type) {
    auto descriptor = blink::mojom::PermissionDescriptor::New();
    descriptor->name = blink::mojom::PermissionName::CLIPBOARD_WRITE;
    bool has_user_gesture = (permission_type == blink::PermissionType::CLIPBOARD_SANITIZED_WRITE);
    bool will_be_sanitized = (permission_type == blink::PermissionType::CLIPBOARD_SANITIZED_WRITE);
    auto clipboard_extension = blink::mojom::ClipboardPermissionDescriptor::New(
        has_user_gesture, will_be_sanitized);
    descriptor->extension =
        blink::mojom::PermissionDescriptorExtension::NewClipboard(
            std::move(clipboard_extension));
    return descriptor;
}

class MockManagerWithRequests : public MockPermissionManager {
public:
    MockManagerWithRequests() {}

    MockManagerWithRequests(const MockManagerWithRequests&) = delete;
    MockManagerWithRequests& operator=(const MockManagerWithRequests&) = delete;

    ~MockManagerWithRequests() override {}
    MOCK_METHOD(
        void,
        RequestPermissionsFromCurrentDocument,
        (RenderFrameHost * render_frame_host,
            const PermissionRequestDescription& request_description,
            const base::OnceCallback<void(const std::vector<PermissionStatus>&)>
                callback),
        (override));
    MOCK_METHOD(
        void,
        RequestPermissions,
        (RenderFrameHost * render_frame_host,
            const PermissionRequestDescription& request_description,
            const base::OnceCallback<void(const std::vector<PermissionStatus>&)>
                callback),
        (override));
    MOCK_METHOD(bool,
        IsPermissionOverridable,
        (PermissionType, const std::optional<url::Origin>&),
        (override));
#if BUILDFLAG(ARKWEB_CLIPBOARD)
    MOCK_METHOD(void,
        GetPermissionStatusAsync,
        (PermissionType permission, const GURL& requesting_origin,
            base::OnceCallback<void(PermissionStatus)> callback),
        (override));
    MOCK_METHOD(bool, IsClipboardSitePermissionEnabled, (), (override));
#endif // BUILDFLAG(ARKWEB_CLIPBOARD)
};
} // namespace

class PermissionServiceImplTest : public RenderViewHostTestHarness {
public:
    PermissionServiceImplTest() = default;
    PermissionServiceImplTest(const PermissionServiceImplTest&) = delete;
    PermissionServiceImplTest& operator=(const PermissionServiceImplTest&) =
        delete;
    ~PermissionServiceImplTest() override = default;

    void SetUp() override {
        RenderViewHostTestHarness::SetUp();
        origin_ = url::Origin::Create(GURL(kTestUrl));
        NavigateAndCommit(origin_.GetURL());
        permission_controller_ =
            PermissionControllerImpl::FromBrowserContext(browser_context());
        auto* render_frame_host = main_rfh();
        render_frame_host_impl_ =
            static_cast<RenderFrameHostImpl*>(render_frame_host);
        permission_service_context_ =
            PermissionServiceContext::GetOrCreateForCurrentDocument(
                render_frame_host);
        reinterpret_cast<TestBrowserContext*>(browser_context())->SetPermissionControllerDelegate(
            std::make_unique<::testing::NiceMock<MockManagerWithRequests>>());

        permission_service_ = std::make_unique<PermissionServiceImpl>(
            permission_service_context(), origin_);
    }

    void TearDown() override {
        permission_service_.reset();
        reinterpret_cast<TestBrowserContext*>(browser_context())->SetPermissionControllerDelegate(nullptr);
        permission_controller_ = nullptr;
        render_frame_host_impl_ = nullptr;
        permission_service_context_ = nullptr;
        RenderViewHostTestHarness::TearDown();
    }

    PermissionControllerImpl* permission_controller() {
        return permission_controller_;
    }

    PermissionServiceContext* permission_service_context() {
        return permission_service_context_;
    }

    RenderFrameHostImpl* render_frame_host() { return render_frame_host_impl_; }

    PermissionServiceImpl* permission_service() {
        return permission_service_.get();
    }

    MockManagerWithRequests* mock_manager() {
        return static_cast<MockManagerWithRequests*>(
            browser_context()->GetPermissionControllerDelegate());
    }

    url::Origin origin() {
        return origin_;
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
#endif
    }

    void PermissionServiceHasPermissionAsync(
        blink::mojom::PermissionDescriptorPtr permission,
        PermissionStatusCallback callback) {
#if BUILDFLAG(ARKWEB_CLIPBOARD)
        permission_service()->HasPermissionAsync(
            std::move(permission), std::move(callback));
#endif
    }
    void PermissionServiceRequestPermissionSync(
        blink::mojom::PermissionDescriptorPtr permission,
        bool user_gesture,
        PermissionStatusCallback callback) {
#if BUILDFLAG(ARKWEB_CLIPBOARD)
        permission_service()->RequestPermissionSync(
            std::move(permission), user_gesture, std::move(callback));
#endif
    }
private:
    url::Origin origin_;
    raw_ptr<PermissionControllerImpl> permission_controller_;
    raw_ptr<RenderFrameHostImpl> render_frame_host_impl_;
    raw_ptr<PermissionServiceContext> permission_service_context_;
    std::unique_ptr<PermissionServiceImpl> permission_service_;
};

TEST_F(PermissionServiceImplTest, HasPermissionAsync_001) {
#if BUILDFLAG(ARKWEB_CLIPBOARD) && BUILDFLAG(ARKWEB_NWEB_EX)
    SetNWebExCommandLineSwitch(true);
    EXPECT_CALL(*mock_manager(), IsClipboardSitePermissionEnabled())
        .WillOnce(testing::Return(true));

    auto forward_callbacks = testing::WithArg<2>(
        [](base::OnceCallback<void(blink::mojom::PermissionStatus)> callback) {
            std::move(callback).Run(blink::mojom::PermissionStatus::DENIED);
            return 0;
        });
    EXPECT_CALL(*mock_manager(),
                GetPermissionStatusAsync(
                    PermissionType::CLIPBOARD_READ_WRITE, origin().GetURL(),
                    testing::_))
                .WillOnce(testing::Invoke(forward_callbacks));

    base::MockCallback<PermissionStatusCallback> callback;
    PermissionServiceHasPermissionAsync(
        CreateClipboardPermissionDescriptor(PermissionType::CLIPBOARD_READ_WRITE),
        callback.Get());
#endif // BUILDFLAG(ARKWEB_CLIPBOARD) && BUILDFLAG(ARKWEB_NWEB_EX)
}

TEST_F(PermissionServiceImplTest, HasPermissionAsync_002) {
#if BUILDFLAG(ARKWEB_CLIPBOARD) && BUILDFLAG(ARKWEB_NWEB_EX)
    SetNWebExCommandLineSwitch(true);
    EXPECT_CALL(*mock_manager(), IsClipboardSitePermissionEnabled())
        .WillOnce(testing::Return(false));

    base::MockCallback<PermissionStatusCallback> callback;
    PermissionServiceHasPermissionAsync(
        CreateClipboardPermissionDescriptor(PermissionType::CLIPBOARD_SANITIZED_WRITE),
        callback.Get());
#endif // BUILDFLAG(ARKWEB_CLIPBOARD) && BUILDFLAG(ARKWEB_NWEB_EX)
}

TEST_F(PermissionServiceImplTest, HasPermissionAsync_003) {
#if BUILDFLAG(ARKWEB_CLIPBOARD) && BUILDFLAG(ARKWEB_NWEB_EX)
    SetNWebExCommandLineSwitch(true);
    base::MockCallback<PermissionStatusCallback> callback;
    PermissionServiceHasPermissionAsync(
        CreateClipboardPermissionDescriptor(PermissionType::GEOLOCATION),
        callback.Get());
#endif // BUILDFLAG(ARKWEB_CLIPBOARD) && BUILDFLAG(ARKWEB_NWEB_EX)
}

TEST_F(PermissionServiceImplTest, HasPermissionAsync_004) {
#if BUILDFLAG(ARKWEB_CLIPBOARD) && BUILDFLAG(ARKWEB_NWEB_EX)
    SetNWebExCommandLineSwitch(false);
    base::MockCallback<PermissionStatusCallback> callback;
    PermissionServiceHasPermissionAsync(
        CreateClipboardPermissionDescriptor(PermissionType::CLIPBOARD_READ_WRITE),
        callback.Get());
#endif // BUILDFLAG(ARKWEB_CLIPBOARD) && BUILDFLAG(ARKWEB_NWEB_EX)
}

TEST_F(PermissionServiceImplTest, RequestPermissionSync) {
#if BUILDFLAG(ARKWEB_CLIPBOARD)
    auto forward_callbacks = testing::WithArg<2>(
        [](base::OnceCallback<void(const std::vector<blink::mojom::PermissionStatus>&)> callback) {
            std::move(callback).Run({ blink::mojom::PermissionStatus::DENIED });
            return 0;
        });
    bool user_gesture = true;
    EXPECT_CALL(*mock_manager(),
                RequestPermissionsFromCurrentDocument(
                    render_frame_host(),
                    PermissionRequestDescription(
                        std::vector<PermissionType>({ PermissionType::CLIPBOARD_READ_WRITE }),
                        user_gesture, origin().GetURL()),
                    testing::_))
    .WillOnce(testing::Invoke(forward_callbacks));

    base::MockCallback<PermissionStatusCallback> callback;
    PermissionServiceRequestPermissionSync(
        CreateClipboardPermissionDescriptor(PermissionType::CLIPBOARD_READ_WRITE),
        user_gesture,
        callback.Get());
#endif // BUILDFLAG(ARKWEB_CLIPBOARD)
}
}  //namespace content