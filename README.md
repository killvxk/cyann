For packing a PE32 exec ==
- First nanomited the exec on linux:
```$ tools/mite.py /path/to/my.exe```
this will produce two file called /path/to/my.exe-mited and /path/to/my.exe-table.
- Second on Windows use the creatersrc.exe two compress both file and place them in th res/ dir with appropriate name:
```C:>.\tools\creatersrc.exe /path/to/my.exe-mited res/x32.res
C:>.\tools\creatersrc.exe /path/to/my.exe-table res/nm32.res```
TAKE CARE OF RESPECTING x32.res for the mited file and nm32.res for the table file!!!
- Third, on linux just compile and link everythink by:
```$ make```
