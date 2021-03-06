#!/bin/bash

. ../common-conf.sh

testdir=${wingufile_dir}/tests/basic
conf1=${testdir}/conf1
worktree=/tmp/worktree
wingufile_app=${wingufile_dir}/app/wingufile
repo_name="test-repo"

./clean.sh
./teardown.sh

rm -r ${worktree}/wt1
mkdir -p ${worktree}/wt1
rm ${conf1}/wingufile/repo.db

gnome-terminal -e "${ccnet} -c ${conf1} -D all -f - --no-multicast"
sleep 3
gnome-terminal -e "${winguf_daemon} -c ${conf1} -w ${worktree}/wt1 -l -"
sleep 3

# create a repo
tmp=`${wingufile_app} -c ${conf1} create ${repo_name} test /tmp/ccnet1`
repoid1=`echo $tmp | awk '{print $6}' | awk -F. '{print $1}'`
sleep 1
tmp=`${wingufile_app} -c ${conf1} create ${repo_name} test /tmp/ccnet2`
repoid2=`echo $tmp | awk '{print $6}' | awk -F. '{print $1}'`
sleep 1
tmp=`${wingufile_app} -c ${conf1} create ${repo_name} test /tmp/ccnet3`
repoid3=`echo $tmp | awk '{print $6}' | awk -F. '{print $1}'`
sleep 1
tmp=`${wingufile_app} -c ${conf1} create ${repo_name} test /tmp/ccnet4`
repoid4=`echo $tmp | awk '{print $6}' | awk -F. '{print $1}'`
sleep 1
tmp=`${wingufile_app} -c ${conf1} create ${repo_name} test /tmp/ccnet5`
repoid5=`echo $tmp | awk '{print $6}' | awk -F. '{print $1}'`
sleep 1
tmp=`${wingufile_app} -c ${conf1} create ${repo_name} test /tmp/ccnet6`
repoid6=`echo $tmp | awk '{print $6}' | awk -F. '{print $1}'`

ls /tmp
sleep 2

${wingufile_app} -c ${conf1} repo-rm ${repoid1} > /dev/null
sleep 1
${wingufile_app} -c ${conf1} repo-rm ${repoid2} > /dev/null
sleep 1
${wingufile_app} -c ${conf1} repo-rm ${repoid3} > /dev/null
sleep 1
${wingufile_app} -c ${conf1} repo-rm ${repoid4} > /dev/null
sleep 1
${wingufile_app} -c ${conf1} repo-rm ${repoid5} > /dev/null
sleep 1
${wingufile_app} -c ${conf1} repo-rm ${repoid6} > /dev/null

ls /tmp

# create a repo
tmp=`${wingufile_app} -c ${conf1} create ${repo_name} test`
repoid1=`echo $tmp | awk '{print $6}' | awk -F. '{print $1}'`
sleep 1
tmp=`${wingufile_app} -c ${conf1} create ${repo_name} test`
repoid2=`echo $tmp | awk '{print $6}' | awk -F. '{print $1}'`
sleep 1
tmp=`${wingufile_app} -c ${conf1} create ${repo_name} test`
repoid3=`echo $tmp | awk '{print $6}' | awk -F. '{print $1}'`
sleep 1
tmp=`${wingufile_app} -c ${conf1} create ${repo_name} test`
repoid4=`echo $tmp | awk '{print $6}' | awk -F. '{print $1}'`
sleep 1
tmp=`${wingufile_app} -c ${conf1} create ${repo_name} test`
repoid5=`echo $tmp | awk '{print $6}' | awk -F. '{print $1}'`
sleep 1
tmp=`${wingufile_app} -c ${conf1} create ${repo_name} test`
repoid6=`echo $tmp | awk '{print $6}' | awk -F. '{print $1}'`

ls /tmp/worktree/wt1/systems
sleep 2

${wingufile_app} -c ${conf1} repo-rm ${repoid1} > /dev/null
sleep 1
${wingufile_app} -c ${conf1} repo-rm ${repoid2} > /dev/null
sleep 1
${wingufile_app} -c ${conf1} repo-rm ${repoid3} > /dev/null
sleep 1
${wingufile_app} -c ${conf1} repo-rm ${repoid4} > /dev/null
sleep 1
${wingufile_app} -c ${conf1} repo-rm ${repoid5} > /dev/null
sleep 1
${wingufile_app} -c ${conf1} repo-rm ${repoid6} > /dev/null

ls /tmp/worktree/wt1/systems
