TEMPLATE = subdirs
DEFINES += QT_USE_QSTRINGBUILDER

SUBDIRS += \
    hsengine \
    hsdatabasegenerator \
    hsdemo

CONFIG(release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT
}
