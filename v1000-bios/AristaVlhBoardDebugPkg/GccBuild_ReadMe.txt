This folder contains supported files for support building InsydeH2O 5.0 firmware source under Linux.
To setup the Linux build support environment, run 

. InstallLinuxBuildSupport

under the Linux terminal console.

The installation requires the user to be a "sudoer" so that required development tools and libraries 
can be installed onto the development system via Linux package installation tools such as "apt-get" 
or "yum", etc.

To build the source code, change to the project directory, for example, SharkBayMultiBordPkg, and run

make kernel

to build InsydeModulePkg source, or

make uefi64
or
make uefi64ddt

to build the project source in release or debug mode respectively.


Note:
Currently, the Linux build is supported for GCC 4.8.
On Ubuntu 18.04, use following commands to set default gcc version to 4.8:

1. Install gcc-4.8
sudo apt-get install gcc-4.8

2. Create gcc alternatives for gcc-4.8
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 48 --slave /usr/bin/gcc-ar gcc-ar /usr/bin/gcc-ar-4.8 --slave /usr/bin/gcc-nm gcc-nm /usr/bin/gcc-nm-4.8 --slave /usr/bin/gcc-ranlib gcc-ranlib /usr/bin/gcc-ranlib-4.8 --slave /usr/bin/gcov gcov /usr/bin/gcov-4.8

3. Create gcc alternatives for gcc-7 (the default gcc on Ubuntu 18.04)
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 70 --slave /usr/bin/gcc-ar gcc-ar /usr/bin/gcc-ar-7 --slave /usr/bin/gcc-nm gcc-nm /usr/bin/gcc-nm-7 --slave /usr/bin/gcc-ranlib gcc-ranlib /usr/bin/gcc-ranlib-7 --slave /usr/bin/gcov gcov /usr/bin/gcov-7 --slave /usr/bin/gcov-dump gcov-dump /usr/bin/gcov-dump-7 --slave /usr/bin/gcov-tool gcov-tool /usr/bin/gcov-tool-7

4. Set default gcc version to 4.8
sudo update-alternatives --set gcc /usr/bin/gcc-4.8

5. Check current default gcc version
gcc --version

Note2:
1. In ubuntu 18.04
2. "open in Termainal" in folder \BilbyBoardPkg\ (folder path cannot too long or cause build failed)
3. CMD: "su"
4. CMD: "make uefi64", or "make efidebug"