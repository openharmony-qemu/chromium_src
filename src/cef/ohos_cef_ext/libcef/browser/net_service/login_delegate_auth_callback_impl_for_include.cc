/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
bool IsHttpAuthInfoSaved() override {
  constexpr int32_t MAX_PWD_LENGTH = 256;
  auto dataBase = CefDataBase::GetGlobalDataBase();
  if (dataBase == nullptr) {
    LOG(ERROR) << "IsHttpAuthInfoSaved dataBase is empty";
    return false;
  }
  if (!dataBase->ExistHttpAuthCredentials()) {
    LOG(WARNING) << "IsHttpAuthInfoSaved dataBase is existHttpAuth";
    return false;
  }
  CefString username;
  char password[MAX_PWD_LENGTH + 1] = {0};
  dataBase->GetHttpAuthCredentials(host_, realm_, username, password,
                                   MAX_PWD_LENGTH + 1);
  if (username.empty() || strlen(password) == 0) {
    if (memset_s(password, MAX_PWD_LENGTH + 1, 0, MAX_PWD_LENGTH + 1) != EOK) {
      return false;
    }
    LOG(WARNING) << "IsHttpAuthInfoSaved name or password is empty";
    return false;
  }
  CefString passwordCef(password, strlen(password));
  if (memset_s(password, MAX_PWD_LENGTH + 1, 0, MAX_PWD_LENGTH + 1) != EOK) {
    return false;
  }
  if (!task_runner_->RunsTasksInCurrentSequence()) {
    task_runner_->PostTask(
        FROM_HERE, base::BindOnce(&AuthCallbackImpl::Continue, this, username,
                                  passwordCef));
    passwordCef.MemsetToZero();
    LOG(INFO) << "IsHttpAuthInfoSaved continue";
    return true;
  }
  if (delegate_) {
    delegate_->Continue(username, passwordCef);
    delegate_ = nullptr;
    passwordCef.MemsetToZero();
    LOG(INFO) << "IsHttpAuthInfoSaved login byself";
    return true;
  }
  passwordCef.MemsetToZero();
  LOG(WARNING) << "IsHttpAuthInfoSaved is not found";
  return false;
}
#endif  // BUILDFLAG(ARKWEB_NETWORK_BASE)

private:
#if BUILDFLAG(ARKWEB_NETWORK_SERVICE)
CefString host_;
CefString realm_;
#endif
