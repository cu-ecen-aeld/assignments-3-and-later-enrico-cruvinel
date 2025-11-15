#!/bin/sh

# Accepts the following runtime arguments: 
# the first argument is a path to a directory on the filesystem (filesdir);
# the second argument is a text string which will be searched within these files (searchstr)

# Exits with return value 1 error and print statements if any of the parameters above were not specified
if [ $# -eq 0 ]; then 
    echo "Directory path and search string not specified."
    exit 1
elif [ $# -eq 1 ]; then
    echo "Search string not specified."
    exit 1
elif [ $# -gt 2 ]; then
    echo "Extra parameters specified."
    exit 1
fi

filesdir=$1
searchstr=$2

# Exits with return value 1 error and print statements if filesdir does not represent a directory on the filesystem
if [ ! -d ${filesdir} ]; then
    echo "Directory path does not repesent a directory on the filesystem"
    exit 1
fi 

# Prints a message "The number of files are X and the number of matching lines are Y" 

# find files in filesdir, exclude directories
# reference: https://www.man7.org/linux/man-pages/man1/find.1.html
files=$(find ${filesdir} -type f)
no_files=$(echo ${files} | wc -w)
no_matches=$(grep -l ${searchstr} ${files}| wc -w)

echo "The number of files are ${no_files} and the number of matching lines are ${no_matches}"
