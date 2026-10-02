[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/eZm3ZHfi)
# backup-restore-script

-- WRITE HERE --
#  Lab 2 – Backup and Restore Automation

## Overview
This project implements an automated **Backup and Restore System** using Bash scripts.
It allows a user to:
- Automatically create backups of a chosen directory every few seconds.
- Keep only a fixed number of recent backups (oldest ones are deleted automatically).
- Restore the directory to any previous or next backup version using a simple menu.

### Folder Structure
LAB2/
├── DIR/ # Folder to be backed up (source)
├── backupdir/ # Folder where all backups are stored
├── backupd.sh # Script for automated periodic backups
├── restore.sh # Script for restoring previous or next versions
├── Makefile # Automates backup, restore, and clean commands
└── README.md # Documentation file (this file)


### Scripts Description
- **backupd.sh**
  Runs continuously, creating backups of `DIR/` every given interval (e.g., every 5 seconds).
  It keeps a maximum number of backups — once the limit is reached, the oldest backup is removed.

- **restore.sh**
  Finds which backup version matches the current directory and allows you to restore either the previous or next version.

- **Makefile** 
  Simplifies running the scripts with short commands:
  - `make backup` → starts the backup process 
  - `make restore` → opens the restore menu 
  - `make clean` → removes all backups and temporary files

---

## Prerequisites
This project was created and tested on **Ubuntu/Linux** using Bash.

Make sure your system has:
- **bash**
- **make**
- **coreutils** (includes `ls`, `cp`, `rm`, `diff`)

If `make` is not installed, install it using:
```bash 
sudo apt update
sudo apt install make


## How to Run
1️⃣ Create the source directory

Create a folder named DIR that contains the files you want to back up:

mkdir DIR
echo "Hello world" > DIR/test.txt

2️⃣ Start the backup process

Run:
make backup

This will:

Create the backup directory (backupdir).

Start creating backups of DIR every few seconds.

Automatically keep only the latest few backups (based on MAX_BACKUPS in the Makefile).

3️⃣ Restore previous or next backup

After backups are created, run:

make restore

You’ll see a menu:

Choose an option:
1 - Restore to previous version
2 - Restore to next version
3 - Exit


Enter 1 to go to the previous backup version.

Enter 2 to move forward to a newer version.

Enter 3 to exit the tool.

4️⃣ Clean all backups

To remove all backup folders and temporary info files:
make clean

##Example Run (max-backups = 3)
Part 1 – Backup
$ make backup
Backup directory is ready: ./backupdir
Backup created at ./backupdir/2025-10-10-02-43-31
Backup created at ./backupdir/2025-10-10-02-43-46
Backup created at ./backupdir/2025-10-10-02-44-06

Part 2 – Restore
$ make restore
✅ Matched current directory to backup version: 2025-10-10-02-44-06
Choose an option:
1 - Restore to previous version
2 - Restore to next version
3 - Exit
Enter your choice: 1
Restored to previous version: 2025-10-10-02-43-46

💡 Notes

-Each backup folder is named using the current date and time (e.g., 2025-10-10-02-43-31).

-The number of backups kept is controlled by the variable MAX_BACKUPS in the Makefile.

-You can adjust the time interval between backups by changing the INTERVAL variable in the Makefile.

-The project uses only basic Bash commands — no external libraries required.


Name: Salma
Course: Operating Systems – Lab 2
Project: Backup and Restore Automation
