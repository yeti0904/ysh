# ysh
shell for Unix and Linux

## building
```sh
git clone https://github.com/MESYETI/ysh
cd ysh
cmake .
make
```
<br>
these platforms have been shown to be compatible with ysh:<br>
- Linux

## customisation
ysh creates a directory at ~/.config called ysh2, inside is a file called init.ysh, this file allows you to set the prompt, you can set the prompt like this:
```sh
set YSH_PROMPT "$ "
```