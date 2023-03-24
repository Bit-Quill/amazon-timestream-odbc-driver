#!/bin/bash

# Check if there is a change within one day
# This may not be delivered to AWS

lastCommit=$(git log -1 --format=%cd --date=unix)
curTime=$(date +%s)
gap=$(($curTime-$lastCommit))
echo "Now is $curTime, last commit is $lastCommit, gap is $gap"

if [ $gap -gt 86400 ]
then
  echo "No changes for more than one day, skip build and test"
else
  # success case must contain key word 'continue' which is checked in mac github workflow
  echo "There is a change within one day, continue build and test"
fi
