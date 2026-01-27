// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "arkweb/chromium_ext/services/network/prp_preload/src/page_res_parallel_preload_mgr_impl.h"

#include "base/ohos/sys_info_utils_ext.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"

namespace {
static ohos_prp_preload::PRParallelPreloadMgrImpl g_prp_preload_mgr_impl;
const size_t MAX_PAGE_COUNT = 100;
const std::string CANCEL_ORIGIN = "origin.DEFAULT.Cancel";
const std::string PRP_PRELOAD_MODE_PRECONNECT = "preconnect";
const std::string PRP_PRELOAD_MODE_PRELOAD = "preload";
} // namespace

namespace ohos_prp_preload {
static inline void SafeRunGetIsolationCB(PageOriginCallback& callback, const std::string& origin)
{
  if (!callback.is_null()) {
	std::move(callback).Run(origin);
  }
}

PRParallelPreloadMgr& PRParallelPreloadMgr::GetInstance() {
  return g_prp_preload_mgr_impl;
}

PRPPreloadMode GetPRParallelPreloadModeInner() {
#if defined(WEBVIEW_ARM64)
  std::string prp_preload_mode =
	OHOS::NWeb::OhosAdapterHelper::GetInstance().GetSystemPropertiesInstance().GetPRPPreloadMode();
  bool isMobile = base::ohos::IsMobileDevice();
  bool isPc = base::ohos::IsPcDevice();
  if (!isMobile && !isPc) {
	LOG(INFO) << "PRPPreload.GetPRParallelPreloadModeInner is not Mobile or PC, NONE";
	return PRPPreloadMode::NONE;
  }

  if (prp_preload_mode == PRP_PRELOAD_MODE_PRECONNECT) {
	LOG(INFO) << "PRPPreload.GetPRParallelPreloadModeInner PRECONNECT";
	return PRPPreloadMode::PRECONNECT;
  }

  if (prp_preload_mode == PRP_PRELOAD_MODE_PRELOAD) {
	LOG(INFO) << "PRPPreload.GetPRParallelPreloadModeInner PRELOAD";
	return PRPPreloadMode::PRELOAD;
  }

  LOG(INFO) << "PRPPreload.GetPRParallelPreloadModeInner NONE";
  return PRPPreloadMode::NONE;
#else
  LOG(INFO) << "PRPPreload.GetPRParallelPreloadModeInner only support arm64, NONE";
  return PRPPreloadMode::NONE;
#endif
}

PRPPreloadMode PRParallelPreloadMgr::GetPRParallelPreloadMode() {
  static PRPPreloadMode mode = GetPRParallelPreloadModeInner();
  return mode;
}

void PRParallelPreloadMgrImpl::Init(const scoped_refptr<base::SingleThreadTaskRunner>& net_task_runner) {
  if (PRParallelPreloadMgr::GetInstance().GetPRParallelPreloadMode() == PRPPreloadMode::NONE) {
	LOG(DEBUG) << "PRPPreload.PRParallelPreloadMgrImpl::Init NOT ENABLE";
	return;
  }
  LOG(DEBUG) << "PRPPreload.PRParallelPreloadMgrImpl::Init ENABLE";
  if (!is_inited_) {
	is_inited_ = true;
	sth_thread_ = std::make_unique<base::Thread>("preload_thread");
	sth_thread_->Start();
	sth_task_runner_ = sth_thread_->task_runner();
	if (sth_task_runner_ == nullptr || net_task_runner == nullptr) {
	  is_inited_ = false;
	  LOG(WARNING) << "PRPPreload.PRParallelPreloadMgrImpl::Init failed";
	  return;
	}
	net_task_runner_ = net_task_runner;
	sth_task_runner_->PostTask(FROM_HERE, base::BindOnce(&ResParallelPreloadCtrler::InitDiskCacheBackendFactory));
  }
}

void PRParallelPreloadMgrImpl::StartPage(const std::string& url,
	const net::NetworkAnonymizationKey& networkAnonymizationKey,
    base::WeakPtr<net::URLRequestContext> url_request_context,
	uint64_t addr_web_handle, PageOriginCallback callback, bool use_no_cache) {
  void* web_handle = reinterpret_cast<void*>(addr_web_handle);
  LOG(DEBUG) << "PRPPreload.PRParallelPreloadMgrImpl::StartPage, is_inited = " << is_inited_ <<
    ", current page num = " << prp_preload_info_map_.size() << ", web_handle_valid = " << !web_handle <<
	", use_no_cache = " << use_no_cache;
  if (use_no_cache || url.empty() || web_handle == nullptr || !is_inited_ ||
      prp_preload_info_map_.size() >= MAX_PAGE_COUNT) {
	SafeRunGetIsolationCB(callback, CANCEL_ORIGIN);
	return;
  }
  std::string main_url = url;
  if (IsPageAlreadyStarted(main_url, callback)) {
	return;
  }
  auto it_web = web_handle_pages_map_.find(web_handle);
  if (it_web != web_handle_pages_map_.end()) {
	auto iter_page_urls = prp_page_url_.find(main_url);
	if ((iter_page_urls == prp_page_url_.end()) && (it_web->second != main_url)) {
	  StopPageInternal(it_web->second);
	  it_web->second = main_url;
	} else if (iter_page_urls != prp_page_url_.end()) {
	  main_url = iter_page_urls->second;
	  if (IsPageAlreadyStarted(main_url, callback)) {
	    return;
      }
	}
  }
  scoped_refptr<ResParallelPreloadCtrler> rp_preload_ctrler = base::WrapRefCounted(
	new (std::nothrow) ResParallelPreloadCtrler(main_url, networkAnonymizationKey, sth_task_runner_,
	base::BindRepeating(&PRParallelPreloadMgrImpl::OnRPPCtrlerTimeout, weak_factory_.GetWeakPtr())));
  if (rp_preload_ctrler == nullptr) {
	SafeRunGetIsolationCB(callback, CANCEL_ORIGIN);
	return;
  }
  rp_preload_ctrler->Init(net_task_runner_, url_request_context,
	base::BindRepeating(&PRParallelPreloadMgrImpl::OnPageOrigin, weak_factory_.GetWeakPtr()));
  web_handle_pages_map_[web_handle] = main_url;
  rp_preload_ctrler->Start();
  prp_preload_info_map_[main_url].rp_preload_ctrler_ = rp_preload_ctrler;
  prp_preload_info_map_[main_url].start_page_ = true;
  if (PRParallelPreloadMgr::GetInstance().GetPRParallelPreloadMode() == PRPPreloadMode::PRELOAD) {
	prp_preload_info_map_[main_url].callback_ = std::move(callback);
	prp_preload_info_map_[main_url].prpp_req_loader_fac_ =
	  PRPPRequestLoaderFactory::CreatePRPPRequestLoaderFactory(main_url, url_request_context);
  } else {
	SafeRunGetIsolationCB(callback, CANCEL_ORIGIN);
	prp_preload_info_map_[main_url].prpp_req_loader_fac_ = nullptr;
  }
}

void PRParallelPreloadMgrImpl::StopPage(uint64_t addr_web_handle) {
  void* web_handle = reinterpret_cast<void*>(addr_web_handle);
  LOG(DEBUG) << "PRPPreload.PRParallelPreloadMgrImpl::StopPage is_inited = " << is_inited_ <<
	", web_handle_valid = " << !web_handle;
  if (!is_inited_ || !web_handle) {
	return;
  }

  auto it_web = web_handle_pages_map_.find(web_handle);
  if (it_web != web_handle_pages_map_.end()) {
	StopPageInternal(it_web->second);
	(void)web_handle_pages_map_.erase(it_web);
  }
}

void PRParallelPreloadMgrImpl::UpdateResRequestInfo(const std::string& key,
    const std::shared_ptr<PRRequestInfo>& info) {
  if (!is_inited_) {
	return;
  }
  FindCurPreloadInfoAndRun(key, [&](PRParallelPreloadInfo& prp_preload_info) {
	prp_preload_info.rp_preload_ctrler_->UpdateResRequestInfo(info);
  });
}

void PRParallelPreloadMgrImpl::UpdateRedirectUrl(const std::string& org_url, const std::string& redirect_url,
	bool replace)
{
  if (!is_inited_) {
	return;
  }

  const std::string* key = &org_url;
  bool need_rm = false;
  if (replace) {
	if (prp_page_url_.find(org_url) != prp_page_url_.end()) {
	  key = &prp_page_url_[org_url];
	  need_rm = true;
	} else {
	  LOG(WARNING) << "PRPPreload.PRParallelPreloadMgrImpl::UpdateRedirectUrl replace but not find";
	  return;
	}
  }

  auto it = prp_preload_info_map_.find(*key);
  if (it != prp_preload_info_map_.end()) {
	if (!it->second.start_page_) {
	  return;
	} else {
	  prp_page_url_[redirect_url] = *key;
	}
  }
  if (need_rm) {
	(void)prp_page_url_.erase(org_url);
  }
}

void PRParallelPreloadMgrImpl::DoRmPageUrl(const std::string& in_org_url)
{
  if (!is_inited_) {
	return;
  }
  for (auto it = prp_page_url_.begin(); it != prp_page_url_.end();) {
	if (in_org_url == it->second) {
	  (void)prp_page_url_.erase(it++);
	} else {
	  it++;
	}
  }
}

void PRParallelPreloadMgrImpl::SetPageOrigin(const std::string& url, const net::IsolationInfo& isl)
{
  if (!is_inited_ || !isl.frame_origin().has_value()) {
	return;
  }

  FindCurPreloadInfoAndRun(url, [&](PRParallelPreloadInfo& prp_preload_info) {
	if (prp_preload_info.prpp_req_loader_fac_) {
	  if (!prp_preload_info.page_origin_ready_) {
		prp_preload_info.page_origin_ready_ = true;
		prp_preload_info.prpp_req_loader_fac_->SetPRPPIsolation(isl);
		prp_preload_info.rp_preload_ctrler_->SetPRPPReqLoaderFac(prp_preload_info.prpp_req_loader_fac_->GetWeak(),
		  (prp_preload_info.prpp_req_loader_fac_->GetWeak().get() != nullptr));
	  }
	  prp_preload_info.rp_preload_ctrler_->SetPageOrigin(isl.frame_origin().value().GetURL().spec());
	}
  });
}

void PRParallelPreloadMgrImpl::StopPageInternal(const std::string& url) {
  auto it = prp_preload_info_map_.find(url);
  if (it != prp_preload_info_map_.end() && it->second.start_page_) {
	it->second.rp_preload_ctrler_->Stop();
	it->second.start_page_ = false;
	  SafeRunGetIsolationCB(it->second.callback_, CANCEL_ORIGIN);
	  auto ctrler = it->second.rp_preload_ctrler_;
	  it->second.prpp_req_loader_fac_ = nullptr;
	  DoRmPageUrl(it->first);
	  (void)prp_preload_info_map_.erase(it);
    if (sth_task_runner_ != nullptr) {
	  sth_task_runner_->PostTask(FROM_HERE, base::BindOnce([]
		(const scoped_refptr<ResParallelPreloadCtrler>& ctrler) {}, std::move(ctrler)));
    }
  }
}

void PRParallelPreloadMgrImpl::OnRPPCtrlerTimeout(const std::string& url) {
  if (net_task_runner_ == nullptr) {
    return;
  }
  net_task_runner_->PostTask(FROM_HERE,
	base::BindOnce(&PRParallelPreloadMgrImpl::DoRPPCtrlerTimeout, weak_factory_.GetWeakPtr(), std::move(url)));
}

void PRParallelPreloadMgrImpl::DoRPPCtrlerTimeout(const std::string& url)
{
  StopPageInternal(url);
}

void PRParallelPreloadMgrImpl::OnPageOrigin(const std::string& url, const std::string& page_origin)
{
  if (!is_inited_) {
	return;
  }

  FindCurPreloadInfoAndRun(url, [&](PRParallelPreloadInfo& prp_preload_info) {
    if (page_origin.empty()) {
	  SafeRunGetIsolationCB(prp_preload_info.callback_, CANCEL_ORIGIN);
	} else {
	  SafeRunGetIsolationCB(prp_preload_info.callback_, page_origin);
	}
  });
}

void PRParallelPreloadMgrImpl::SetURLLoaderFactoryParam(network::mojom::URLLoaderFactoryParamsPtr params)
{
  if (!is_inited_ || !params) {
	return;
  }

  FindCurPreloadInfoAndRun(params->main_url, [&](PRParallelPreloadInfo& prp_preload_info) {
	if (prp_preload_info.prpp_req_loader_fac_ && !prp_preload_info.page_origin_ready_) {
	  prp_preload_info.page_origin_ready_ = true;
	  prp_preload_info.prpp_req_loader_fac_->SetPRPPIsolation(params->isolation_info);
	  prp_preload_info.rp_preload_ctrler_->SetPRPPReqLoaderFac(prp_preload_info.prpp_req_loader_fac_->GetWeak(),
	    (prp_preload_info.prpp_req_loader_fac_->GetWeak().get() != nullptr));
	}
  });
}

base::WeakPtr<PRPPRequestLoaderFactory> PRParallelPreloadMgrImpl::GetRequestLoaderFactory(const std::string& main_url)
{
  if (!is_inited_) {
	return nullptr;
  }
  base::WeakPtr<PRPPRequestLoaderFactory> factory = nullptr;
	FindCurPreloadInfoAndRun(main_url, [&](PRParallelPreloadInfo& prp_preload_info) {
	  if (prp_preload_info.prpp_req_loader_fac_) {
		LOG(DEBUG) << "PRPPreload.PRParallelPreloadMgrImpl::GetRequestLoaderFactory got loader fac";
		factory = prp_preload_info.prpp_req_loader_fac_->GetWeak();
	  }
	});
  return factory;
}

void PRParallelPreloadMgrImpl::UpdateIdlePrerequestCount(const std::string& key)
{
  if (!is_inited_) {
	return;
  }
  FindCurPreloadInfoAndRun(key, [&](PRParallelPreloadInfo& prp_preload_info) {
	prp_preload_info.rp_preload_ctrler_->UpdateIdlePrerequestCount();
  });
}

void PRParallelPreloadMgrImpl::FindCurPreloadInfoAndRun(const std::string& key,
	std::function<void(PRParallelPreloadInfo&)> func)
{
  auto it = prp_preload_info_map_.find(key);
  bool check_redirect = false;
  do {
	if (it != prp_preload_info_map_.end()) {
	  if (!it->second.start_page_) {
		break;
	  }
	  func(it->second);
	  break;
	}
	if (check_redirect) {
	  break;
	}
	auto iter = prp_page_url_.find(key);
	if (iter == prp_page_url_.end()) {
	  break;
	}
	check_redirect = true;
	it = prp_preload_info_map_.find(iter->second);
  } while (check_redirect);
}

bool PRParallelPreloadMgrImpl::IsPageAlreadyStarted(const std::string& key,
    PageOriginCallback& callback)
{
  auto it = prp_preload_info_map_.find(key);
  if (it != prp_preload_info_map_.end()) {
	LOG(DEBUG) << "PRPPreload.PRParallelPreloadMgrImpl::StartPage already";
	SafeRunGetIsolationCB(it->second.callback_, CANCEL_ORIGIN);
	it->second.callback_ = std::move(callback);
	return true;
  }
  return false;
}

void PRParallelPreloadMgrImpl::RemoveCache(base::Time start_time, base::Time end_time)
{
  if (sth_task_runner_ == nullptr) {
    return;
  }
  sth_task_runner_->PostTask(FROM_HERE, base::BindOnce(&ResParallelPreloadCtrler::RemoveCache, start_time, end_time));
}

} // namespace ohos_prp_preload