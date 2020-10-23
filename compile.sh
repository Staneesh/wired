#!/bin/bash

error_message="Provide debug/release arguments to this script!"

if [ $# -lt 1 ] || [ $# -gt 1 ] 
then
	echo $error_message 
	exit 1
fi

mode=$1
flags=""
common="-lm -pthread -march=native"

if [ $mode == "debug" ]
then
	flags="-O0 -Wall -Wextra -Wshadow -g -fsanitize=undefined"
elif [ $mode == "release" ]
then
	flags="-O3"
else
	echo $error_message
	exit 1
fi

mkdir build > /dev/null 2>$1
mkdir build/debug > /dev/null 2>$1 
mkdir build/release > /dev/null 2>$1 

time gcc source/main.c -o build/"$mode"/main $flags $common

# TODO(stanisz): i dont know why these are created, so this
# deletion is a temporary hotfix
rm debug release > /dev/null 2>$1
