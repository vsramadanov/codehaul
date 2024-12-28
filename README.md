# CODEHAUL PROJECT


## BUILD & DEPLOY 

### Build the project

```bash
. poky/oe-init-build-env 
bitbake codehaul-image
```

### Create a SD card image

Build the `wic-tools` (if haven't built yet) and use it to create a SD card image
```bash
bitbake wic-tools
cd path/to/image
wic create sdimage-raspberrypi -e codehaul-image
```
Than flash the image to the SD card. Use `lsblk` to find `/dev/your-sd-card` device
```bash
cd path/to/image
sudo dd if=sdimage-raspberrypi-202412282210-mmcblk0.direct of=/dev/mmcblk0
```