## Qestions
1. 不能获取网络中断  
检查PIC设置，是否开启了对应的中断开关，具体设置可以参见idt.c里面的enable_irq
2. 有网络中断，但是不能获取正确的网络类型
检查receive buffer设置是否正确，特别注意这里使用物理地址，需要虚拟地址转化成物理地址，但是virt2phys函数写的有问题，导致不能正确读取数据，参见rtl8139.c里面的init设置
3. 正确释放申请的内容块

## Network debug
主要使用qemu模拟rtl8139, 使用如下代码：
```shell
qemu-system-i386 -cdrom $(BUILD_DIR)/$(OS_ISO) -m $(QEMU_MEM) -serial stdio \
	-netdev tap,id=b1 -device rtl8139,netdev=b1 # 详情可以查看qemu-system-i386 -h
# -netdev tap,id=u1,helper=/usr/lib/qemu/qemu-bridge-helper -device rtl8139,netdev=u1,id=f1 \
# -object filter-dump,id=f1,netdev=u1,file=dump.dat
# -nic tap,mac=52:54:0:12:34:56,model=rtl8139,helper=/usr/lib/qemu/qemu-bridge-helper
```
主要的参考资料可以使用```qemu-system-i386 -h```, qemu模拟网络分为前端和后端，前端指的是guest里面使用的模拟网络设备，比如本例中使用的rtl8139
；后端指的是跟host宿主机沟通的中间设备，比如本例中使用tap设备, 上面代码主要设置qemu启动参数项，启动参数项经历好多版本迭代，网上资料不一定准确，最好参看help参数阅读。
### 宿主机配置网络环境
主要实验环境是win11里面使用vmware安装archlinux，所以默认情况下archlinux可以认为是qemu的宿主环境，使用物理网络ens33；但是如果本身宿主环境不是使用网络网路，而是使用无线比如wifi网络，情况会不一样，后面会有提到。配置的环境主要原理是通过tap转发qemu guest机器的网络，然后通过虚拟bridge转发到实体物理网卡转发出去，但是需要set ens33 master到bridge，导致物理网卡不能正常工作，两外wifi网络不能set master到virtual bridge，所以宿主机是wifi网络情况下, 可以使用iptables forward请求到网卡发出去。  
1. 非wifi情况下
```shell
sudo pacman -S bridge-utils net-tools dhclient
sudo ip link add dev br0 type bridge
sudo ip link set dev ens33 master br0
sudo ip tuntap add dev tap0 mode tap
sudo ip link set dev tap0 master br0

sudo systemctl stop dhcpcd #关闭dhcp client，防止ens33的ip重新获取
sudo ip addr delete 192.168.126.128/24 dev ens33
sudo ip addr add 192.168.126.128/24 dev br0
```
此时ens33一端连接网卡输出，另一端连接br0；tap0一端连接br0, 一端连接qemu guest网卡。就算ens33保留ip，也不会ping通了，此时[ens33就相当于一根网线](https://segmentfault.com/a/1190000009491002?utm_source=sf-similar-article)

2. wifi情况
https://wiki.qemu.org/Documentation/Networking/NAT  
https://wiki.gentoo.org/wiki/QEMU/Bridge_with_Wifi_Routing
                               
