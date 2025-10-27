/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2022. All rights reserved.
 */

#include "libcef/browser/password/oh_password_store_factory.h"

#include "base/containers/contains.h"
#include "base/path_service.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/profiles/profile.h"
#include "components/keyed_service/content/browser_context_dependency_manager.h"
#include "components/password_manager/core/browser/password_store/login_database.h"
#include "components/password_manager/core/browser/password_store/password_store.h"
#include "components/password_manager/core/browser/password_store_factory_util.h"
#include "content/public/browser/browser_context.h"
#include "cef/libcef/browser/browser_context.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/logging.h"
#endif

using password_manager::PasswordStore;

#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
constexpr base::FilePath::CharType kNWebMigrateDir[] = FILE_PATH_LITERAL("migrate_bak");
#endif

// static
scoped_refptr<PasswordStore> OhPasswordStoreFactory::GetPasswordStoreForContext(
    content::BrowserContext* context,
    ServiceAccessType access_type) {
  // |profile| gets always redirected to a non-Incognito profile below, so
  // Incognito & IMPLICIT_ACCESS means that incognito browsing session would
  // result in traces in the normal profile without the user knowing it.

  if (access_type == ServiceAccessType::IMPLICIT_ACCESS &&
      context->IsOffTheRecord()) {
    return nullptr;
  }
  return GetInstance()->GetStoreForContext(context, true);
}

// static
OhPasswordStoreFactory* OhPasswordStoreFactory::GetInstance() {
  return base::Singleton<OhPasswordStoreFactory>::get();
}

OhPasswordStoreFactory::OhPasswordStoreFactory() {}

OhPasswordStoreFactory::~OhPasswordStoreFactory() {}

scoped_refptr<PasswordStore> OhPasswordStoreFactory::BuildServiceInstanceFor(
    content::BrowserContext* context) {
  Profile* profile =
      CefBrowserContext::FromBrowserContext(context)->AsProfile();
  if (!profile) {
    return nullptr;
  }

  if (password_store_) {
    return password_store_;
  }

  base::FilePath cache_path;
  base::PathService::Get(base::DIR_CACHE, &cache_path);
#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
  base::FilePath migrate_path = cache_path.Append(FILE_PATH_LITERAL(kNWebMigrateDir));
  if (base::PathExists(migrate_path)) {
    cache_path = migrate_path;
  } else {
    LOG(WARNING) << "[Autofill] create db, migrate dir is not exist.";
  }
#endif

  password_store_ = new password_manager::PasswordStore(
      password_manager::PasswordStoreBackend::Create(cache_path, profile->GetPrefs()));

  password_store_->Init(profile->GetPrefs(), nullptr);

  DCHECK(password_store_);
  return password_store_;
}

scoped_refptr<PasswordStore> OhPasswordStoreFactory::GetStoreForContext(
    void* context,
    bool create) {
  if (!context) {
    return nullptr;
  }

  const auto& it = mapping_.find(context);
  if (it != mapping_.end()) {
    return it->second;
  }

  // Object not found.
  if (!create) {
    return nullptr;  // And we're forbidden from creating one.
  }

  scoped_refptr<PasswordStore> store;
  store =
      BuildServiceInstanceFor(static_cast<content::BrowserContext*>(context));

  Associate(context, store);
  return store;
}

void OhPasswordStoreFactory::Associate(
    void* context,
    const scoped_refptr<PasswordStore>& store) {
  DCHECK(!base::Contains(mapping_, context));
  mapping_.insert(std::make_pair(context, store));
}

content::BrowserContext* OhPasswordStoreFactory::GetBrowserContextToUse(
    content::BrowserContext* context) const {
  return context;
}
