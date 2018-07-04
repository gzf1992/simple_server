#!/bin/bash
# Program:
#   This program is used to generate system ctags for vim editor and auto add path for vim
#History:
# 2012/12/6     Kom     First release
 
PATH=/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin:/~/bin
export PATH
 
# The ctags result will be store at /home/$(USER)/.vim/systags
systagsdirpath="/home/${USER}/.vim/"
systagspath="${systagsdirpath}systags"
#echo ${systagsdirpath}
#echo ${systagspath}
 
# detect specific directory is exist
# return:
#   0: exist
#   1: doesn't exist
#   2: parameters number error
function isdirexist()
{
    if [ $# -eq 0 ] || [ $# -gt 1 ]; then
        return 2
    fi
    if [ -d $1 ]; then
        return 0
    else
        return 1
    fi
}
 
#detect specific file is exist
#return:
#   0: exist
#   1: doesn't exist
#   2: parameters number error
function isfileexist()
{
    if [ $# -eq 0 ] || [ $# -gt 1 ]; then
        return 2
    fi
    if [ -e $1 ]; then
        return 0
    else
        return 1
    fi
}
 
# if the dir to store systags doesn't exist, create it
isdirexist ${systagsdirpath}
if [ $? == 1 ]; then
    echo -n "'"${systagsdirpath}"' doesn't exist, now create ... "
    mkdir ${systagsdirpath}
    isdirexist ${systagsdirpath}
    if [ $? == 0 ]; then
        echo "success."
    else
        echo "FAILED."
        return 1
    fi
else
    echo "'"${systagsdirpath}"' already exist."
fi
 
# if the systags file already exist, delete it
isfileexist ${systagspath}
if [ $? == 1 ]; then
    echo "'"${systagspath}"' doesn't exist."
else
    echo -n "'"${systagspath}"' already exist, now delete is ... "
    rm -fr ${systagspath}
    isfileexist ${systagspath}
    if [ $? == 1 ]; then
        echo "success."
    else
        echo "FAILED."
        return 1
    fi
fi
 
# now, generate tags file
echo -n "Now, generate system ctags for vim editor ... "
ctags -I __THROW -I __attribute__ -I __attribute_pure__ -I __nonnull --file-scope=yes --langmap=c:+.h --languages=c,c++ --links=yes --c-kinds=+p --c++-kinds=+p --fields=+iaS --extra=+q -R -f ${systagspath} /usr/include /usr/local/include
#ctags -I __THROW -I __attribute__ -I __attribute_pure__ -I __nonnull --file-scope=yes --langmap=c:+.h --languages=c,c++ --links=yes --c-kinds=+p --c++-kinds=+p --fields=+iaS --extra=+q -f ${systagspath} /usr/include/* /usr/include/sys/* /usr/include/bits/* /usr/include/netinet/* /usr/include/arpa/* /usr/include/mysql/*
 
echo "success."
 
# now, add it to the vim
echo -n "Now, add it to the vim ... "
echo "set tags+=${systagspath}" >> /home/${USER}/.vimrc
echo "success."
