#!/bin/bash

SCRIPT=$(readlink -f "$0")
INSTALLPATH=$(dirname "${SCRIPT}")
TOPDIR=$(dirname "${INSTALLPATH}")
default_ccnet_conf_dir=${TOPDIR}/ccnet

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

check_python_executable;
read_wingufile_data_dir;

export CCNET_CONF_DIR=${default_ccnet_conf_dir}
export WINGUFILE_CONF_DIR=${wingufile_data_dir}
export PYTHONPATH=${INSTALLPATH}/wingufile/lib/python2.6/site-packages:${INSTALLPATH}/wingufile/lib64/python2.6/site-packages:${INSTALLPATH}/winguhub/thirdpart:$PYTHONPATH
export PYTHONPATH=${INSTALLPATH}/wingufile/lib/python2.7/site-packages:${INSTALLPATH}/wingufile/lib64/python2.7/site-packages:$PYTHONPATH

manage_py=${INSTALLPATH}/winguhub/manage.py
exec "$PYTHON" "$manage_py" createsuperuser