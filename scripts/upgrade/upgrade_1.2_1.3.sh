#!/bin/bash

SCRIPT=$(readlink -f "$0") # haiwen/wingufile-server-1.3.0/upgrade/upgrade_xx_xx.sh
UPGRADE_DIR=$(dirname "$SCRIPT") # haiwen/wingufile-server-1.3.0/upgrade/
INSTALLPATH=$(dirname "$UPGRADE_DIR") # haiwen/wingufile-server-1.3.0/
TOPDIR=$(dirname "${INSTALLPATH}") # haiwen/
default_ccnet_conf_dir=${TOPDIR}/ccnet
default_wingufile_data_dir=${TOPDIR}/wingufile-data
default_winguhub_db=${TOPDIR}/winguhub.db

export CCNET_CONF_DIR=${default_ccnet_conf_dir}
export PYTHONPATH=${INSTALLPATH}/wingufile/lib/python2.6/site-packages:${INSTALLPATH}/wingufile/lib64/python2.6/site-packages:${INSTALLPATH}/wingufile/lib/python2.7/site-packages:${INSTALLPATH}/winguhub/thirdpart:$PYTHONPATH
export PYTHONPATH=${INSTALLPATH}/wingufile/lib/python2.7/site-packages:${INSTALLPATH}/wingufile/lib64/python2.7/site-packages:$PYTHONPATH

prev_version=1.2.0
current_version=1.3.0

echo
echo "-------------------------------------------------------------"
echo "This script would upgrade your wingufile server from ${prev_version} to ${current_version}"
echo "Press [ENTER] to contiune"
echo "-------------------------------------------------------------"
echo
read dummy

function check_python_executable() {
    if [[ "$PYTHON" != "" && -x $PYTHON ]]; then
        return 0
    fi
        
    if which python2.7 2>/dev/null 1>&2; then
        PYTHON=python2.7
    elif which python27 2>/dev/null 1>&2; then
        PYTHON=python27
    elif which python2.6 2>/dev/null 1>&2; then
        PYTHON=python2.6
    elif which python26 2>/dev/null 1>&2; then
        PYTHON=python26
    else
        echo 
        echo "Can't find a python executable of version 2.6 or above in PATH"
        echo "Install python 2.6+ before continue."
        echo "Or if you installed it in a non-standard PATH, set the PYTHON enviroment varirable to it"
        echo 
        exit 1
    fi
}

function read_wingufile_data_dir () {
    wingufile_ini=${default_ccnet_conf_dir}/wingufile.ini
    if [[ ! -f ${wingufile_ini} ]]; then
        echo "${wingufile_ini} not found. Now quit"
        exit 1
    fi
    wingufile_data_dir=$(cat "${wingufile_ini}")
    if [[ ! -d ${wingufile_data_dir} ]]; then
        echo "Your wingufile server data directory \"${wingufile_data_dir}\" is invalid or doesn't exits."
        echo "Please check it first, or create this directory yourself."
        echo ""
        exit 1;
    fi
}

check_python_executable
read_wingufile_data_dir

export WINGUFILE_CONF_DIR=$wingufile_data_dir

# test whether wingufile server has been stopped.
if pgrep seaf-server 2>/dev/null 1>&2 ; then
    echo 
    echo "wingufile server is still running !"
    echo "stop it using scripts before upgrade."
    echo
    exit 1
elif pgrep -f "manage.py run_gunicorn" 2>/dev/null 1>&2 ; then
    echo 
    echo "winguhub server is still running !"
    echo "stop it before upgrade."
    echo
    exit 1
fi

# run django syncdb command
echo "------------------------------"
echo "updating winguhub database ... "
echo
manage_py=${INSTALLPATH}/winguhub/manage.py
pushd "${INSTALLPATH}/winguhub" 2>/dev/null 1>&2
if ! $PYTHON manage.py syncdb 2>/dev/null 1>&2; then
    echo "failed"
    exit -1
fi
popd 2>/dev/null 1>&2

echo "DONE"
echo "------------------------------"
echo

echo "------------------------------"
echo "migrating avatars ..."
echo
media_dir=${INSTALLPATH}/winguhub/media
orig_avatar_dir=${INSTALLPATH}/winguhub/media/avatars
dest_avatar_dir=${TOPDIR}/winguhub-data/avatars

# move "media/avatars" directory outside 
if [[ ! -d ${dest_avatar_dir} ]]; then
    mkdir -p "${TOPDIR}/winguhub-data"
    mv "${orig_avatar_dir}" "${dest_avatar_dir}" 2>/dev/null 1>&2
    ln -s ../../../winguhub-data/avatars ${media_dir}

elif [[ ! -L ${orig_avatar_dir}} ]]; then
    mv ${orig_avatar_dir}/* "${dest_avatar_dir}" 2>/dev/null 1>&2
    rm -rf "${orig_avatar_dir}"
    ln -s ../../../winguhub-data/avatars ${media_dir}
fi

echo "DONE"
echo "------------------------------"
echo