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

#include "arkweb/chromium_ext/content/browser/notifications/blink_notification_service_impl_ext.h"

#include <memory>
#include <utility>

#include "base/command_line.h"
#include "base/functional/bind.h"
#include "base/run_loop.h"
#include "content/browser/notifications/platform_notification_context_impl.h"
#include "content/browser/service_worker/embedded_worker_test_helper.h"
#include "content/public/browser/browser_context.h"
#include "content/public/common/content_switches.h"
#include "content/public/test/browser_task_environment.h"
#include "content/public/test/mock_permission_manager.h"
#include "content/public/test/mock_render_process_host.h"
#include "content/public/test/test_browser_context.h"
#include "content/public/test/test_renderer_host.h"
#include "content/public/test/test_utils.h"
#include "content/public/test/web_contents_tester.h"
#include "content/test/mock_platform_notification_service.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/public/common/permissions/permission_utils.h"
#include "third_party/blink/public/common/storage_key/storage_key.h"
#include "third_party/blink/public/mojom/notifications/notification_service.mojom.h"
#include "third_party/blink/public/mojom/permissions/permission_status.mojom.h"

using ::testing::_;
using ::testing::Return;

namespace content {

namespace {

const char kTestOrigin[] = "https://example.com";

class MockManagerWithRequests : public MockPermissionManager {
 public:
  MockManagerWithRequests() {}

  MockManagerWithRequests(const MockManagerWithRequests&) = delete;
  MockManagerWithRequests& operator=(const MockManagerWithRequests&) = delete;

  ~MockManagerWithRequests() override {}

#if BUILDFLAG(ARKWEB_NOTIFICATION)
   MOCK_METHOD3(GetPermissionStatusAsync,
                void(blink::PermissionType permission,
                     const GURL& requesting_origin,
                     base::OnceCallback<void(blink::mojom::PermissionStatus)> callback));
#endif
};
}  // anonymous namespace

class BlinkNotificationServiceImplExtTest : public ::testing::Test {
 public:
  BlinkNotificationServiceImplExtTest()
      : task_environment_(BrowserTaskEnvironment::IO_MAINLOOP),
        embedded_worker_helper_(
            std::make_unique<EmbeddedWorkerTestHelper>(base::FilePath())),
        render_process_host_(&browser_context_) {
    browser_context_.SetPlatformNotificationService(
        std::make_unique<MockPlatformNotificationService>(&browser_context_));
  }

  BlinkNotificationServiceImplExtTest(
      const BlinkNotificationServiceImplExtTest&) = delete;
  BlinkNotificationServiceImplExtTest& operator=(
      const BlinkNotificationServiceImplExtTest&) = delete;

  ~BlinkNotificationServiceImplExtTest() override = default;

  void SetUp() override {
    notification_context_ = new PlatformNotificationContextImpl(
        base::FilePath(), &browser_context_,
        embedded_worker_helper_->context_wrapper());
    notification_context_->Initialize();

    RunAllTasksUntilIdle();

    contents_ = CreateTestWebContents();

    storage_key_ = blink::StorageKey::CreateFirstParty(
        url::Origin::Create(GURL(kTestOrigin)));

    notification_service_ext_ = std::make_unique<BlinkNotificationServiceImplExt>(
        notification_context_.get(), &browser_context_,
        embedded_worker_helper_->context_wrapper(), &render_process_host_,
        storage_key_,
        /*document_url=*/GURL(),
        contents_.get()->GetPrimaryMainFrame()->GetWeakDocumentPtr(),
        RenderProcessHost::NotificationServiceCreatorType::kDocument,
        notification_service_remote_.BindNewPipeAndPassReceiver());

    browser_context_.SetPermissionControllerDelegate(
        std::make_unique<testing::NiceMock<MockManagerWithRequests>>());
  }

  void TearDown() override {
    embedded_worker_helper_.reset();
    base::RunLoop().RunUntilIdle();
  }

  void DidGetPermissionStatus(
      base::OnceClosure quit_closure,
      blink::mojom::PermissionStatus permission_status) {
    permission_callback_result_ = permission_status;
    std::move(quit_closure).Run();
  }

  blink::mojom::PermissionStatus GetPermissionCallbackResult() {
    return permission_callback_result_;
  }

  void SetNWebExCommandLineSwitch(bool enabled) {
    if (enabled) {
      base::CommandLine::ForCurrentProcess()->AppendSwitch(switches::kEnableNwebEx);
    } else {
      base::CommandLine::ForCurrentProcess()->RemoveSwitch(switches::kEnableNwebEx);
    }
  }

  void SetPermissionStatus(blink::mojom::PermissionStatus permission_status) {
    MockManagerWithRequests* mock_permission_manager =
        static_cast<MockManagerWithRequests*>(
            browser_context_.GetPermissionControllerDelegate());

    ON_CALL(*mock_permission_manager,
            GetPermissionStatusForCurrentDocument(
                blink::PermissionType::NOTIFICATIONS, _, _))
        .WillByDefault(Return(permission_status));
    ON_CALL(*mock_permission_manager,
            GetPermissionStatusForWorker(blink::PermissionType::NOTIFICATIONS,
                                         _, _))
        .WillByDefault(Return(permission_status));
#if BUILDFLAG(ARKWEB_NOTIFICATION)
    auto forward_callbacks = testing::WithArg<2>(
        [permission_status = permission_status](
            base::OnceCallback<void(blink::mojom::PermissionStatus)> callback) {
          std::move(callback).Run(permission_status);
          return 0;
        });
    ON_CALL(*mock_permission_manager,
            GetPermissionStatusAsync(blink::PermissionType::NOTIFICATIONS,
                                         _, _))
        .WillByDefault(testing::Invoke(forward_callbacks));
#endif
  }

 protected:
  BrowserTaskEnvironment task_environment_;

  blink::StorageKey storage_key_;

  std::unique_ptr<EmbeddedWorkerTestHelper> embedded_worker_helper_;

  std::unique_ptr<BlinkNotificationServiceImplExt> notification_service_ext_;

  mojo::Remote<blink::mojom::NotificationService> notification_service_remote_;

  TestBrowserContext browser_context_;

  MockRenderProcessHost render_process_host_;

  scoped_refptr<PlatformNotificationContextImpl> notification_context_;

 private:
  std::unique_ptr<content::WebContents> CreateTestWebContents() {
    auto site_instance = content::SiteInstance::Create(&browser_context_);
    return content::WebContentsTester::CreateTestWebContents(
        &browser_context_, std::move(site_instance));
  }

  blink::mojom::PermissionStatus permission_callback_result_ =
      blink::mojom::PermissionStatus::ASK;

  RenderViewHostTestEnabler rvh_enabler_;

  std::unique_ptr<WebContents> contents_;
};

#if BUILDFLAG(ARKWEB_NOTIFICATION)
TEST_F(BlinkNotificationServiceImplExtTest,
       GetPermissionStatusExtWithoutEnableNwebEx001) {
  SetNWebExCommandLineSwitch(false);

  SetPermissionStatus(blink::mojom::PermissionStatus::GRANTED);

  {
    base::RunLoop run_loop;
    notification_service_ext_->GetPermissionStatusExt(base::BindOnce(
        &BlinkNotificationServiceImplExtTest::DidGetPermissionStatus,
        base::Unretained(this), run_loop.QuitClosure()));
    run_loop.Run();
  }

  EXPECT_EQ(blink::mojom::PermissionStatus::GRANTED,
            GetPermissionCallbackResult());
}
#endif  // ARKWEB_NOTIFICATION

#if BUILDFLAG(ARKWEB_NOTIFICATION)
TEST_F(BlinkNotificationServiceImplExtTest,
       GetPermissionStatusExtWithoutEnableNwebEx002) {
  SetNWebExCommandLineSwitch(false);
  SetPermissionStatus(blink::mojom::PermissionStatus::DENIED);

  {
    base::RunLoop run_loop;
    notification_service_ext_->GetPermissionStatusExt(base::BindOnce(
        &BlinkNotificationServiceImplExtTest::DidGetPermissionStatus,
        base::Unretained(this), run_loop.QuitClosure()));
    run_loop.Run();
  }

  EXPECT_EQ(blink::mojom::PermissionStatus::DENIED,
            GetPermissionCallbackResult());
}
#endif  // ARKWEB_NOTIFICATION

#if BUILDFLAG(ARKWEB_NOTIFICATION)
TEST_F(BlinkNotificationServiceImplExtTest,
       GetPermissionStatusExtWithoutEnableNwebEx003) {
  SetNWebExCommandLineSwitch(false);
  SetPermissionStatus(blink::mojom::PermissionStatus::ASK);

  {
    base::RunLoop run_loop;
    notification_service_ext_->GetPermissionStatusExt(base::BindOnce(
        &BlinkNotificationServiceImplExtTest::DidGetPermissionStatus,
        base::Unretained(this), run_loop.QuitClosure()));
    run_loop.Run();
  }

  EXPECT_EQ(blink::mojom::PermissionStatus::ASK, GetPermissionCallbackResult());
}
#endif  // ARKWEB_NOTIFICATION

#if BUILDFLAG(ARKWEB_NOTIFICATION)
TEST_F(BlinkNotificationServiceImplExtTest,
       GetPermissionStatusExtWithEnableNwebEx001) {
  SetNWebExCommandLineSwitch(true);

  SetPermissionStatus(blink::mojom::PermissionStatus::GRANTED);

  {
    base::RunLoop run_loop;
    notification_service_ext_->GetPermissionStatusExt(base::BindOnce(
        &BlinkNotificationServiceImplExtTest::DidGetPermissionStatus,
        base::Unretained(this), run_loop.QuitClosure()));
    run_loop.Run();
  }

  EXPECT_EQ(blink::mojom::PermissionStatus::GRANTED,
            GetPermissionCallbackResult());
}
#endif  // ARKWEB_NOTIFICATION

#if BUILDFLAG(ARKWEB_NOTIFICATION)
TEST_F(BlinkNotificationServiceImplExtTest,
       GetPermissionStatusExtWithEnableNwebEx002) {
  SetNWebExCommandLineSwitch(true);

  SetPermissionStatus(blink::mojom::PermissionStatus::DENIED);

  {
    base::RunLoop run_loop;
    notification_service_ext_->GetPermissionStatusExt(base::BindOnce(
        &BlinkNotificationServiceImplExtTest::DidGetPermissionStatus,
        base::Unretained(this), run_loop.QuitClosure()));
    run_loop.Run();
  }

  EXPECT_EQ(blink::mojom::PermissionStatus::DENIED,
            GetPermissionCallbackResult());
}
#endif  // ARKWEB_NOTIFICATION

#if BUILDFLAG(ARKWEB_NOTIFICATION)
TEST_F(BlinkNotificationServiceImplExtTest,
       GetPermissionStatusExtWithEnableNwebEx003) {
  SetNWebExCommandLineSwitch(true);

  SetPermissionStatus(blink::mojom::PermissionStatus::ASK);

  {
    base::RunLoop run_loop;
    notification_service_ext_->GetPermissionStatusExt(base::BindOnce(
        &BlinkNotificationServiceImplExtTest::DidGetPermissionStatus,
        base::Unretained(this), run_loop.QuitClosure()));
    run_loop.Run();
  }

  EXPECT_EQ(blink::mojom::PermissionStatus::ASK, GetPermissionCallbackResult());
}
#endif  // ARKWEB_NOTIFICATION

#if BUILDFLAG(ARKWEB_NOTIFICATION)
TEST_F(BlinkNotificationServiceImplExtTest, CheckPermissionStatusAsync001) {
  SetPermissionStatus(blink::mojom::PermissionStatus::GRANTED);

  {
    base::RunLoop run_loop;
    notification_service_ext_->CheckPermissionStatusAsync(base::BindOnce(
        &BlinkNotificationServiceImplExtTest::DidGetPermissionStatus,
        base::Unretained(this), run_loop.QuitClosure()));
    run_loop.Run();
  }

  EXPECT_EQ(blink::mojom::PermissionStatus::GRANTED,
            GetPermissionCallbackResult());
}
#endif  // ARKWEB_NOTIFICATION

#if BUILDFLAG(ARKWEB_NOTIFICATION)
TEST_F(BlinkNotificationServiceImplExtTest, CheckPermissionStatusAsync002) {
  SetPermissionStatus(blink::mojom::PermissionStatus::DENIED);

  {
    base::RunLoop run_loop;
    notification_service_ext_->CheckPermissionStatusAsync(base::BindOnce(
        &BlinkNotificationServiceImplExtTest::DidGetPermissionStatus,
        base::Unretained(this), run_loop.QuitClosure()));
    run_loop.Run();
  }

  EXPECT_EQ(blink::mojom::PermissionStatus::DENIED,
            GetPermissionCallbackResult());
}
#endif  // ARKWEB_NOTIFICATION

#if BUILDFLAG(ARKWEB_NOTIFICATION)
TEST_F(BlinkNotificationServiceImplExtTest, CheckPermissionStatusAsync003) {
  SetPermissionStatus(blink::mojom::PermissionStatus::ASK);

  {
    base::RunLoop run_loop;
    notification_service_ext_->CheckPermissionStatusAsync(base::BindOnce(
        &BlinkNotificationServiceImplExtTest::DidGetPermissionStatus,
        base::Unretained(this), run_loop.QuitClosure()));
    run_loop.Run();
  }

  EXPECT_EQ(blink::mojom::PermissionStatus::ASK, GetPermissionCallbackResult());
}
#endif  // ARKWEB_NOTIFICATION

}  // namespace content
