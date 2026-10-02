#!/bin/bash
if [ $# -ne 2 ]; then
  echo "Usage: $0 <dir> <backupdir>"
  exit 1
fi

dir=$1
backupdir=$2


if [ ! -d "$backupdir" ]; then
  echo "Error: Backup directory '$backupdir' does not exist."
  exit 1
fi

backups=($(ls -1 "$backupdir" | sort))

for i in "${!backups[@]}"; do
  if diff -r "$dir" "$backupdir/${backups[$i]}" >/dev/null 2>&1; then
    current_index=$i
    break
  fi
done

if [ -z "${current_index+x}" ]; then
  echo "No matching backup found for current state."
  exit 0
fi


if [ -z "$current_index" ]; then
  echo "⚠️  Warning: current directory does not match any known backup."
  current_index=0
else
  echo "✅ Matched current directory to backup version: ${backups[$current_index]}"
fi


current_backup="${backups[$current_index]}"

while true; do
  echo ""
  echo "Choose an option:"
  echo "1 - Restore to previous version"
  echo "2 - Restore to next version"
  echo "3 - Exit"
  read -p "Enter your choice: " choice

  case $choice in
    1)
      if [ $current_index -gt 0 ]; then
        current_index=$((current_index - 1))
        current_backup="${backups[$current_index]}"
        rm -rf "$dir"
        cp -r "$backupdir/$current_backup" "$dir"
      echo "Restored to a previous version: $current_backup"
      else
        echo "No older backup available to restore."
      fi
      ;;
    2)
      if [ $current_index -lt $((${#backups[@]} - 1)) ]; then
        current_index=$((current_index + 1))
        current_backup="${backups[$current_index]}"
        rm -rf "$dir"
        cp -r "$backupdir/$current_backup" "$dir"
       echo "Restored to a next version: $current_backup"
      else
        echo "No newer backup available to restore."
      fi
      ;;
    3)
      echo "Exiting restore tool."
      break
      ;;
    *)
      echo "Invalid option. Please try again."
      ;;
  esac
done
