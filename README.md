Introduction
============

WIP utils regarding C. REquires P99 to work. 

Known Hard Issues
============

 - container api should be converted in x-macros.
 - macro type annotations sjhould be everywhere;
 - sprint signature should change.

Installation
============

```
git clone <project_root>
cd project_root
cd P99
mkdir -p build/Debug
cd build/Debug
cmake ../..
make && sudo make install && sudo ldconfig
cd ../../../
cd CUtils
mkdir -pv build/Debug
cd build/Debug
cmake ../..
# perform c-utils tests
./Test
make && sudo make install && sudo ldconfig
```

Uninstall
=========

```
cd project_root
cd P99/build/Debug
sudo make uninstall
cd ../../../
cd CUtils/build/Debug
sudo make uninstall
```


