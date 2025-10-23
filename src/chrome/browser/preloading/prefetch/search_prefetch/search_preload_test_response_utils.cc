// Copyright 2023 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "search_preload_test_response_utils.h"

#include <memory>
#include <string>

#include "base/memory/weak_ptr.h"
#include "base/run_loop.h"
#include "base/strings/string_split.h"
#include "content/public/browser/browser_thread.h"
#include "testing/gtest/include/gtest/gtest.h"

SearchPreloadDeferrableResponse::SearchPreloadDeferrableResponse(
    SearchPreloadResponseController* test_harness,
    SearchPreloadTestResponseDeferralType deferral_type,
    net::HttpStatusCode code,
    base::StringPairs headers,
    const std::string& response_body)
    : test_harness_(test_harness),
      service_deferral_type_(deferral_type),
      headers_(headers),
      body_(response_body) {
  set_code(code);
}

SearchPreloadDeferrableResponse::~SearchPreloadDeferrableResponse() = default;

void SearchPreloadDeferrableResponse::SendResponse(
    base::WeakPtr<net::test_server::HttpResponseDelegate> delegate) {
  switch (service_deferral_type_) {
    case SearchPreloadTestResponseDeferralType::kNoDeferral:
      delegate->SendHeadersContentAndFinish(
          code(), net::GetHttpReasonPhrase(code()), headers_, body_);
      break;
    case SearchPreloadTestResponseDeferralType::kDeferHeader:
      test_harness_->AddDelayedResponseTask(
          base::SingleThreadTaskRunner::GetCurrentDefault(),
          base::BindOnce(&net::test_server::HttpResponseDelegate::
                             SendHeadersContentAndFinish,
                         delegate, code(), net::GetHttpReasonPhrase(code()),
                         headers_, body_));
      break;
    case SearchPreloadTestResponseDeferralType::kDeferBody:
      test_harness_->AddDelayedResponseTask(
          base::SingleThreadTaskRunner::GetCurrentDefault(),
          base::BindOnce(
              &net::test_server::HttpResponseDelegate::SendContentsAndFinish,
              delegate, body_));
      delegate->SendResponseHeaders(code(), net::GetHttpReasonPhrase(code()),
                                    headers_);
      break;
    case SearchPreloadTestResponseDeferralType::kDeferHeaderThenBody:
      test_harness_->AddDelayedResponseTask(
          base::SingleThreadTaskRunner::GetCurrentDefault(),
          base::BindOnce(
              &net::test_server::HttpResponseDelegate::SendResponseHeaders,
              delegate, code(), "OK", headers_));
      test_harness_->AddDelayedResponseTask(
          base::SingleThreadTaskRunner::GetCurrentDefault(),
          base::BindOnce(
              &net::test_server::HttpResponseDelegate::SendContentsAndFinish,
              delegate, body_));
      break;
    case SearchPreloadTestResponseDeferralType::kDeferChunkedResponseBody:
      size_t body_size = body_.size();
      ASSERT_GE(body_size, size_t(2));
      // This task will send the first part.
      test_harness_->AddDelayedResponseTask(
          base::SingleThreadTaskRunner::GetCurrentDefault(),
          base::BindOnce(&net::test_server::HttpResponseDelegate::SendContents,
                         delegate, body_.substr(0, body_size / 2),
                         base::DoNothing()));
      // This task will send the remaining part of the content.
      test_harness_->AddDelayedResponseTask(
          base::SingleThreadTaskRunner::GetCurrentDefault(),
          base::BindOnce(
              &net::test_server::HttpResponseDelegate::SendContentsAndFinish,
              delegate, body_.substr(body_size / 2)));
      delegate->SendResponseHeaders(code(), net::GetHttpReasonPhrase(code()),
                                    headers_);

      break;
  }
}

SearchPreloadResponseController::SearchPreloadResponseController() = default;
SearchPreloadResponseController::~SearchPreloadResponseController() = default;

void SearchPreloadResponseController::AddDelayedResponseTask(
    const scoped_refptr<base::SingleThreadTaskRunner>& task_runner,
    base::OnceClosure response_closure) {
  ASSERT_TRUE(task_runner->BelongsToCurrentThread());
  base::OnceClosure monitor_callback;
  {
    base::AutoLock auto_lock(response_queue_lock_);
    delayed_response_tasks_.emplace(task_runner, std::move(response_closure));
    monitor_callback = std::move(monitor_callback_);
  }
  if (monitor_callback) {
    std::move(monitor_callback).Run();
  }
}

void SearchPreloadResponseController::DispatchDelayedResponseTask() {
  ASSERT_TRUE(content::BrowserThread::CurrentlyOn(content::BrowserThread::UI));
  std::optional<DelayedResponseTask> delayed_task;
  {
    base::AutoLock auto_lock(response_queue_lock_);
    if (!delayed_response_tasks_.empty()) {
      delayed_task = std::move(delayed_response_tasks_.front());
      delayed_response_tasks_.pop();
    }
  }
  if (delayed_task) {
    delayed_task->Run();
    return;
  }
  base::RunLoop run_loop;
  {
    base::AutoLock auto_lock(response_queue_lock_);
    monitor_callback_ = run_loop.QuitClosure();
  }
  run_loop.Run();
  DispatchDelayedResponseTask();
}

std::unique_ptr<net::test_server::HttpResponse>
SearchPreloadResponseController::CreateDeferrableResponse(
    net::HttpStatusCode code,
    const base::StringPairs& headers,
    const std::string& response_body) {
  std::unique_ptr<SearchPreloadDeferrableResponse> res =
      std::make_unique<SearchPreloadDeferrableResponse>(
          this, service_deferral_type_, code, headers, response_body);
  return res;
}
