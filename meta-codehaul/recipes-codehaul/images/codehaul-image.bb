include recipes-core/images/core-image-minimal.bb 

IMAGE_INSTALL:append = " libstdc++ mtd-utils" 
IMAGE_INSTALL:append = " openssh openssl openssh-sftp-server" 
