
# 3ds-webserver

A small webserver that runs on 3DS and linux.

## Dependencies
- CMake (build tool)
- python (custom build script to make stuff easier)
- [devkitPro](https://devkitpro.org/wiki/Main_Page)
## Build Instructions
- Clone the repo

**to build for linux**
- run `python build.py`

**to build for 3ds**
- run `python build.py 3ds`

The binary will be put in `build/`. For linux, you can
simply run `./build/3ds-webserver`. For 3ds, you can copy
the `3ds-webserver.3dsx` into the `3ds` folder in the root of your SD card.
Finally, launch from homebrew.

## Usage
By default, the webserver listens on all interfaces on port 8000. It serves
files from a directory named `3ds-webserver-files/` in the same directory as the binary.
Your folder structure should look like:
```
.
├── 3ds-webserver
└── 3ds-webserver-files
    ├── about.html
    └── index.html
```

On 3ds, it should look like:
```
3ds
├── [other 3dsx apps...]
├── 3ds-webserver.3dsx
└── 3ds-webserver-files
    ├── about.html
    └── index.html
```
## Planned Features

- [ ] Proper cache support
- [ ] Options configurable through a config file
- [ ] SSL support
- [ ] CIA support
