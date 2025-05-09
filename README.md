# Multi-User-Dungeon

## Specification
* A level for each team member
* 10 rooms each level
* All the levels must be able to seamelessly traversible
* Each level have an item
* If the player get an item, they win, regardless of level.

## Controls
* Player traverse with 4 buttons that take them north, south, east, west.

## Compile server
### in /server
#### with gcc
```
gcc *.c -o game.exe
```

#### with Makefile
Windows
```
mingw32-make
```

Linux
```
make
```