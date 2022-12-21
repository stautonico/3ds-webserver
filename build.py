#!/usr/bin/env python3
import os
import sys
import shutil


def make_build_dir():
    if not os.path.exists("build"):
        os.mkdir("build")

    os.chdir("build")


def main():
    make_build_dir()

    last_build = None
    if os.path.exists("lastbuild"):
        with open("lastbuild", "r") as f:
            last_build = f.read().strip()

    if len(sys.argv) > 1:
        current_build = sys.argv[1].upper()
    else:
        current_build = "LINUX"

    if last_build != current_build:
        os.chdir("..")
        shutil.rmtree("build")
        make_build_dir()

    if current_build == "3DS":
        command = "cmake .. -DCMAKE_TOOLCHAIN_FILE=../DevkitArm3DS.cmake -DTARGET_PLATFORM=3DS"
    else:
        command = "cmake .. -DTARGET_PLATFORM=LINUX "

    os.system(command)
    os.system("make")

    with open("lastbuild", "w") as f:
        f.write(current_build)


if __name__ == "__main__":
    main()
