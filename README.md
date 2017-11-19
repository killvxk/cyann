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

It is just a PoC, can (and will) be greatly improved. 
The most important part is the tiny python script who use capstone to patch with nanomite the original binary.
For the moment you can't pack heavy binary, the injection technique used (process hollowing) will fail if size(original_bin) > size(packed_bin), the virtalloc will fail to allocate sufficient memory in remote process. As said, it's just a tiny PoC. I will fix that by adding fake nanomite in the table.
