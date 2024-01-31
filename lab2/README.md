# Backup Bash Script

Backup.sh is a bash script for creating continuous backup files whenever a specific file or directory is modified and it updates automatically, while maintaining a maximum number of backup files to save space.

## Installation

You need to install: 
Linux system and bash as a default shell.

It assumes that the backup directory already exists.

'makefile' creates a new backup directory file if there's none that exists.

## Usage

To use this script you need to run it in a ubuntu (or regular) terminal,
that's using the command:
	bash backup.sh <directory_source> <backup_directory> <interval_secs> <max_backups>
while passing the right arguments of course.

Parameters:
<directory_source>: the source directory that have list of file we need to backup.
<backup_directory>: the destination directory that will have the backup
<interval_secs>: time to wait between every check
<max_backups>: maximum number of backups needs to be reserved

New backup files created will be named by the name of creation in the following format:
YYYY-MM-DD-hh-mm-ss


## Mechanism

1) The script creates an initial backup file, immediately after running.
2) Then it enters an infinite loop of checking for updates.
3) It compares snapshots of the directory.
4) When a modification is detected, a new backup file is created with the time stamp as a name.
5) After that the script checks if the condition of maximum number of files isn't violated.
6) If the number of files exceeded the maximum number required, the oldest file is deleted.


## Features

Automatic : Doesn't need manual checking , only the availability of destination and source files.

Space efficient: It continuously maintains a maximum number of backup files created and deletes older ones.

Time Labeled: It makes it easier to know which file was created last and identify the last update that occurred to the file.

## Contributing

Pull requests are welcome. For major changes, please open an issue first
to discuss what you would like to change.

Please make sure to update tests as appropriate.

For more in detail understanding or for modifications, please refer to the source code of the bash script.
