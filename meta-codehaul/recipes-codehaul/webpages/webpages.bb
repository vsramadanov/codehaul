SUMMARY = "Add html and other files for NGINX to share"
DESCRIPTION = ""
LICENSE = "CLOSED"

codemirror_version = "5.65.2"
NGINX_HTMLDIR ?= "${localstatedir}/www/localhost/html"

SRC_URI = " \
    https://cdnjs.cloudflare.com/ajax/libs/codemirror/${codemirror_version}/codemirror.min.css;name=1 \
    https://cdnjs.cloudflare.com/ajax/libs/codemirror/${codemirror_version}/codemirror.min.js;name=2  \
    https://cdnjs.cloudflare.com/ajax/libs/codemirror/${codemirror_version}/mode/clike/clike.min.js;name=3"

SRC_URI[1.sha256sum] = "11077112ab6955d29fe41085c62365c7d4a2f00a570c7475e2aec2a8cbc85fc4"
SRC_URI[2.sha256sum] = "1a76110fab03d295155ce6405b151a2a158bf8025678fdab055edf5acd8ea015"
SRC_URI[3.sha256sum] = "4fa83b2dc30095c175bf1ca999ce4dcaecd3b7f3a4757a24d59aebb22916bfb6"

SRC_URI += "file://index.html"

do_install(){
    install -D -m 644 ${WORKDIR}/index.html ${D}${NGINX_HTMLDIR}/index.html

    install -D -m 644 ${WORKDIR}/codemirror.min.css ${D}${NGINX_HTMLDIR}/codemirror/codemirror.min.css
    install -D -m 644 ${WORKDIR}/codemirror.min.js  ${D}${NGINX_HTMLDIR}/codemirror/codemirror.min.js
    install -D -m 644 ${WORKDIR}/clike.min.js       ${D}${NGINX_HTMLDIR}/codemirror/clike.min.js
}

FILES:${PN} += "${NGINX_HTMLDIR}/*"
