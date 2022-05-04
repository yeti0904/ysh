# ysh
shell for Unix and Linux

## building
```sh
git clone https://github.com/MESYETI/ysh
cd ysh
cmake .
make
```

## compatibility
these platforms have been shown to be compatible with ysh:<br>
 - Linux
 - MacOS

## customisation
ysh creates a directory at ~/.config called ysh2, inside is a file called init.ysh, this file allows you to set the prompt, you can set the prompt like this:
```sh
set YSH_PROMPT "$ "
```

## features
ysh isn't finished but i'm working on useful features, here is a list of them

 - autocompletion (files AND commands)
 - multiple commands on one line, here's an example: `printf "hello "; printf "world\n"`