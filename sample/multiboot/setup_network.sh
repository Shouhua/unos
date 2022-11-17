#!/usr/bin/env bash

set -ueo pipefail

sudo pacman -Ss dhclient
sudo ip link add br0 type bridge
sudo ip link set dev ens33 down
sudo ip link set dev ens33 master br0

sudo ip tuntap add dev tap0 mode tap
sudo ip link set dev tap0 master br0
sudo ip link set dev tap0 up

sudo ip link set dev ens33 up
sudo ip link set dev br0 up
sudo dhclient br0


#sudo ip address add dev br0 192.168.126.128/24

#sudo ip link set dev tap0 down
#sudo ip link delete dev tap0 master br0
#sudo ip link delete dev br0 type bridge
