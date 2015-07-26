#!/bin/bash
# lazy man's script to generate the random times for events

function check_file_exists {
    if [ -f $1 ]; then
        printf "\n  \033[1;33mAre you sure you want to wipe $1 and others \033[1;37m[\033[1;32mY\033[1;37m/\033[1;31mn\033[1;37m]?\033[0m "
        read yn
        if [[ $yn == [Nn] ]];then exit 0; fi
    fi
}

# some setup parameters
EVENT_RANGE=50
EVENT_MIN=10

NS_MUL=1000000
NS_MIN=250000000

# possible keys for mouse
keys='0123456789-=[]\;'',./abcdefghijklmnopqrstuvwxyz'
n_keys=${#keys}

# make required subdirectories
mkdir -p usb
mkdir -p dev
mkdir -p output

input_file=./usb/input
known_dev_file=./known_devices

check_file_exists ./usb/input0

devices=('mouse' 'keyboard')
n_check=$((${#devices[@]} - 1))

# get all device IDs in array
IDS=(`cat $known_dev_file | cut -d' ' -f2`)

# get number of devices
n_devices=$((`cat $known_dev_file | wc -l`))
printf "\n  \033[1;35m$n_devices devices \033[0min\033[1;36m $known_dev_file\n\n"

dev_count=0
# for each device in known_devices...
for dev in `eval echo {1..$n_devices}`; do

    # ...find the full name of the current device and...
    current_device=`head -n$dev $known_dev_file | tail -n1`
    printf "  \033[1;32mSearching for device \033[1;35m$current_device\033[0m...\n"

    # ...scoll through all device types ({mouse, keyboard}) to find whether this one exists
    device_type=NULL
    for which_device in `eval echo {0..$n_check}`; do
        if [[ `echo $current_device | grep -ic "${devices[$which_device]}"` -gt 0 ]]; then
            device_type=${devices[$which_device]}
        fi
    done

    # check whether we found a corresponding device type
    if [ $device_type == "NULL" ]; then
        continue
    fi

    # generate random number of events for input file
    dev_file=$input_file$dev_count;
    n_events=$((RANDOM % $EVENT_RANGE + $EVENT_MIN))
    printf "  \033[1;33m>>> \033[0m$n_events events\033[0m generated for a $device_type > \033[1;36m$dev_file.\n"

    for t in `eval echo {0..$n_events}`; do
        # generate random delay between 0-2s and 2e9-10e9 ns
        tv_sec=$((RANDOM%3))
        tv_nsec=$(( ( RANDOM % 100 ) * $NS_MUL + $NS_MIN))

        unset activity
        # if event > 1, then generate random activity (we need 04x:04x ID and device type)
        if [ $t -gt 0 ]; then
            if [[ $device_type == 'mouse' ]]; then
                activity=($((RANDOM%3)) $((RANDOM%3)) $((RANDOM % 255 - 127)) $((RANDOM % 255 - 127)))
            else
                activity=($((RANDOM % $n_keys)) $((RANDOM % 2)) $((RANDOM % 2)))
            fi
        fi
        echo "$tv_sec $tv_nsec ${activity[@]}" >> tmp
    done
    # format the first line to have ID and device type
    cat tmp | sed "1s/$/${IDS[$dev-1]} $device_type/"> $dev_file
    rm -f tmp
    dev_count=$(($dev_count+1))
done

printf "\n"

