SOURCEDIR := DIR
BACKUPDIR := backupdir
INTERVAL := 5
MAX_BACKUPS := 3

all: backup

backup:
	mkdir -p $(BACKUPDIR)
	bash backupd.sh $(SOURCEDIR) $(BACKUPDIR) $(INTERVAL) $(MAX_BACKUPS)

restore:
	bash restore.sh $(SOURCEDIR) $(BACKUPDIR)

clean:
	rm -rf $(BACKUPDIR) directory-info.*

