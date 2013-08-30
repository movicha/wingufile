#!/bin/bash

SCRIPT=$(readlink -f "$0") # haiwen/wingufile-server-1.3.0/upgrade/upgrade_xx_xx.sh
UPGRADE_DIR=$(dirname "$SCRIPT") # haiwen/wingufile-server-1.3.0/upgrade/
INSTALLPATH=$(dirname "$UPGRADE_DIR") # haiwen/wingufile-server-1.3.0/
TOPDIR=$(dirname "${INSTALLPATH}") # haiwen/

echo
echo "-------------------------------------------------------------"
echo "This script would do the minor upgrade for you."
echo "Press [ENTER] to contiune"
echo "-------------------------------------------------------------"
echo
read dummy

echo
echo "------------------------------"
echo "migrating avatars ..."
echo
media_dir=${INSTALLPATH}/winguhub/media
orig_avatar_dir=${INSTALLPATH}/winguhub/media/avatars
dest_avatar_dir=${TOPDIR}/winguhub-data/avatars

# move "media/avatars" directory outside
if [[ ! -d ${dest_avatar_dir} ]]; then
    echo
    echo "Error: avatars directory \"${dest_avatar_dir}\" does not exist" 2>&1
    echo
    exit 1

elif [[ ! -L ${orig_avatar_dir}} ]]; then
    mv ${orig_avatar_dir}/* "${dest_avatar_dir}" 2>/dev/null 1>&2
    rm -rf "${orig_avatar_dir}"
    ln -s ../../../winguhub-data/avatars ${media_dir}
fi

echo "DONE"
echo "------------------------------"
echo
