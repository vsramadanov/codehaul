require recipes-core/images/core-image-base.bb

IMAGE_INSTALL:append = " systemd libstdc++ mtd-utils" 
IMAGE_INSTALL:append = " openssh openssl openssh-sftp-server" 
IMAGE_INSTALL:append = " nginx"
