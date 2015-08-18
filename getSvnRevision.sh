#!/bin/bash
# $Id$

# Shell script for extracting subversion revision number and writing to specified header define file in specified folder.
# If folder is missing no file is written
# Author: Peter Nordin peter.nordin@liu.se
# Date:   2011-10-26
# For use in Hopsan, requires "subversion commandline" installed (apt-get install subversion)

foldername="$1"
filename="$2"
definename="$3"

writeFile()
{
# Args:
# 1 = Filepath
# 2 = DEFINENAME
# 3 = Revision number
  echo "//This file has been automatically generated by getSvnRevision.sh" > $1
  echo "// \$Id" >> $1
  hguard="$2"_H_INCLUDED
  echo "#ifndef $hguard" >> $1
  echo "#define $hguard" >> $1
  if [ -z "$3" ]; then
    echo echo "#define $2 UNKNOWN" >> $1
  else
    echo "#define $2 $3" >> $1
  fi
  echo "#endif" >> $1
}

# Get revision number from last change
if [ -x /usr/bin/svn ]; then
  rev=`svn info | grep "Last Changed Rev:" | cut -d" " -f4`
else
  rev=""
fi

#Check if dir exist, if so write the file
if [ -d "$foldername" ]; then
  writeFile "$foldername/$filename" $definename $rev
fi 

# Determine what to report and what exit code to give
if [ -z "$rev" ]; then
  echo "RevisionInformationNotFound"
  exit 1
else 
  echo $rev
  exit 0
fi

