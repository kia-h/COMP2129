#! /bin/bash

###################
# Find repositories
# author: Internet's own boy
###################

#loop through the curent directory and find folders with '.hg. in them
for i in $(find . -type d -name '*.hg' ); 
do

	#remove the part with '.hg' and prints out the result(s)
	j="${i%/*}";
	echo $j;
done;
