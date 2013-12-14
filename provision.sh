#!/bin/bash
set -xe

sudo apt-get update

# essential toolchain stuff
sudo apt-get install -y build-essential nasm git mtools

# development niceties
sudo apt-get install -y vim gdb

# get a decent shell prompt going
cat > ~/.bash_profile <<SH
function current-branch {
    git branch --color=never 2>/dev/null | grep '^*' | cut -b2-
}
export PS1=$'\e[33;1m\w\e[36m\$(current-branch) \e[32mλ\e[0m '
SH
