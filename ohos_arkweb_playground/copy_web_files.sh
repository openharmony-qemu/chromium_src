#!/bin/bash
OUTDIR="../out/rk3568_64"

set -e

cp $OUTDIR/*.so demoHap/entry/libs/arm64-v8a/

cp $OUTDIR/ohos_nweb.hap demoHap/entry/src/main/resources/resfile/nweb.hap

echo "copy web resources ok!"
