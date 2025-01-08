require recipes-core/images/core-image-base.bb

IMAGE_INSTALL:append = " systemd libstdc++ mtd-utils" 
IMAGE_INSTALL:append = " openssh openssl openssh-sftp-server" 
IMAGE_INSTALL:append = " packagegroup-core-buildessential"
IMAGE_INSTALL:append = " nginx fcgi grpc"
IMAGE_INSTALL:append = " webpages dispatcher"
