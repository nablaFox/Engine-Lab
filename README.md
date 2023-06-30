# Engine Lab

A simple Vulkan game engine. It's junk, but useful (for me) to learn a lot of things.  
Currently based on this [tutorial](https://youtube.com/playlist?list=PL8327DO66nu9qYVKLDmdLW_84-yE4auCR).

## Unix Build

Install the following packages:

```sh
sudo apt install vulkan-tools
sudo apt install libvulkan-dev
sudo apt install vulkan-validationlayers-dev spirv-tools
sudo apt install libxxf86vm-dev libxi-dev
```

Then run:

```sh
git clone --recurse-submodules https://github.com/nablaFox/Engine-Lab.git
cd Engine-Lab
make
```