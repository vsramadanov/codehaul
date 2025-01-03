SUMMARY = "Dispatches tasks between workers and IDP"
DESCRIPTION = ""
LICENSE = "CLOSED"

SRC_URI = "file://dispatcher.cpp \
           file://makefile"

S = "${WORKDIR}"

DEPENDS = "fcgi"

do_compile() {
    oe_runmake CPPFLAGS="${CPPFLAGS}"
}

do_install() {
    install -D -m 0755 dispatcher ${D}${bindir}/dispatcher
}

FILES:${PN} += "${bindir}/dispatcher"
