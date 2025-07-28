/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <gmock/gmock.h>

#include "nweb_data_base_delegate.h"
#include "nweb_hilog.h"
#include "gtest/gtest.h"

#define private public
#include "nweb_data_base_impl.h"

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

class NWebDataBaseImplMock : public NWebDataBaseImpl {
public:
    NWebDataBaseImplMock() {
    }

    void SetDelegate(std::shared_ptr<NWebDataBaseDelegate> delegate) {
        delegate_ = delegate;
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

    void ClearAllPermission(int type, bool incognito) { }

    bool ExistPermissionByOrigin(const std::string& origin,
                               int type,
                               bool incognito) { 
        return false;
    }
    private:
    bool data_base_flag = false;
};



TEST(NWebDataBaseImplTEST, ExistHttpAuthCredentials001) {
    auto nweb_data_base_impl = std::make_shared<NWebDataBaseImpl>();
    EXPECT_FALSE(nweb_data_base_impl->ExistHttpAuthCredentials());
}

TEST(NWebDataBaseImplTEST, ExistHttpAuthCredentials002) {
    auto nweb_data_base_impl = std::make_shared<NWebDataBaseImplMock>();
    nweb_data_base_impl->SetDelegate(nullptr);
    EXPECT_FALSE(nweb_data_base_impl->ExistHttpAuthCredentials());
}

TEST(NWebDataBaseImplTEST, DeleteHttpAuthCredentials) {
    auto nweb_data_base_impl = std::make_shared<NWebDataBaseImpl>();
    nweb_data_base_impl->DeleteHttpAuthCredentials();
}

TEST(NWebDataBaseImplTEST, SaveHttpAuthCredentials) {
    auto nweb_data_base_impl = std::make_shared<NWebDataBaseImpl>();
    const std::string host = "";
    const std::string realm = "";
    const std::string username = "";
    char pw[32];
    nweb_data_base_impl->SaveHttpAuthCredentials(host, realm, username, pw);  
}

TEST(NWebDataBaseImplTEST, GetHttpAuthCredentials) {
    auto nweb_data_base_impl = std::make_shared<NWebDataBaseImpl>();
    const std::string host = "";
    const std::string realm = "";
    std::string username = "";
    char pw[32];
    nweb_data_base_impl->GetHttpAuthCredentials(host, realm, username, pw, 1);  
}

TEST(NWebDataBaseImplTEST, ExistPermissionByOrigin001) {
    auto nweb_data_base_impl = std::make_shared<NWebDataBaseImplMock>();
    const std::string origin = "";
    EXPECT_FALSE(nweb_data_base_impl->
        ExistPermissionByOrigin(origin, 1, false));
}

TEST(NWebDataBaseImplTEST, ExistPermissionByOrigin002) {
    auto nweb_data_base_impl = std::make_shared<NWebDataBaseImplMock>();
    nweb_data_base_impl->SetDelegate(nullptr);
    const std::string origin = "";
    EXPECT_FALSE(nweb_data_base_impl->
        ExistPermissionByOrigin(origin, 1, false));
}

TEST(NWebDataBaseImplTEST, GetPermissionResultByOrigin001) {
    auto nweb_data_base_impl = std::make_shared<NWebDataBaseImplMock>();
    const std::string origin = "";
    auto delegate_mock = std::make_shared<NWebDataBaseDelegateMock>();
    nweb_data_base_impl->SetDelegate(delegate_mock);
    bool result = false;
    EXPECT_FALSE(nweb_data_base_impl->
        GetPermissionResultByOrigin(origin, 1, result, false));
}

TEST(NWebDataBaseImplTEST, GetPermissionResultByOrigin002) {
    auto nweb_data_base_impl = std::make_shared<NWebDataBaseImplMock>();
    nweb_data_base_impl->SetDelegate(nullptr);
    const std::string origin = "";
    bool result = false;
    EXPECT_FALSE(nweb_data_base_impl->
        GetPermissionResultByOrigin(origin, 1, result, false));
}

TEST(NWebDataBaseImplTEST, SetPermissionByOrigin001) {
    auto nweb_data_base_impl = std::make_shared<NWebDataBaseImplMock>();
    auto delegate_mock = std::make_shared<NWebDataBaseDelegateMock>();
    nweb_data_base_impl->SetDelegate(delegate_mock);
    const std::string origin = "";
    bool result = false;
    EXPECT_EQ(nweb_data_base_impl->
        SetPermissionByOrigin(origin, 1, result, true), 17100011);
}

TEST(NWebDataBaseImplTEST, SetPermissionByOrigin002) {
    auto nweb_data_base_impl = std::make_shared<NWebDataBaseImplMock>();
    nweb_data_base_impl->SetDelegate(nullptr);
    const std::string origin = "";
    bool result = false;
    EXPECT_EQ(nweb_data_base_impl->
        SetPermissionByOrigin(origin, 1, result, true), -1);
}

TEST(NWebDataBaseImplTEST, ClearPermissionByOrigin001) {
    auto nweb_data_base_impl = std::make_shared<NWebDataBaseImpl>();
    const std::string origin = "";
    bool result = false;
    EXPECT_EQ(nweb_data_base_impl->
        ClearPermissionByOrigin(origin, 1, true), 17100011);
}

TEST(NWebDataBaseImplTEST, ClearPermissionByOrigin002) {
    auto nweb_data_base_impl = std::make_shared<NWebDataBaseImplMock>();
    nweb_data_base_impl->SetDelegate(nullptr);
    const std::string origin = "";
    bool result = false;
    EXPECT_EQ(nweb_data_base_impl->
        ClearPermissionByOrigin(origin, 1, true), -1);
}

TEST(NWebDataBaseImplTEST, ClearAllPermission) {
    auto nweb_data_base_impl = std::make_shared<NWebDataBaseImplMock>();
    auto delegate_mock = std::make_shared<NWebDataBaseDelegateMock>();
    nweb_data_base_impl->SetDelegate(delegate_mock);
    const std::string origin = "";
    bool result = false;
    nweb_data_base_impl->ClearAllPermission(1, false);
}

TEST(NWebDataBaseImplTEST, GetOriginsByPermission001) {
    auto nweb_data_base_impl = std::make_shared<NWebDataBaseImpl>();
    const std::string origin = "";
    bool result = false;
    std::vector<std::string> origins;
    EXPECT_EQ(nweb_data_base_impl->GetOriginsByPermission(1, false), origins);
}

TEST(NWebDataBaseImplTEST, GetOriginsByPermission002) {
    auto nweb_data_base_impl = std::make_shared<NWebDataBaseImplMock>();
    nweb_data_base_impl->SetDelegate(nullptr);
    const std::string origin = "";
    bool result = false;
    std::vector<std::string> origins;
    EXPECT_EQ(nweb_data_base_impl->GetOriginsByPermission(1, false), origins);
}

TEST(NWebDataBaseImplTEST, ExistFormData001) {
    auto nweb_data_base_impl = std::make_shared<NWebDataBaseImpl>();
    EXPECT_FALSE(nweb_data_base_impl->ExistFormData());
}

TEST(NWebDataBaseImplTEST, ExistFormData002) {
    auto nweb_data_base_impl = std::make_shared<NWebDataBaseImplMock>();
    nweb_data_base_impl->SetDelegate(nullptr);
    EXPECT_FALSE(nweb_data_base_impl->ExistFormData());
}

TEST(NWebDataBaseImplTEST, ClearFormData001) {
    auto nweb_data_base_impl = std::make_shared<NWebDataBaseImpl>();
    nweb_data_base_impl->ClearFormData();
}

TEST(NWebDataBaseImplTEST, ClearFormData002) {
    auto nweb_data_base_impl = std::make_shared<NWebDataBaseImplMock>();
    nweb_data_base_impl->SetDelegate(nullptr);
    nweb_data_base_impl->ClearFormData();
}