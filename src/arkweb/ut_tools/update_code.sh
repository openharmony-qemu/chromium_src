WORKSPACE=/home/webauto/ut_workspace
CODE_ROOT=$WORKSPACE/chromium_next_dev
SRC_ROOT=$CODE_ROOT/src
CUR_TIME=`date +"%Y-%m-%d-%H-%M-%S"`

CODE_REPO=https://szv-open.codehub.huawei.com/innersource/shanhai/wutong/oh-chromium/manifest.git
CODE_BRANCH=huawei/ohos/114_20240126_5.0_dev

# 1. Update codes
echo 'begin update chromium code '
if ! [ -d $CODE_ROOT ]; then
    mkdir -p $CODE_ROOT
fi

cd $CODE_ROOT
if ! [ -d $CODE_ROOT/.mm ]; then
   git mm init -u $CODE_REPO -b $CODE_BRANCH
   git mm sync
else
   git mm sync
fi

echo 'update chromium code done'
