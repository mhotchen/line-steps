# List line numbers

An example implementation of my proposed line numbering system named "list line numbers".

Article: http://dividebyze.ro/2016/08/01/an-experiment-in-line-numbers.html

To use, clone the repo, `cd` in to the directory and run:

```
cmake .
make
./line_steps main.c
```

And change main.cpp to any file you want to run it against.

Note the following assumptions:

* You never have a line longer than 512 characters (it will get cut off)
* Tabs are 4 spaces wide
* You don't mind loading the whole file in to memory
