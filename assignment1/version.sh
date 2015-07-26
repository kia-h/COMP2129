#! /bin/bash

###################
# Version
# author: Internet's own boy
###################

#prints out the script file name
echo ./`basename $0`;

fieName="version.c"

#hear document, using 'cat' command to make a file by given name..we could have used '$1', then user have to enter the file name
cat <<endOfFile >$fieName
#include<stdio.h>
#include<stdlib.h>
int main(void){printf("\n");}
endOfFile

#compile the c program
gcc -W -Wall -pedantic -std=gnu99 ./$fieName
  
#prints out the c file name
echo ./`basename ${fieName%.*}`;

#prints out the desired date format..a for day of the week, d for day of the month, b for month, Y for year, k for minute, M for hours, Z for time zone  
echo Compiled on `date +"%a %d %b %Y %k:%M %Z"`
