#!/bin/bash
# Copyright (c) 2021 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set -x

build_type="$1"
root_path="."
sdk_path="${root_path}/src/ohos_sdk"
sign_tool_path="${sdk_path}/14/toolchains/lib"
nosign_hap_path="${root_path}/src/out/${build_type}/ohos_nweb.hap"
sign_hap_path="${root_path}/src/out/${build_type}/NWeb-${build_type}.hap"
profile_path="${root_path}/src/third_party/ohos_nweb_hap/signature"

java -jar ${sign_tool_path}/hap-sign-tool.jar sign-profile -keyAlias "openharmony application profile release" -signAlg "SHA256withECDSA" -mode "localSign" -profileCertFile "${sign_tool_path}/OpenHarmonyProfileRelease.pem" -inFile "${profile_path}/UnsgnedReleasedProfileTemplate.json" -keystoreFile "${sign_tool_path}/OpenHarmony.p12" -outFile "openharmony_nweb.p7b" -keyPwd "123456" -keystorePwd "123456"
java -jar ${sign_tool_path}/hap-sign-tool.jar sign-app -keyAlias "openharmony application release" -signAlg "SHA256withECDSA" -mode "localSign" -appCertFile "${sdk_path}/OpenHarmonyApplication.pem" -profileFile "openharmony_nweb.p7b" -inFile "${nosign_hap_path}" -keystoreFile "${sign_tool_path}/OpenHarmony.p12" -outFile "${sign_hap_path}" -keyPwd "123456" -keystorePwd "123456"
