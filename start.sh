#!/bin/bash
ulimit -c unlimited
echo "/tmp/core-%e-%p-%t" > /proc/sys/kernel/core_pattern
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../boost_1_61_0/stage/lib:../lib:../glog_0_3_3:./lib/:/usr/local/lib
./FollowOrderPro 
