#! /bin/bash
source /opt/ros/noetic/setup.bash
if [ $# -ne 4 ]; then                #making sure all paramters were given
  echo "Incorrect Paramters!!!"
  exit 1
fi
#Variaables
dir_src=$1
backupdir=$2
secs=$3
max=$4
#checking everythin was read correctly
echo "Source: $dir_src and backup: $backupdir and sec: $secs and max: $max"
echo #for adding new line


echo "Creating initial backup"
echo
current_date=$(date +"%Y-%m-%d %T")
backup_name="$backupdir/$current_date"
cp -r "$dir_src" "$backup_name" #copy the src into dest for backup

#Infinite loop
while true; do
  ls -lR "$dir_src" > ls-lR.last
  sleep $secs
  ls -lR "$dir_src" > ls-lR.new 
  echo "Comparing 2 files"
  echo "...."
  echo

  if cmp -s -- ls-lR.last ls-lR.new ;then
   echo "No changes!"
   echo
  else
   echo "Creating new backup file"
   echo
   current_date=$(date +"%Y-%m-%d %T")
   backup_name="$backupdir/$current_date"
   cp -r "$dir_src" "$backup_name"
   
#calculating number of backup files
   cd $backupdir
   num_backups="$(ls -1 | wc -l)"
   echo "Number of files = $num_backups"
   echo 
   if [ $num_backups -gt $max ]; then
      echo "Removing 1 file"
      echo
      rm -r -- "$(ls -rt | head -n 1)"      
   fi
   cd ..
  fi
done
