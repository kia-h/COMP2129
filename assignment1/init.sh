#! /bin/bash

###################
# Initialise
# author: Internet's own boy
###################

#variable to hold the given file or folder name
fileName=$1;

#prints out the appropriate message if file or folder exists otherwise make the directories
if test -f ./$fileName; then
	echo There is already a file called $fileName;
	exit;
fi;

if test -d ./$fileName ; then
	echo There is already a folder for that subject
else 
	mkdir ./$fileName;
	mkdir ./$fileName/week{1..13}	
fi;
	
