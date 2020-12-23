#!/bin/sh
cd `dirname $0`
RELEASE_PATH="/etc/*-release"
distr=""
if grep -qi "centos" ${RELEASE_PATH}; then
    distr="centos"
elif grep -qi "red hat" ${RELEASE_PATH}; then
    distr="redhat"
elif grep -qi "ubuntu" ${RELEASE_PATH}; then
    distr="ubuntu"
elif grep -qi "debian" ${RELEASE_PATH}; then
    distr="debian"
elif grep -qiE "sles.*12" ${RELEASE_PATH}; then
    distr="sles12"
elif grep -qiE "sles.*13" ${RELEASE_PATH}; then
    distr="sles13"
fi

case "$distr" in
    "centos" | "redhat" )
        make nmon_x86_rhel6
        ;;
    "debian" )
        make nmon_x86_debian3
        ;;
    "ubuntu" )
        make nmon_x86_ubuntu1404
        ;;
    "sles12" )
        make nmon_x86_sles12
        ;;
    "sles13" )
        make nmon_x86_sles113
        ;;
    * )
        echo "Your operation system is not supported by build script. Please look makefile to build nmon for your operation system."
        exit 1
        ;;
esac
exit 0
        


