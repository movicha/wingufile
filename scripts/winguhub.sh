#!/bin/bash

### BEGIN INIT INFO
# Provides:          winguhub
# Required-Start:    $local_fs $remote_fs $network
# Required-Stop:     $local_fs
# Default-Start:     1 2 3 4 5
# Default-Stop:
# Short-Description: Starts Seahub
# Description:       starts Seahub
### END INIT INFO

echo ""

SCRIPT=$(readlink -f "$0")
INSTALLPATH=$(dirname "${SCRIPT}")
TOPDIR=$(dirname "${INSTALLPATH}")
default_ccnet_conf_dir=${TOPDIR}/ccnet

manage_py=${INSTALLPATH}/winguhub/manage.py
gunicorn_conf=${INSTALLPATH}/runtime/winguhub.conf
pidfile=${INSTALLPATH}/runtime/winguhub.pid
errorlog=${INSTALLPATH}/runtime/error.log
accesslog=${INSTALLPATH}/runtime/access.log


script_name=$0
function usage () {
    echo "Usage: "
    echo
    echo "  $(basename ${script_name}) { start <port> | stop | restart <port> }"
    echo
    echo "To run winguhub in fastcgi:"
    echo
    echo "  $(basename ${script_name}) { start-fastcgi <port> | stop | restart-fastcgi <port> }"
    echo
    echo "<port> is optional, and defaults to 8000"
    echo ""
}

# Check args
if [[ $1 != "start" && $1 != "stop" && $1 != "restart" \
    && $1 != "start-fastcgi" && $1 != "restart-fastcgi" ]]; then
    usage;
    exit 1;
fi

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

function validate_winguhub_running () {
    if pgrep -f "${manage_py}" 2>/dev/null 1>&2; then
        echo "Seahub is already running."
        exit 1;
    fi
}

function validate_port () {
    if ! [[ ${port} =~ ^[1-9][0-9]{1,4}$ ]] ; then
        printf "\033[033m${port}\033[m is not a valid port number\n\n"
        usage;
        exit 1
    fi
}

if [[ ($1 == "start" || $1 == "restart" || $1 == "start-fastcgi" || $1 == "restart-fastcgi") \
    && ($# == 2 || $# == 1) ]]; then
    if [[ $# == 2 ]]; then
        port=$2
        validate_port
    else
        port=8000
    fi
elif [[ $1 == "stop" && $# == 1 ]]; then
    dummy=dummy
else
    usage;
    exit 1
fi

function warning_if_wingufile_not_running () {
    if ! pgrep -f "wingufile-controller -c" 2>/dev/null 1>&2; then
        echo
        echo "Warning: wingufile-controller not running. Have you run \"./wingufile.sh start\" ?"
        echo
    fi
}

function prepare_winguhub_log_dir() {
    logdir=${TOPDIR}/logs
    if ! [[ -d ${logsdir} ]]; then
        if ! mkdir -p "${logdir}"; then
            echo "ERROR: failed to create logs dir \"${logdir}\""
            exit 1
        fi
    fi
    export SEAHUB_LOG_DIR=${logdir}
}

function before_start() {
    check_python_executable;
    validate_ccnet_conf_dir;
    read_wingufile_data_dir;

    warning_if_wingufile_not_running;
    validate_winguhub_running;
    prepare_winguhub_log_dir;

    export CCNET_CONF_DIR=${default_ccnet_conf_dir}
    export WINGUFILE_CONF_DIR=${wingufile_data_dir}
    export PYTHONPATH=${INSTALLPATH}/wingufile/lib/python2.6/site-packages:${INSTALLPATH}/wingufile/lib64/python2.6/site-packages:${INSTALLPATH}/winguhub/thirdpart:$PYTHONPATH
    export PYTHONPATH=${INSTALLPATH}/wingufile/lib/python2.7/site-packages:${INSTALLPATH}/wingufile/lib64/python2.7/site-packages:$PYTHONPATH
}

function start_winguhub () {
    before_start;
    echo "Starting winguhub at port ${port} ..."
    $PYTHON "${manage_py}" run_gunicorn -c "${gunicorn_conf}" -b "0.0.0.0:${port}"

    # Ensure winguhub is started successfully
    sleep 5
    if ! pgrep -f "${manage_py}" 2>/dev/null 1>&2; then
        printf "\033[33mError:Seahub failed to start.\033[m\n"
        echo "Please try to run \"./winguhub.sh start\" again"
        exit 1;
    fi
}

function start_winguhub_fastcgi () {
    before_start;
    echo "Starting winguhub (fastcgi) at port ${port} ..."
    $PYTHON "${manage_py}" runfcgi host=127.0.0.1 port=$port pidfile=$pidfile \
        outlog=${accesslog} errlog=${errorlog}

    # Ensure winguhub is started successfully
    sleep 5
    if ! pgrep -f "${manage_py}" 1>/dev/null; then
        printf "\033[33mError:Seahub failed to start.\033[m\n"
        exit 1;
    fi
}

function stop_winguhub () {
    if [[ -f ${pidfile} ]]; then
        pid=$(cat "${pidfile}")
        echo "Stopping winguhub ..."
        kill ${pid}
        rm -f ${pidfile}
        return 0
    else
        echo "Seahub is not running"
    fi
}

case $1 in
    "start" )
        start_winguhub;
        ;;
    "start-fastcgi" )
        start_winguhub_fastcgi;
        ;;
    "stop" )
        stop_winguhub;
        ;;
    "restart" )
        stop_winguhub
        sleep 2
        start_winguhub
        ;;
    "restart-fastcgi" )
        stop_winguhub
        sleep 2
        start_winguhub_fastcgi
        ;;
esac

echo "Done."
echo ""
