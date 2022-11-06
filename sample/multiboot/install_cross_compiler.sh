#! /usr/bin/env bash
set -euo pipefail

# https://wiki.osdev.org/GCC_Cross-Compiler#Preparing_for_the_build
# 主要内容来源于上述wiki，本脚本主要用于安装gcc cross compiler，环境是ubuntu22.04

# install dependencies
sudo apt update &&\
	sudo apt install -y \
		build-essential \
		bison\
		flex\
		libgmp3-dev\
		libmpc-dev\
		libmpfr-dev\
		texinfo

# source code and build directory
SRC_DIR="$HOME/software/gcc-cross-compiler"

GCC_VERSION=12.2.0 
GCC_DIR="gcc-${GCC_VERSION}"
GCC_URL= "https://ftp.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.xz"

BINUTILS_VERSION=2.39
BINUTILS_DIR="binutils-${BINUTILS_VERSION}"
BINUTILS_URL="https://ftp.gnu.org/gnu/binutils/binutils-${BINUTILS_VERSION}.tar.xz"

# configure parameters
# 命令安装目录
export PREFIX="$HOME/cross-compiler"
export TARGET="i386-elf"
export PATH="$PREFIX/bin:$PATH"
# echo 'export PATH="$PREFIX/bin:$PATH"' >> $HOME/.bashrc

mkdir -p "${PREFIX}"
# NOTE: 必须新建额外的build目录，否则编译gcc可能出错，而且编译时间可能会持续半小时
mkdir -p "${SRC_DIR}/build-gcc"
mkdir -p "${SRC_DIR}/build-binutils"

cd "${SRC_DIR}"

# download and extract gcc and binutils source code
if [ ! -d "${SRC_DIR}/${GCC_DIR}" ]
then
	(curl -LO ${GCC_URL} \
		&& tar xf "${GCC_DIR}.tar.xz") || exit
	# rm -r "${GCC_DIR}.tar.xz"
else
	echo "skip downloading gcc"
fi

if [ ! -d "${SRC_DIR}/${BINUTILS_DIR}" ]
then
	(curl -LO ${BINUTILS_URL} \
		&& tar xf "${BINUTILS_DIR}.tar.xz") || exit
	# rm -r "${BINUTILS_DIR}.tar.xz"
else
	echo "skip downloading binutils"
fi

echo "Building binutils"

cd "${SRC_DIR}/build-binutils"
("${SRC_DIR}/${BINUTILS_DIR}/configure" --target=$TARGET --prefix="$PREFIX" \
	--with-sysroot --disable-nls --disable-werror) || exit
(make && make install) || exit
echo "Building binutils successfully!"

echo "Building gcc"

cd "${SRC_DIR}/build-gcc"
which -- "$TARGET-as" || echo "$TARGET-as is not in the PATH"
("${SRC_DIR}/${GCC_DIR}/configure" --target=$TARGET --prefix="$PREFIX" \
	--disable-nls --enable-languages=c,c++ --without-headers) || exit

(make all-gcc &&\
 make all-target-libgcc &&\
 make install-gcc &&\
 make install-target-libgcc) || exit

echo "done"
