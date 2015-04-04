#!/bin/bash

IFS=$'\n'
LOG_FILE="/etc/X11/glm/log/glm.log"
RUNTIME=()

function get_entry_len {
    loc=`grep -n "Date" "${LOG_FILE}" | cut -f1 -d':' | tail -1`
    len=`wc -l "${LOG_FILE}" | cut -f1 -d' '`
    n=$[ ${len} - ${loc} ]
    
    echo "${n}"
}



function is_number {
    check=`echo "$1 + 0" | bc 2> /dev/null`
    
    if [[ "${check}" != "" && "${check}" != 0 ]]; then
        echo true
    else
        echo false
    fi
}



function sort_runtime {
    
    FUNC_RUNTIME+=("$2")
    last=$[ ${#FUNC_RUNTIME[@]} - 1 ]
    echo ${last}
    
    i=0
    for item in "${FUNC_RUNTIME[@]:0:${last}}"; do
        cur=`echo "${item}" | awk '{ print $3 }'`
        j=$[ ${i} + 1 ]
        
        if [ ${j} -gt ${last} ]; then
            break
        fi
        
        for entry in "${FUNC_RUNTIME[@]:${j}:${last}}"; do
            comp=`echo "${entry}" | awk '{ print $3 }'`
            check=`echo "${comp} > ${cur}" | bc`
            
            if [ ${check} -eq 1 ]; then
                FUNC_RUNTIME[${i}]="${entry}"
                FUNC_RUNTIME[${j}]="${item}"
                # echo "Swapping $comp ($j) and $cur ($i)"
                break
            fi
            
            j=$[ ${j} + 1 ]
        done
        
        i=$[ ${i} + 1 ]
    done
}



n=`get_entry_len`

entry=(`tail -"${n}" "${LOG_FILE}"`)
src_file=(`echo "${entry[*]}" | awk '{ print $1 }' | xargs -n1 basename`)
src_func=(`echo "${entry[*]}" | awk '{ print $2 }' | sed 's/(//'`)
src_time=(`echo "${entry[*]}" | awk '{ print $4 }'`)

last=$[ ${#entry[@]} - 1 ]

for i in `seq 0 ${last}`; do
    
    max_file="${src_file[${i}]}"
    max_func="${src_func[${i}]}"
    max_time="${src_time[${i}]}"
    max_loc=${i}
    
    [ `is_number "${max_time}"` == false ] && continue
    
    next=$[ ${i} + 1 ]
    
    for j in `seq ${next} ${last}`; do
        cur_file="${src_file[${j}]}"
        cur_func="${src_func[${j}]}"
        cur_time="${src_time[${j}]}"
        
        [ `is_number "${cur_time}"` == false ] && continue
        
        check=`echo "${max_time} < ${cur_time}" | bc`
        
        if [ ${check} -eq 1 ]; then
            max_file="${cur_file}"
            max_func="${cur_func}"
            max_time="${cur_time}"
            max_loc=${j}
        fi
    done
    
    src_time[${max_loc}]="${src_time[${i}]}"
    
    line=`printf "%-20s %-30s %-10s\n" "${max_file}" "${max_func}" "${max_time}"`
    RUNTIME+=("${line}")
done


echo "${RUNTIME[*]}"
