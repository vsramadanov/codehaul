SUMMARY = "Dispatches tasks between workers and IDP"
DESCRIPTION = ""
LICENSE = "CLOSED"

SRC_URI = "file://dispatcher.cpp \
           file://dispatcher.proto \
           file://makefile"

S = "${WORKDIR}"

DEPENDS = "fcgi grpc protobuf grpc-native protobuf-native"
RDEPENDS_${PN} += "libstdc++"

do_configure() {
    oe_runmake out/dispatcher.pb.cc
}
do_compile() {
    oe_runmake
}

do_install() {
    install -D -m 0755 dispatcher ${D}${bindir}/dispatcher
}

FILES:${PN} += "${bindir}/dispatcher"
