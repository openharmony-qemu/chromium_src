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

#include "cef/ohos_cef_ext/libcef/browser/anti_tracking/third_party_cookie_access_policy.h"

#include <stdio.h>

#include <fstream>

#include "base/files/file_util.h"
#include "base/functional/bind.h"
#include "base/task/thread_pool.h"
#include "base/values.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "net/base/registry_controlled_domains/registry_controlled_domain.h"
#include "third_party/blink/public/mojom/loader/resource_load_info.mojom-shared.h"
#include "third_party/jsoncpp/source/include/json/json.h"
#include "third_party/jsoncpp/source/include/json/reader.h"
#include "third_party/jsoncpp/source/include/json/value.h"

namespace ohos_anti_tracking {

namespace {

const std::string TBW_DOMAIN_KEY = "trackingBehaviorWebsite";

base::LazyInstance<ThirdPartyCookieAccessPolicy>::Leaky g_lazy_instance;

std::vector<uint8_t> ToByteArray(const std::string& text) {
  std::vector<uint8_t> result(text.length() + 1, 0);
  result.assign(text.begin(), text.end());
  return result;
}

bool CheckIsInResult(const std::set<std::string>& result,
                     const std::string& host) {
  if (result.find(host) != result.end()) {
    return true;
  }

  std::size_t found = host.find_first_of('.');
  if (found != std::string::npos) {
    return CheckIsInResult(result, host.substr(found + 1));
  }
  return false;
}

std::unique_ptr<autofill::Trie<std::string>> ParsingTBWOnFileThread(
    const base::FilePath& tbw_path) {
  std::unique_ptr<autofill::Trie<std::string>> tbw_data =
      std::make_unique<autofill::Trie<std::string>>();
  Json::Reader reader;
  Json::Value root;
  std::ifstream is;

  if (!base::PathExists(tbw_path)) {
    LOG(INFO) << "tbw file not exists: " << tbw_path.value();
    return tbw_data;
  }

  LOG(INFO) << "parsing tbw on file thread start";
  std::string filename = tbw_path.value();
  is.open(filename, std::ios::binary);
  if (reader.parse(is, root)) {
    if (root.isMember("body")) {
      Json::Value body = root["body"];
      if (!body.isNull() && body.isMember(TBW_DOMAIN_KEY)) {
        if (!body[TBW_DOMAIN_KEY].isNull() && body[TBW_DOMAIN_KEY].isArray()) {
          Json::ArrayIndex domain_size = body[TBW_DOMAIN_KEY].size();
          for (Json::ArrayIndex i = 0; i < domain_size; i++) {
            Json::Value domain = body[TBW_DOMAIN_KEY][i];
            if (!domain.isNull() && domain.isMember("domain")) {
              if (domain["domain"].isString()) {
                std::string domain_string = domain["domain"].asString();
                std::string trimed_domain;
                base::TrimWhitespaceASCII(domain_string, base::TRIM_ALL,
                                          &trimed_domain);
                std::vector<uint8_t> key = ToByteArray(trimed_domain);
                std::reverse(std::begin(key), std::end(key));
                tbw_data->AddDataForKey(key, trimed_domain);
              }
            }
          }
          LOG(INFO) << "parsing tbw on file thread done size: " << domain_size;
        } else {
          LOG(INFO) << "parsing tbw on file thread done, root is null";
        }
      }
    }
  } else {
    LOG(INFO) << "parsing tbw on file thread done, parse error";
  }

  is.close();

  return tbw_data;
}

}  // namespace

ThirdPartyCookieAccessPolicy::ThirdPartyCookieAccessPolicy() = default;

ThirdPartyCookieAccessPolicy::~ThirdPartyCookieAccessPolicy() = default;

ThirdPartyCookieAccessPolicy* ThirdPartyCookieAccessPolicy::GetInstance() {
  return g_lazy_instance.Pointer();
}

bool ThirdPartyCookieAccessPolicy::IsHostInITPBypassingList(
    const std::string& host) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::IO);
  return bypassing_host_list_.find(host) != bypassing_host_list_.end();
}

bool ThirdPartyCookieAccessPolicy::AllowGetCookies(
    const network::ResourceRequest& request,
    const GURL& main_frame_url) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::IO);
  if (request.resource_type ==
      static_cast<int>(blink::mojom::ResourceType::kMainFrame)) {
    return true;
  }

  if (net::registry_controlled_domains::SameDomainOrHost(
          request.url, request.site_for_cookies.RepresentativeUrl(),
          net::registry_controlled_domains::INCLUDE_PRIVATE_REGISTRIES)) {
    return true;
  }

  if (main_frame_url.is_valid() && main_frame_url.has_host() &&
      IsHostInITPBypassingList(main_frame_url.host())) {
    return true;
  }

  return !CheckIsInTBW(request.url.host());
}

void ThirdPartyCookieAccessPolicy::SetTBWFilePath(
    const base::FilePath& tbw_path) {
  tbw_path_ = tbw_path;
  LOG(INFO) << "Set TBWFile path: " << tbw_path.value();

  base::ThreadPool::PostTaskAndReplyWithResult(
      FROM_HERE, {base::MayBlock(), base::TaskPriority::USER_VISIBLE},
      base::BindOnce(&ParsingTBWOnFileThread, tbw_path_),
      base::BindOnce(&ThirdPartyCookieAccessPolicy::OnTBWDataReadDone,
                     base::Unretained(this)));
}

void ThirdPartyCookieAccessPolicy::OnTBWDataReadDone(
    std::unique_ptr<autofill::Trie<std::string>> tbw_data) {
  content::GetIOThreadTaskRunner({})->PostTask(
      FROM_HERE,
      base::BindOnce(&ThirdPartyCookieAccessPolicy::UpdateTBWOnIOThread,
                     base::Unretained(this), std::move(tbw_data)));
}

void ThirdPartyCookieAccessPolicy::UpdateTBWOnIOThread(
    std::unique_ptr<autofill::Trie<std::string>> tbw_data) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::IO);

  if (!tbw_data) {
    return;
  }
  tbw_data_ = std::move(tbw_data);
  LOG(INFO) << "Update tbw on io thread done";
}

bool ThirdPartyCookieAccessPolicy::CheckIsInTBW(const std::string& host) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::IO);

  if (host.empty()) {
    return false;
  }

  if (!tbw_data_ || tbw_data_->empty()) {
    return false;
  }

  std::string registrable_domain =
      net::registry_controlled_domains::GetDomainAndRegistry(
          host, net::registry_controlled_domains::INCLUDE_PRIVATE_REGISTRIES);
  std::vector<uint8_t> key = ToByteArray(registrable_domain);
  std::reverse(std::begin(key), std::end(key));

  std::set<std::string> result;
  tbw_data_->FindDataForKeyPrefix(key, &result);

  return CheckIsInResult(result, host);
}

void ThirdPartyCookieAccessPolicy::AddITPBypassingList(
    const std::vector<std::string>& host_list) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  LOG(INFO) << "AddITPBypassingList size: " << host_list.size();
  content::GetIOThreadTaskRunner({})->PostTask(
      FROM_HERE,
      base::BindOnce(
          &ThirdPartyCookieAccessPolicy::AddITPBypassingListOnIOThread,
          base::Unretained(this), host_list));
}

void ThirdPartyCookieAccessPolicy::AddITPBypassingListOnIOThread(
    const std::vector<std::string>& host_list) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::IO);

  bypassing_host_list_.insert(host_list.begin(), host_list.end());
  LOG(INFO) << "AddITPBypassingListOnIOThread done, host_list size "
            << host_list.size() << ", block host allow list size "
            << bypassing_host_list_.size();
}

void ThirdPartyCookieAccessPolicy::RemoveITPBypassingList(
    const std::vector<std::string>& host_list) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  LOG(INFO) << "RemoveITPBypassingList size: " << host_list.size();
  content::GetIOThreadTaskRunner({})->PostTask(
      FROM_HERE,
      base::BindOnce(
          &ThirdPartyCookieAccessPolicy::RemoveITPBypassingListOnIOThread,
          base::Unretained(this), host_list));
}

void ThirdPartyCookieAccessPolicy::RemoveITPBypassingListOnIOThread(
    const std::vector<std::string>& host_list) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::IO);

  for (auto& host : host_list) {
    if (bypassing_host_list_.find(host) != bypassing_host_list_.end()) {
      bypassing_host_list_.erase(host);
    }
  }
  LOG(INFO) << "RemoveITPBypassingListOnIOThread done, host_list size "
            << host_list.size() << ", block host allow list size "
            << bypassing_host_list_.size();
}

void ThirdPartyCookieAccessPolicy::ClearITPBypassingList() {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  bypassing_host_list_.clear();
  LOG(INFO) << "ClearITPBypassingList";
  content::GetIOThreadTaskRunner({})->PostTask(
      FROM_HERE,
      base::BindOnce(
          &ThirdPartyCookieAccessPolicy::ClearITPBypassingListOnIOThread,
          base::Unretained(this)));
}

void ThirdPartyCookieAccessPolicy::ClearITPBypassingListOnIOThread() {
  DCHECK_CURRENTLY_ON(content::BrowserThread::IO);

  bypassing_host_list_.clear();
  LOG(INFO) << "ClearITPBypassingListOnIOThread done";
}

}  // namespace ohos_anti_tracking
