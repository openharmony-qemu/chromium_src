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

#include "nweb_data_base_delegate.h"

#include <vector>
#include <gmock/gmock.h>

#include "base/logging.h"
#include "url/gurl.h"
#include "gtest/gtest.h"

using namespace testing;
using namespace OHOS::NWeb;

class CefDataBaseMock : public CefDataBase {
    public:
    CefDataBaseMock() { }
    ~CefDataBaseMock() { }
    CefRefPtr<CefDataBase> GetGlobalCefDataBase() { 
        return nullptr;
    }

    bool ExistHttpAuthCredentials() {
        return true;
    }

    void DeleteHttpAuthCredentials() { }

    void SaveHttpAuthCredentials(const CefString& host, 
                                 const CefString& realm,
                                 const CefString& username,
                                 const char* password) { }
    void GetHttpAuthCredentials(const CefString& host, 
                                const CefString& realm,
                                CefString& username,
                                char* password,
                                uint32_t passwordSize) { }
    bool ExistPermissionByOrigin(const CefString& oirgin, int type) {
        return true;
    }
    bool GetPermissionResultByOrigin(const CefString& oirgin, 
                                     int type, 
                                     bool& result) {
        return true;
    }

    void SetPermissionByOrigin(const CefString& origin, 
                                int type, bool result) { }

    void ClearPermissionByOrigin(const CefString& oirgin, int type) { }

    void ClearAllPermission(int type) { }

    void GetOriginsByPermission(int type, 
                                std::vector<CefString>& origins) { }

    bool HasAtLeastOneRef() const { }

    bool HasOneRef() const { }                                

    bool Release() const { }

    void AddRef() const { }
    CefRefPtr<CefDataBase> GetGlobalIncognitoDataBase() { 
        return nullptr;
    }
};

class NWebDataBaseDelegateMock : public OHOS::NWeb::NWebDataBaseDelegate {
    
    public:
    NWebDataBaseDelegateMock() { }

    void SetDataBaseFlag(bool flag) {
        data_base_flag = flag;
    }
    CefRefPtr<CefDataBase> GetGlobalCefDataBase() {
        CefRefPtr<CefDataBase> data_base_tmp = nullptr;
        if (data_base_flag) {
            data_base_tmp = new CefDataBaseMock();
        }
        return data_base_tmp;
    }
    private:
    bool data_base_flag = false;
};

TEST(NWebDataBaseDelegateTEST, ExistHttpAuthCredentials001) {
    auto nweb_data_base_delegate = std::make_shared<NWebDataBaseDelegateMock>();
    nweb_data_base_delegate->SetDataBaseFlag(false);
    EXPECT_FALSE(nweb_data_base_delegate->ExistHttpAuthCredentials());
}

TEST(NWebDataBaseDelegateTEST, DeleteHttpAuthCredentials) {
    auto nweb_data_base_delegate = std::make_shared<NWebDataBaseDelegateMock>();
    nweb_data_base_delegate->SetDataBaseFlag(false);
    nweb_data_base_delegate->DeleteHttpAuthCredentials();
}

TEST(NWebDataBaseDelegateTEST, SaveHttpAuthCredentials) {
    auto nweb_data_base_delegate = std::make_shared<NWebDataBaseDelegateMock>();
    nweb_data_base_delegate->SetDataBaseFlag(false);
    char password[32];
    const std::string host = "";
    const std::string realm = "";
    std::string username = "";
    nweb_data_base_delegate->SaveHttpAuthCredentials(host, realm, username, password);
}

TEST(NWebDataBaseDelegateTEST, GetHttpAuthCredentials) {
    auto nweb_data_base_delegate = std::make_shared<NWebDataBaseDelegateMock>();
    nweb_data_base_delegate->SetDataBaseFlag(false);
    char password[32];
    const std::string host = "";
    const std::string realm = "";
    std::string username = "";
    nweb_data_base_delegate->GetHttpAuthCredentials(host, realm, username, password, 1);
}

TEST(NWebDataBaseDelegateTEST, ExistPermissionByOrigin) {
    auto nweb_data_base_delegate = std::make_shared<NWebDataBaseDelegateMock>();
    nweb_data_base_delegate->SetDataBaseFlag(false);
    EXPECT_FALSE(nweb_data_base_delegate->ExistPermissionByOrigin("", 1, false));
}

TEST(NWebDataBaseDelegateTEST, GetPermissionResultByOrigin) {
    auto nweb_data_base_delegate = std::make_shared<NWebDataBaseDelegateMock>();
    nweb_data_base_delegate->SetDataBaseFlag(false);
    bool flag = false;
    EXPECT_FALSE(nweb_data_base_delegate->GetPermissionResultByOrigin("", 1, flag, false));
}

TEST(NWebDataBaseDelegateTEST, SetPermissionByOrigin001) {
    auto nweb_data_base_delegate = std::make_shared<NWebDataBaseDelegateMock>();
    nweb_data_base_delegate->SetDataBaseFlag(false);
    EXPECT_EQ(nweb_data_base_delegate->SetPermissionByOrigin("http://origin1", 1, false, false), -1);
}

TEST(NWebDataBaseDelegateTEST, SetPermissionByOrigin002) {
    auto nweb_data_base_delegate = std::make_shared<NWebDataBaseDelegateMock>();
    nweb_data_base_delegate->SetDataBaseFlag(false);
    EXPECT_EQ(nweb_data_base_delegate->SetPermissionByOrigin("", 1, false, false), OHOS::NWeb::NWEB_INVALID_ORIGIN);
}

TEST(NWebDataBaseDelegateTEST, ClearPermissionByOrigin001) {
    auto nweb_data_base_delegate = std::make_shared<NWebDataBaseDelegateMock>();
    nweb_data_base_delegate->SetDataBaseFlag(false);
    EXPECT_EQ(nweb_data_base_delegate->ClearPermissionByOrigin("http://origin1", 1, false), -1);
}

TEST(NWebDataBaseDelegateTEST, ClearPermissionByOrigin002) {
    auto nweb_data_base_delegate = std::make_shared<NWebDataBaseDelegateMock>();
    nweb_data_base_delegate->SetDataBaseFlag(false);
    EXPECT_EQ(nweb_data_base_delegate->ClearPermissionByOrigin("", 1, false), OHOS::NWeb::NWEB_INVALID_ORIGIN);
}

TEST(NWebDataBaseDelegateTEST, ClearAllPermission) {
    auto nweb_data_base_delegate = std::make_shared<NWebDataBaseDelegateMock>();
    nweb_data_base_delegate->SetDataBaseFlag(false);
    nweb_data_base_delegate->ClearAllPermission(1, false);
}

TEST(NWebDataBaseDelegateTEST, GetOriginsByPermission) {
    auto nweb_data_base_delegate = std::make_shared<NWebDataBaseDelegateMock>();
    nweb_data_base_delegate->SetDataBaseFlag(false);
    std::vector<std::string> list;
    EXPECT_EQ(nweb_data_base_delegate->GetOriginsByPermission(1, false), list);
}

TEST(NWebDataBaseDelegateTEST, ExistFormData) {
    auto nweb_data_base_delegate = std::make_shared<NWebDataBaseDelegateMock>();
    nweb_data_base_delegate->SetDataBaseFlag(false);
    EXPECT_FALSE(nweb_data_base_delegate->ExistFormData());
}

TEST(NWebDataBaseDelegateTEST, ClearFormData) {
    auto nweb_data_base_delegate = std::make_shared<NWebDataBaseDelegateMock>();
    nweb_data_base_delegate->SetDataBaseFlag(false);
    nweb_data_base_delegate->ClearFormData();
}
