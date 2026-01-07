> mem_free.log
while true 
do
#From the file /proc/meminfo, select the lines containing the substring " MemFree"
#Use the comand date with the appropriate flags to return the current time and date
#Pipe to tee with the flag -a in order to append the output to the log and simultanously write to the stdin
echo `grep MemFree /proc/meminfo` : `date +%d/%m/%Y" "%H:%M:%S` | tee -a mem_free.log
sleep 5
done
