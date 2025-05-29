#!/bin/bash
export NB_FILE_SERVER_IP=7.190.160.98:7.190.160.230:7.190.160.207:7.190.163.243:7.190.161.35
CUR_DIR=$PWD
ROOT_DIR="${CUR_DIR%/src*}""/src"

for arg in "$@"; do
        case $arg in
                -cfi)
                        echo "pc"
                        bash nextbuild.sh -t p musl_64 -G use_cfi=true
                        bash $ROOT_DIR/arkweb/build/upload.sh
                        ;;
                -watch)
                        echo "watch"
                        bash nextbuild.sh -t p musl_64 -G build_watch=true
                        bash $ROOT_DIR/arkweb/build/upload.sh
                        ;;
                -x86)
                        echo "x86"
                        bash nextbuild.sh -t p x86_64
                        bash $ROOT_DIR/arkweb/build/upload.sh
                        ;;
                -ut)
                        echo "ut"
                        bash nextbuild.sh  -A -t allut musl_64 -coverage -j 16
                        bash $ROOT_DIR/arkweb/build/upload.sh
                        cd $ROOT_DIR
                        zip -r ut.zip out/musl_64/ut
                        ;;
                -coreut)
                        echo "coreut"
                        bash nextbuild.sh -A -t coreut musl_64 -coverage
                        bash $ROOT_DIR/arkweb/build/upload.sh
                        cd $ROOT_DIR
                        zip -r report.zip out/musl_64/ut
                        ;;
                *)
                        echo"error:$arg"
                        exit 1
                        ;;
        esac
done

if [ $# -eq 0 ];then
        bash nextbuild.sh -t p musl_64
        bash $ROOT_DIR/arkweb/build/upload.sh
fi
