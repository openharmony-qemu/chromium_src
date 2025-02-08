// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "services/network/prp_preload/src/page_res_parallel_preload_mgr_impl.h"

#include "base/ohos/sys_info_utils.h"
#include "ohos_adapter_helper.h"

#define SAFE_RUN_GET_ISOLATION_CB(callback, origin) \
  if (!callback.is_null()) { \
	std::move(callback).Run(origin); \
  }

namespace {
static ohos_prp_preload::PRParallelPreloadMgrImpl g_prp_preload_mgr_impl;
const size_t MAX_PAGE_COUNT = 100;
const std::string CANCEL_ORIGIN = "origin.DEFAULT.Cancel";
const std::string PRP_PRELOAD_MODE_PRECONNECT = "preconnect";
const std::string PRP_PRELOAD_MODE_PRELOAD = "preload";
} // namespace

namespace ohos_prp_preload {
PRParallelPreloadMgr& PRParallelPreloadMgr::GetInstance() {
  return g_prp_preload_mgr_impl;
}

PRPPreloadMode GetPRParallelPreloadModeInner() {
if defined(WEBVIEW_ARM64)
  std::string prp_preload_mode =
	OHOS::NWeb::OhosAdapterHelper::GetInstance().GetSystemPropertiesInstance().GetPRPPreloadMode();
  bool isMobile = base::ohos::IsMobileDevice();
  if (!isMobile) {
	LOG(DEBUG) << "PRPPreload.GetPRParallelPreloadModeInner is not Mobile, NONE";
	return PRPPreloadMode::NONE;
  }

  if (prp_preload_mode == PRP_PRELOAD_MODE_PRECONNECT) {
	LOG(DEBUG) << "PRPPreload.GetPRParallelPreloadModeInner PRECONNECT";
	return PRPPreloadMode::PRECONNECT;
  }

  if (prp_preload_mode == PRP_PRELOAD_MODE_PRELOAD) {
	LOG(DEBUG) << "PRPPreload.GetPRParallelPreloadModeInner PRELOAD";
	return PRPPreloadMode::PRELOAD;
  }

  LOG(DEBUG) << "PRPPreload.GetPRParallelPreloadModeInner NONE";
  return PRPPreloadMode::NONE;
#else
  LOG(DEBUG) << "PRPPreload.GetPRParallelPreloadModeInner only support arm64, NONE";
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
	disk_cache_backend_factory_ = base::WrapRefCounted(new (std::nothrow) DiskCacheBackendFactory());
	if (disk_cache_backend_factory_ == nullptr) {
	  is_inited_ = false;
	  LOG(WARNING) << "PRPPreload.PRParallelPreloadMgrImpl::Init failed no mem";
	  return;
	}
	disk_cache_backend_factory_->CreateBackend();

	sth_task_runner_ = base::ThreadPool::CreateSingleThreadTaskRunner(
	  {base::TaskPriority::USER_VISIBLE}, base::SingleThreadTaskRunnerThreadMode::DEDICATED);
	if (sth_task_runner_ == nullptr || net_task_runner == nullptr) {
	  is_inited_ = false;
	  LOG(WARNING) << "PRPPreload.PRParallelPreloadMgrImpl::Init failed";
	  return;
	}
	net_task_runner_ = net_task_runner;
  }
}

void PRParallelPreloadMgrImpl::StartPage(const std::string& url,
    base::WeakPtr<net::URLRequestContext> url_request_context,
	uint64_t addr_web_handle, PageOriginCallback callback) {
  void* web_handle = reinterpret_cast<void*>(addr_web_handle);
  LOG(DEBUG) << "PRPPreload.PRParallelPreloadMgrImpl::StartPage, is_inited = " << is_inited_ <<
	", current page num = " << prp_preload_info_map_.size() << ", web_handle_valid = " << !web_handle;
  if (url.empty() || web_handle == nullptr ||
      !is_inited_ || prp_preload_info_map_.size() >= MAX_PAGE_COUNT) {
	LOG(DEBUG) << "PRPPreload.PRParallelPreloadMgrImpl::StartPage failed";
	SAFE_RUN_GET_ISOLATION_CB(callback, CANCEL_ORIGIN);
	return;
  }
  std::string main_url = url;
  auto it = prp_preload_info_map_.find(main_url);
  if (it != prp_preload_info_map_.end()) {
	LOG(DEBUG) << "PRPPreload.PRParallelPreloadMgrImpl::StartPage already";
	SAFE_RUN_GET_ISOLATION_CB(it->second.callback_, CANCEL_ORIGIN);
	it->second.callback_ = std::move(callback);
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
	  auto it_info = prp_preload_info_map_.find(main_url);
	  if (it_info != prp_preload_info_map_.end()) {
		LOG(DEBUG) << "PRPPreload.PRParallelPreloadMgrImpl::StartPage org already";
		SAFE_RUN_GET_ISOLATION_CB(it_info->second.callback_, CANCEL_ORIGIN);
		it_info->second.callback_ = std::move(callback);
		return;
	  }
	}
  }

  scoped_refptr<ResParallelPreloadCtrler> rp_preload_ctrler = base::WrapRefCounted(
	new (std::nothrow) ResParallelPreloadCtrler(main_url, sth_task_runner_,
	base::BindRepeating(&PRParallelPreloadMgrImpl::OnRPPCtrlerTimeout, base::Unretained(this))));
  if (rp_preload_ctrler == nullptr ||
      !rp_preload_ctrler->Init(disk_cache_backend_factory_, net_task_runner_, url_request_context,
	  base::BindRepeating(&PRParallelPreloadMgrImpl::OnPageOrigin, base::Unretained(this)))) {
	LOG(DEBUG) << "PRPPreload.PRParallelPreloadMgrImpl::StartPage new ResParallelPreloadCtrler failed";
	SAFE_RUN_GET_ISOLATION_CB(callback, CANCEL_ORIGIN);
	return;
  }

  web_handle_pages_map_[web_handle] = main_url;

  rp_preload_ctrler->Start();
  prp_preload_info_map_[main_url].rp_preload_ctrler_ = rp_preload_ctrler;
  prp_preload_info_map_[main_url].start_page_ = true;
  if (PRParallelPreloadMgr::GetInstance().GetPRParallelPreloadMode() == PRPPreloadMode::PRELOAD) {
	prp_preload_info_map_[main_url].callback_ = std::move(callback);
	prp_preload_info_map_[main_url].prpp_req_loader_fac_ =
	  PRPPRequestLoaderFactory::CreatePRPPRequestLoaderFactory(main_url, url_request_context);
  } else {
	SAFE_RUN_GET_ISOLATION_CB(callback, CANCEL_ORIGIN);
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
		prp_preload_info.rp_preload_ctrler_->SetPRPPReqLoaderFac(prp_preload_info.prpp_req_loader_fac_->GetWeak());
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
	  SAFE_RUN_GET_ISOLATION_CB(it->second.callback_, CANCEL_ORIGIN);
	  auto ctrler = it->second.rp_preload_ctrler_;
	  it->second.prpp_req_loader_fac_ = nullptr;
	  DoRmPageUrl(it->first);
	  (void)prp_preload_info_map_.erase(it);
    if (sth_task_runner_ != nullptr) {
	  sth_task_runner_->PostTask(FROM_HERE, base::BindOnce([]
		(const scoped_refptr<ResParallelPreloadCtrler>& ctrler) {}, ctrler));
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
	  SAFE_RUN_GET_ISOLATION_CB(prp_preload_info.callback_, CANCEL_ORIGIN);
	} else {
	  SAFE_RUN_GET_ISOLATION_CB(prp_preload_info.callback_, page_origin);
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
	  prp_preload_info.rp_preload_ctrler_->SetPRPPReqLoaderFac(prp_preload_info.prpp_req_loader_fac_->GetWeak());
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

} // namespace ohos_prp_preload