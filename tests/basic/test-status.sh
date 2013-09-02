#!/bin/bash

. ../common-conf.sh

testdir=${wingufile_dir}/tests/basic
conf1=${testdir}/conf1
worktree=/tmp/worktree
wingufile_app=${wingufile_dir}/app/wingufile
repo_name="test-repo"

./clean.sh
./teardown.sh

mkdir -p ${worktree}/wt1

gnome-terminal -e "${ccnet} -c ${conf1} -D all -f - --no-multicast"
sleep 3
gnome-terminal -e "${winguf_daemon} -c ${conf1} -w ${worktree}/wt1 -l -"
sleep 3

# create a repo
${wingufile_app} -c ${conf1} create ${repo_name} test > /dev/null
sleep 3
repo_id=`ls ${worktree}/wt1/systems`

mkdir -p ${worktree}/wt1/works/${repo_name}/test1/test2
cp ${top_srcdir}/README ${worktree}/wt1/works/${repo_name}
cp ${top_srcdir}/autogen.sh ${worktree}/wt1/works/${repo_name}/test1
cp ${top_srcdir}/configure.ac ${worktree}/wt1/works/${repo_name}/test1/test2
sleep 1

${wingufile_app} -c ${conf1} status ${repo_id}

# add a file
${wingufile_app} -c ${conf1} add ${repo_id} README > /dev/null

${wingufile_app} -c ${conf1} status ${repo_id}

# commit
${wingufile_app} -c ${conf1} commit ${repo_id} commit1 > /dev/null

# add a file
${wingufile_app} -c ${conf1} add ${repo_id} test1/autogen.sh > /dev/null
${wingufile_app} -c ${conf1} add ${repo_id} test1/test2/configure.ac > /dev/null

${wingufile_app} -c ${conf1} status ${repo_id}

# commit
${wingufile_app} -c ${conf1} commit ${repo_id} commit2 > /dev/null

rm ${worktree}/wt1/works/${repo_name}/README
echo "hello" >> ${worktree}/wt1/works/${repo_name}/test1/test2/configure.ac

${wingufile_app} -c ${conf1} status ${repo_id}

echo "hello" >> ${worktree}/wt1/works/${repo_name}/test1/autogen.sh
sleep 1

${wingufile_app} -c ${conf1} add ${repo_id} test1/autogen.sh

${wingufile_app} -c ${conf1} status ${repo_id}

rm -rf ${worktree}
