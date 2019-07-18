#!/bin/bash

if [[ $# -eq 1 ]]; then
    dir=$1
else
    echo "please input a diretory of source files"
fi

cd ${dir}
if [[ $? -ne 0 ]]; then
    echo "the diretory is bad"
    exit 1
fi

logfile="${HOME}/checklogs.txt"
resfile="${HOME}/checklogparam.result"

rm -rf ${logfile}
rm -rf ${resfile}

getlogflag=0

for file in `find . -name "*.cpp" -o -name "*.cc"`
do
    while read line
    do
        if [[ ${line} =~ "Log_Info" ]]; then
            echo ${line} >> ${logfile}
            if [[ ${line: -1} != ';' ]]; then
                getlogflag=1
            fi
            continue
        fi

        if [[ ${getlogflag} -eq 1 ]]; then
            echo ${line} >> ${logfile}
            if [[ ${line: -1} == ';' ]]; then
                getlogflag=0
            fi
        fi
    done < ${file}
done