#!/bin/bash

### BEGIN INIT INFO
# Provides:          wingufile
# Required-Start:    $local_fs $remote_fs $network
# Required-Stop:     $local_fs
# Default-Start:     1 2 3 4 5
# Default-Stop:
# Short-Description: Starts Wingufile Server
# Description:       starts Wingufile Server
### END INIT INFO

echo ""

SCRIPT=$(readlink -f "$0")
INSTALLPATH=$(dirname "${SCRIPT}")
TOPDIR=$(dirname "${INSTALLPATH}")
default_ccnet_conf_dir=${TOPDIR}/ccnet
ccnet_pidfile=${INSTALLPATH}/runtime/ccnet.pid
winguf_controller="${INSTALLPATH}/wingufile/bin/wingufile-controller"


export PATH=${INSTALLPATH}/wingufile/bin:$PATH
export WINGUFILE_LD_LIBRARY_PATH=${INSTALLPATH}/wingufile/lib/:${INSTALLPATH}/wingufile/lib64:${LD_LIBRARY_PATH}

script_name=$0
function usage () {
    echo "usage : "
    echo "$(basename ${script_name}) { start | stop | restart } "
    echo ""
}

# check args
if [[ $# != 1 || ( "$1" != "start" && "$1" != "stop" && "$1" != "restart" ) ]]; then
    usage;
    exit 1;
fi

function validate_ccnet_conf_dir () {
    if [[ ! -d ${default_ccnet_conf_dir} ]]; then
        echo "Error: there is no ccnet config directory."
        echo "Have you run setup-wingufile.sh before this?"
        echo ""
        exit -1;
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

function test_config() {
    if ! LD_LIBRARY_PATH=$WINGUFILE_LD_LIBRARY_PATH ${winguf_controller} --test -c "${default_ccnet_conf_dir}" -d "${wingufile_data_dir}"; then
        exit 1;
    fi
}

function check_component_running() {
    name=$1
    cmd=$2
    if pid=$(pgrep -f "$cmd" 2>/dev/null); then
        echo "[$name] is running, pid $pid. You can stop is by: "
        echo
        echo "        kill $pid"
        echo
        echo "Stop it and try again."
        echo
        exit
    fi
}

function validate_already_running () {
    if pid=$(pgrep -f "wingufile-controller -c ${default_ccnet_conf_dir}" 2>/dev/null); then
        echo "Wingufile controller is already running, pid $pid"
        echo
        exit 1;
    fi

    check_component_running "ccnet-server" "ccnet-server -c"
    check_component_running "winguf-server" "winguf-server -c"
    check_component_running "httpserver" "httpserver -c"
}

function start_wingufile_server () {
    validate_already_running;
    validate_ccnet_conf_dir;
    read_wingufile_data_dir;
    test_config;

    echo "Starting wingufile server, please wait ..."

    LD_LIBRARY_PATH=$WINGUFILE_LD_LIBRARY_PATH ${winguf_controller} -c "${default_ccnet_conf_dir}" -d "${wingufile_data_dir}"

    sleep 3

    # check if wingufile server started successfully
    if ! pgrep -f "wingufile-controller -c ${default_ccnet_conf_dir}" 2>/dev/null 1>&2; then
        echo "Failed to start wingufile server"
        exit 1;
    fi

    echo "Wingufile server started"
    echo
}

function stop_wingufile_server () {
    if ! pgrep -f "wingufile-controller -c ${default_ccnet_conf_dir}" 2>/dev/null 1>&2; then
        echo "wingufile server not running yet"
        return 1;
    fi

    echo "Stopping wingufile server ..."
    pkill -SIGTERM -f "wingufile-controller -c ${default_ccnet_conf_dir}"
    pkill ccnet-server
    pkill winguf-server
    pkill httpserver
    return 0
}

function restart_wingufile_server () {
    stop_wingufile_server;
    sleep 2
    start_wingufile_server;
}

case $1 in
    "start" )
        start_wingufile_server;
        ;;
    "stop" )
        stop_wingufile_server;
        ;;
    "restart" )
        restart_wingufile_server;
esac

echo "Done."
