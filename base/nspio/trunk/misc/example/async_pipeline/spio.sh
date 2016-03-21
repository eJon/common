#!/bin/bash

bin_wd="/home/w/bin/nspiod"
conf_wd="./conf"

if [[ $1 == "start" ]]; then
    $bin_wd -c $conf_wd -s start
elif [[ $1 == "stop" ]]; then
    $bin_wd -c $conf_wd -s stop
elif [[ $1 == "restart" ]]; then
    $bin_wd -c $conf_wd -s restart
fi

sleep 0.2
