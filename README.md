## Setup environment on Ubuntu 22.04 desktop
### basic software
```shell
sudo apt install curl
sudo apt install git
sudo apt install tmux
sudo apt install build-essential
sudo apt install openssh-server
sudo add-apt-repository ppa:jonathonf/vim
sudo apt update
sudo apt install vim
sudo apt install zsh
sh -c "$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)"
```
### Bochs
1. Install from apt
```shell
sudo apt install bochs
sudo apt install bochs-x
sudo apt install bochs-sdl
```
2. [Build from source](https://wiki.osdev.org/Bochs)
```shell
# dispaly_library: sdl2, options=debug_gui
sudo apt install libgtk2.0-dev
git clone https://github.com/bochs-emu/Bochs.git
git checkout REL_2_7_FINAL
cd Bochs/bochs
# 命令目录可以通过./configure --help查看，默认在/usr/local/bin/bochs, 需要修改bochsrc中romimage的位置：/usr/local/share/bochs/BIOS-bochs-latest
./configure --enable-debugger --with-sdl2 --enable-usb-xhci --enable-raw-serial --enable-usb --enable-pcidev --enable-cdrom --enable-plugins --enable-debugger-gui --with-x11
make
sudo make install
# sudo make uninstall
# make clean
```
## Cross gcc compiler
1. Create Dockerfile
```shell
FROM randomdude/gcc-cross-x86_64-elf
RUN apt update
RUN apt upgrade -y
RUN apt install -y nasm
RUN apt install -y xorriso
RUN apt isntall -y grub-pc-bin
RUN apt install -y grub-common

VOLUME /root/env
WORKDIR /root/env
```
2. Build docker image
```shell
docker build -t ./Dockerfile -t my-buildenv .
```
3. Run docker image
```shell
docker run --rm -it -v $pwd:/root/env my-buildenv
```
4. Make build
```shell
make clean
make
grub-mkrescue -o unos.iso iso
```
### grub2
```shell
sudo apt install grub-pc-bin
sudo apt install grub-common
sudo apt install xorriso
sudo apt install mtools # 有可能会报错invalid mformat
```

## objdump命令查看object file
```shell
objdump -h list all list segement
objdump -s list all segement content
objdump -t list all symbols
objdump -d disassembly
objdump -j .rodata display information only for secion
objdump -M intel -d disassembly with intel format assembly code
objdump -r display relocation table
```

## Serial Port
LSB: The Least Significant Byte
MSB: The Most Significant Byte