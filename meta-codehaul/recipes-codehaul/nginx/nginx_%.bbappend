do_install:append() {
    rm ${D}${NGINX_WWWDIR}/html/index.html
    chown -R root:root ${D}${NGINX_WWWDIR}
}
