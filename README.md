```
______________.___.  _____    _______    _______   
\_   ___ \__  |   | /  _  \   \      \   \      \  
/    \  \//   |   |/  /_\  \  /   |   \  /   |   \ 
\     \___\____   /    |    \/    |    \/    |    \
 \______  / ______\____|__  /\____|__  /\____|__  /
        \/\/              \/         \/         \/ 
```
###      PoC Nanomite packer for PE32/PE32++ 

README will be updated with better explanation about how it work. Instead of waiting you may directly look at the code :)

Under Linux cd to the cyann dir, then ```./pack /path/to/my.exe```

Dependances:
- mingw for linux (i686 and x86_64)  
- python2 with capstone and pefile module  

Is it just a PoC, can (and will) be greatly improved. 
It will patch all Jcc and call instructions in the target binary with breakpoint and then the first stage use a common technique called Process hollowing to make a driven process who will be debugged in order to execute all mited instruction at runtime.
It's the first stage who will emulate the mited instruction by using windows API function like GetThreadContext and SetThreadContext
For the moment it will not work with large executable, because it will fail to allocate sufficient space in remote process.
