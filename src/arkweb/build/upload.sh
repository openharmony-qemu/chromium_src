#!/bin/bash
# Copyright (c) 2025 Huawei Device Co., Ltd.
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

set +e
CUR_DIR=$PWD
ROOT_DIR="${CUR_DIR%/src*}""/src"
OUT_DIR="$ROOT_DIR/out/musl_64"
SO_DIR="$OUT_DIR/lib.unstripped"
releaseVersion=$(jq -r '.app.versionName' $ROOT_DIR/third_party/ohos_nweb_hap/AppScope/app.json5.template)
buildId=`file $SO_DIR/libarkweb_engine.so | grep '[0-9a-fA-F]\{10,100\}' -o`
UPLOAD_DIR="$OUT_DIR/upload"
NOW_TIME=$(date "+%Y%m%d")

if [ ! -d "$UPLOAD_DIR" ]; then
    mkdir -p "$UPLOAD_DIR"
fi

if [ ! -f "$UPLOAD_DIR/artget" ]; then
    curl -o $UPLOAD_DIR/artget https://cmc-szver-artifactory.cmc.tools.huawei.com/artifactory/CMC-Release/artget/install/prod/Latest/linux/artget
    chmod 755 $UPLOAD_DIR/artget
fi

mkdir -p $UPLOAD_DIR/${releaseVersion}_${NOW_TIME}_${buildId}
cp $SO_DIR/* $UPLOAD_DIR/${releaseVersion}_${NOW_TIME}_${buildId}

if [[ ! -z "${apk_config_svn_path}" ]]; then
    $UPLOAD_DIR/artget push "ArkwebSymbolTable 5.1" -ru software -user x30045917 -pwd encryption:Q21jRGVjcnlwdA==AAAAAgAAAAAAAAAAAAAABgAAAAfnQSwL9MWegvqai7G+IPXIqy4t/iJtrk8oEVHyOoe8UgEAAAEAAAAAAAAAIMtYgrS+CR/D9+xCD16CVp1QU4l3v1FrhV4Vwsdmom2Z -ap "$UPLOAD_DIR/${releaseVersion}_${NOW_TIME}_${buildId}" -rp "${releaseVersion}_${NOW_TIME}_${buildId}"
fi

if [ -f "$OUT_DIR/NWeb-musl_64.hmp" ]; then
    mv "$OUT_DIR/NWeb-musl_64.hmp" "$OUT_DIR/ModuleUpdatePackage.hmp"
    md5=$(md5sum "$OUT_DIR/ModuleUpdatePackage.hmp" | awk '{print $1}')
    echo "MD5: $md5"
    sha1=$(sha1sum "$OUT_DIR/ModuleUpdatePackage.hmp" | awk '{print $1}')
    echo "SHA-1: $sha1"
    sha256=$(sha256sum "$OUT_DIR/ModuleUpdatePackage.hmp" | awk '{print $1}')
    echo "SHA-256: $sha256"
fi

set -e
exit 0
