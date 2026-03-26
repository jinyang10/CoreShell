# My_Shell

OS Shell which support standard commands and allows execution of concurrent processes  

set command: supports up to 5 alphanumeric tokens as the STRING variable  
example execution: $ set x 20 bob alice toto xyz  
$ print x  
$ 20 bob alic toto xyz  
  
print command: print VAR  
run command: run SCRIPT  
echo command: simplified version that takes only one token string as input
example execution: $ echo $mary
   
example execution: set mary 123  
echo $mary    
123  
$  
  
my_ls command: lists all the files present in the current directory  
- if the current directory contains other directories, only displays the name (not contents) of the directory  
- the file/directory names are shown in alphabetical order  

one - liners are supported, where multiple commands can be accepted separated by semicolons  
example execution:  $ set x abc; set y 123; print y; print x;  
123  
abc  
$  

exec command: accepts up to 3 files as arguments (scripts), and a 4th argument which is a string representing the scheduling policy:    
exec prog1 prog2 prog3 POLICY  
where POLICY is always the last parameter of exec, and takes the values FCFS, SJF, RR, or AGING  
executes up to 3 scripts concurrently given a scheduling policy POLICY  

