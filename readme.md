# Getting sources

`git clone --recursive https://github.com/lofcek/on-semi.git`

## Building

Please install ninja. Then

```bash
    cd on-semi
    ninja
```

Building is executed by command `ninja`. Compiled binaries are stored in directory `binaries` and temporary files (like *.o) are stored in directory `build`.

If you want use another compiler,let say `gcc` please change it in file `build.ninja`.
