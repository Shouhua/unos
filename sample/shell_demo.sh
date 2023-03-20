#! /usr/bin/env bash
set -ueo pipefail

# string expansion
GREETING=""

if [ -z $GREETING ]; then echo "$GREETING is NULL"; else echo "$GREETING is not NULL"; fi;
echo $GREETING
# 获取字符串长度
echo 'GREETING length:' ${#GREETING}
# 为空替换
echo '${GREETING:-word1}:' ${GREETING:-word1} 
echo $GREETING
# 为空替换，并且将值赋给$GREETING变量
echo '${GREETING:=word2}:' ${GREETING:=word2} 
echo $GREETING
# 为空报错
echo '${GREETING:?word3}:' ${GREETING:?word3} 
echo $GREETING
# 不为空替换
echo '${GREETING:+word4}:' ${GREETING:+word4} 
echo $GREETING
echo '---------------------------------' 
# ！开头间接引用indirect ref
param="var"
var="helo, world!"
echo ${!param}
echo '---------------------------------' 
echo ${GREETING#wo}
echo ${GREETING##wo}
# *表示任意长度字符，？表示单个字符长度
echo ${GREETING%rd*}
echo ${GREETING%%rd?}
echo '---------------------------------' 
# 只替换第一个匹配
echo ${GREETING/or/ro} 
# 替换所有匹配
echo ${GREETING//or/ro}

set_dash() {
    set - new argument
    echo "set_dash function:"
    echo $1
    echo $2
}

set_double_dash() {
    set -- new argument
    echo "set_double_dash function:"
    echo $1
    echo $2
}

# set -
# 如果后面没有参数不会unset传参
# set --
# 不管后面有没有参数，都会unset传参
set_dash 1 2 3
set_double_dash 1 2 3

# 如果返回0表示成功，!0表示failure
# 其他表示failure，!1表示successful
# https://unix.stackexchange.com/questions/437812/what-does-typing-a-single-exclamation-mark-do-in-bash
# ! 1，！默认是历史命令的展开，所以使用！加数字要注意，以下例子中_mysql_want_help函数使用数字返回值没有问题，这里是要解释这个现象
# ! true; echo "! true is $?"
# ! 1; echo "! true is $?"
# NOTICE
# set -e 函数返回非0值表示有问题，如果单行调用函数，会退出程序；但是可以在if语句中判断，就像这个例子调用函数判断一样，程序可以继续执行
# 如果没有set -e出现问题还是可以继续执行下去
_mysql_want_help() {
	local arg
	for arg; do
		case "$arg" in
			-'?'|--help|--print-defaults|-V|--version)
				return 0
				;;
		esac
	done
	return 1
}

if ! _mysql_want_help "mysqld"; then
    echo $?
    echo "Need help info"
else
    echo $?
    echo "Dont Need help info"
fi

_arg_demo() {
    local arg
    # for没有in，默认使用为for arg in "$@"
    for arg; do
        echo $arg
    done
}
_arg_demo "helo" "world"

# array
PROJ="proj1"
declare -A test=(
    ["proj1"]=${BRANCH:-"dev"}
)

echo "${test[${PROJ}]}" # dev