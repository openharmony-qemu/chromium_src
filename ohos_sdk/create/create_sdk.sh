#!/usr/bin/env bash

SDK_VERSION=$1
SDK_PACKAGE=$2

SDK_PATH=openharmony
SDK_NAME=HarmonyOS-NEXT-DB2

print_usage() {
    printf "Put this script into the same dir with original sdk package.\n"
    printf "Usage: create_sdk.sh {sdk version} {original sdk package}\n"
    printf "\texample: ./create_sdk.sh 5.0.0.31 commandline-tools-linux-x64-5.0.3.500.zip\n"
}

if [ -z "${SDK_VERSION}" ] || [ -z "${SDK_PACKAGE}" ]; then
    print_usage
    exit 1
fi

echo "Begin to extract ${SDK_PACKAGE} ..."
if ! unzip -q "${SDK_PACKAGE}"; then
    echo "extract ${SDK_PACKAGE} failed."
    exit 1
fi

cp -r command-line-tools/sdk/${SDK_NAME}/${SDK_PATH}/ ./
mkdir ets js native previewer toolchains ohos-sdk

ln -s ../$SDK_PATH ohos-sdk/linux
ln -s ../$SDK_PATH/ets ets/$SDK_VERSION
ln -s ../$SDK_PATH/js js/$SDK_VERSION
ln -s ../$SDK_PATH/native native/$SDK_VERSION
ln -s ../$SDK_PATH/previewer previewer/$SDK_VERSION
ln -s ../$SDK_PATH/toolchains toolchains/$SDK_VERSION

TARGET_SDK_FILE="ohos-sdk-linux-$SDK_VERSION.tar.gz"
echo "Begion to pack ohos-sdk: ${TARGET_SDK_FILE}"
tar -czf "${TARGET_SDK_FILE}" ets js native previewer toolchains ohos-sdk $SDK_PATH

split -d -b 1G "${TARGET_SDK_FILE}" "${TARGET_SDK_FILE}."
rm -f "${TARGET_SDK_FILE}"

