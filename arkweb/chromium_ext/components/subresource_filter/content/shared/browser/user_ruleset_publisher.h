// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on user_ruleset_service.h originally written by
// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_SUBRESOURCE_FILTER_CONTENT_SHARED_BROWSER_USER_RULESET_PUBLISHER_H_
#define COMPONENTS_SUBRESOURCE_FILTER_CONTENT_SHARED_BROWSER_USER_RULESET_PUBLISHER_H_

#include <memory>

#include "base/files/file.h"
#include "base/files/file_path.h"
#include "base/functional/callback.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/ref_counted.h"
#include "base/task/sequenced_task_runner.h"
#include "base/task/single_thread_task_runner.h"
#include "components/subresource_filter/core/browser/user_ruleset_version.h"
#include "components/subresource_filter/core/browser/verified_ruleset_dealer.h"
#include "components/subresource_filter/core/common/ruleset_config.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/render_process_host_creation_observer.h"

namespace subresource_filter {

class UserRulesetService;

// Owned by the underlying UserRulesetService. Its main responsibility is receiving
// new versions of subresource filtering rules from the UserRulesetService, and
// distributing them to renderer processes, where they will be memory-mapped
// as-needed by an UnverifiedRulesetDealer.
//
// Implementers must define `SendRulesetToRenderProcess()` as well as a mojo
// interface and implementation class that lives on the renderer to receive the
// ruleset.
class UserRulesetPublisher : public content::RenderProcessHostCreationObserver {
 public:
  UserRulesetPublisher(const UserRulesetPublisher&) = delete;
  UserRulesetPublisher& operator=(const UserRulesetPublisher&) = delete;

  ~UserRulesetPublisher() override;

  class Factory {
   public:
    virtual std::unique_ptr<UserRulesetPublisher> Create(
        UserRulesetService* ruleset_service,
        scoped_refptr<base::SequencedTaskRunner> blocking_task_runner)
        const = 0;
  };

  // Schedules file open and use it as ruleset file. In the case of success,
  // the new and valid |base::File| is passed to |callback|. In the case of
  // error an invalid |base::File| is passed to |callback|. The previous
  // ruleset file will be used (if any). In either case, the supplied
  // unique_ptr always contains a non-null |base::File|.
  virtual void TryOpenAndSetRulesetFile(
      const base::FilePath& file_path,
      int expected_checksum,
      base::OnceCallback<void(RulesetFilePtr)> callback);

  // Redistributes the new version of the |ruleset| to all existing consumers,
  // and sets up |ruleset| to be distributed to all future consumers.
  virtual void PublishNewRulesetVersion(RulesetFilePtr ruleset_data);

  // Task queue for best effort tasks in the thread the object was created in.
  // Used for tasks triggered on UserRulesetService instantiation so it doesn't
  // interfere with startup.  Runs in the UI thread.
  virtual scoped_refptr<base::SingleThreadTaskRunner> BestEffortTaskRunner();

  virtual VerifiedRulesetDealer::Handle* GetRulesetDealer();

  virtual void SetRulesetPublishedCallbackForTesting(
      base::OnceClosure callback);

  // Forwards calls to the underlying ruleset_service_.
  void IndexAndStoreAndPublishRulesetIfNeeded(
      const UserUnindexedRulesetInfo& unindex_ruleset_info);

 protected:
  // Protected to force instantiation through a `UserRulesetPublisher::Factory`.
  UserRulesetPublisher(
      UserRulesetService* ruleset_service,
      scoped_refptr<base::SequencedTaskRunner> blocking_task_runner,
      const RulesetConfig& ruleset_config);

  virtual void UserSendRulesetToRenderProcess(base::File* file,
                                          content::RenderProcessHost* rph) = 0;

 private:
  // content::RenderProcessHostCreationObserver:
  void OnRenderProcessHostCreated(content::RenderProcessHost* rph) override;

  // The service owns the publisher, and therefore outlives it.
  raw_ptr<UserRulesetService> ruleset_service_ = nullptr;

  RulesetFilePtr ruleset_data_{nullptr, base::OnTaskRunnerDeleter{nullptr}};
  base::OnceClosure ruleset_published_callback_;

  std::unique_ptr<VerifiedRulesetDealer::Handle> ruleset_dealer_;
  scoped_refptr<base::SingleThreadTaskRunner> best_effort_task_runner_;
};

}  // namespace subresource_filter

#endif  // COMPONENTS_SUBRESOURCE_FILTER_CONTENT_SHARED_BROWSER_USER_RULESET_PUBLISHER_H_
