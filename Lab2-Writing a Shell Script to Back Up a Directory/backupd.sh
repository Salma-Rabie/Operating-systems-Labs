#!/bin/bash

if [ $# -ne 4 ]; then
  echo "Usage: $0 <dir> <backupdir> <interval-secs> <max-backups>"
  exit 1
fi

dir=$1
backupdir=$2
interval=$3
max_backups=$4

if ! [[ "$interval" =~ ^[0-9]+$ ]] || ! [[ "$max_backups" =~ ^[0-9]+$ ]]; then
  echo "Error: interval-secs and max-backups must be integers."
  exit 1
fi



if [ ! -d "$dir" ]; then
  echo "Error: Source directory '$dir' does not exist."
  exit 1
fi

mkdir -p "$backupdir"




  ls -lR "$dir" > directory-info.last
  timestamp=$(date '+%Y-%m-%d-%H-%M-%S')
  cp -a "$dir" "$backupdir/$timestamp"



while true
do
  sleep "$interval"
  ls -lR "$dir" > directory-info.new

if ! diff directory-info.last directory-info.new > /dev/null; then
    timestamp=$(date +"%Y-%m-%d-%H-%M-%S")
    cp -a "$dir" "$backupdir/$timestamp"
    cp directory-info.new directory-info.last

    #currentbackups=$(ls "$backupdir" | wc -l)
   currentbackups=$(find "$backupdir" -mindepth 1 -maxdepth 1 -type d | wc -l)

     if [ "$currentbackups" -gt "$max_backups" ]; then
      oldest=$(ls "$backupdir" | sort | head -n 1)
      rm -rf "$backupdir/$oldest"
    fi
  fi
done
