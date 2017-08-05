#!/bin/bash
export PATH=/root/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/lib/git-core:/root/bin.local
#date +"%d/%m  %H:%M:%S"
echo -n '/:'
df / | tail -1 | nth -2 | tr -d '\n'
echo -n '  /iso2:'
df /iso2 | tail -1 | nth -2
cat /sys/class/thermal/thermal_zone0/temp | tr -d '\n'
echo -n 'C L:'
cat /proc/loadavg |nth 0 | tr -d '\n'
echo -n ' '
printf 'F:%2d' $(cat /sys/class/pwm-sunxi-opi0/pwm0/activecycles)
echo '%'
