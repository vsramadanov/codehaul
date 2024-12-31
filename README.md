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
After the SD card device name have been found, find the image and flush it:
```bash
ls -lh sdimage-*.direct
sudo dd if=sdimage-your-name.direct of=/dev/your-sd-card
```

### Fix SD card image

(TODO: add these changes to yocto -- fixed, that paragraph is outdated)
To turn on serial console and boot log, do the following steps:
- find PARTUUID
```
blkid | grep mmcblk0p2
```
- Replace the /boot/cmdline.txt
```
dwc_otg.lpm_enable=0 console=serial0,115200 console=tty1 root=PARTUUID=a0cd4d16-02 rootfstype=ext4 elevator=deadline fsck.repair=yes rootwait
```
- Add to /boot/config.txt
```
enable_uart=1
```

## Raspberry Pi3b+ steps

### UART layout

| Board       | Wire        |
|-------------|-------------|
| Pin 6 (GND) | Black (GND) |
| Pin 8 (TX)  | White (RX)  |
| Pin 10 (RX) | Green (TX)  |

for more information see the [link](https://pinout.xyz/)

### connect to Wifi

