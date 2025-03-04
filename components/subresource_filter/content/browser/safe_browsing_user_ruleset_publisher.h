// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_SUBRESOURCE_FILTER_CONTENT_BROWSER_SAFE_BROWSING_USER_RULESET_PUBLISHER_H_
#define COMPONENTS_SUBRESOURCE_FILTER_CONTENT_BROWSER_SAFE_BROWSING_USER_RULESET_PUBLISHER_H_

#include <memory>
#include <utility>

#include "base/files/file.h"
#include "base/memory/ref_counted.h"
#include "base/task/sequenced_task_runner.h"
#include "components/subresource_filter/content/shared/browser/user_ruleset_publisher.h"
#include "components/subresource_filter/content/shared/browser/user_ruleset_service.h"

namespace content {
class RenderProcessHost;
}  // namespace content

namespace subresource_filter {

class UserRulesetService;

class SafeBrowsingUserRulesetPublisher : public UserRulesetPublisher {
 public:
  SafeBrowsingUserRulesetPublisher(
      UserRulesetService* ruleset_service,
      scoped_refptr<base::SequencedTaskRunner> blocking_task_runner)
      : UserRulesetPublisher(ruleset_service,
                             std::move(blocking_task_runner),
                             ruleset_service->config()) {}

  SafeBrowsingUserRulesetPublisher(const SafeBrowsingUserRulesetPublisher&) =
      delete;
  SafeBrowsingUserRulesetPublisher& operator=(
      const SafeBrowsingUserRulesetPublisher&) = delete;

  class Factory : public UserRulesetPublisher::Factory {
   public:
    std::unique_ptr<UserRulesetPublisher> Create(
        UserRulesetService* ruleset_service,
        scoped_refptr<base::SequencedTaskRunner> blocking_task_runner)
        const override;
  };

  ~SafeBrowsingUserRulesetPublisher() override = default;

  void UserSendRulesetToRenderProcess(base::File* file,
                                      content::RenderProcessHost* rph) override;
};

}  // namespace subresource_filter

#endif  // COMPONENTS_SUBRESOURCE_FILTER_CONTENT_BROWSER_SAFE_BROWSING_USER_RULESET_PUBLISHER_H_
