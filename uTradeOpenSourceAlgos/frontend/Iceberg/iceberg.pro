#-------------------------------------------------
#
# Project created by QtCreator 2018-09-14T18:00:17
#
#-------------------------------------------------

QT       += widgets sql

TARGET = libIceberg
TEMPLATE = lib

DEFINES += ICEBERG_LIBRARY
DEFINES += FRONTEND_COMPILATION
INCLUDEPATH += ../../api2/api_includes/
SOURCES += iceberg.cpp \
    icebergui.cpp  \
    ../../FrontEnd_Api_Includes/dll_includes/apiincludesgetters.cpp \
    icebergparams.cpp

HEADERS += iceberg.h\
        iceberg_global.h \
    icebergui.h \
    ../../api2/api_includes/userParamsReader.h \
    ../../api2/api_includes/sharedUtilities.h \
    ../../api2/api_includes/sharedSingleOrder.h \
    ../../api2/api_includes/sharedResponse.h \
    ../../api2/api_includes/sharedDefines.h \
    ../../api2/api_includes/sharedCommands.h \
    ../../api2/api_includes/sgSymbolDataDefines.h \
    ../../api2/api_includes/sgOrderBookDefines.h \
    ../../api2/api_includes/sgMktData.h \
    ../../api2/api_includes/sgDebugLogDefines.h \
    ../../api2/api_includes/sgContext.h \
    ../../api2/api_includes/sgCommon.h \
    ../../api2/api_includes/sgApiParameters.h \
    ../../api2/api_includes/serializeCommands.h \
    ../../api2/api_includes/cmdDefines.h \
    ../../api2/api_includes/baseCommands.h \
    ../../api2/api_includes/apiStructs.h \
    ../../api2/api_includes/apiDataTypes.h \
    ../../api2/api_includes/apiConstants.h \
    ../../api2/api_includes/api2UserCommands.h \
    ../../api2/api_includes/api2RequestResponse.h \
    ../../api2/api_includes/api2Exceptions.h \
    ../../FrontEnd_Api_Includes/dll_includes/StrategyConstants.h \
    ../../FrontEnd_Api_Includes/dll_includes/sharedStrutures.h \
    ../../FrontEnd_Api_Includes/dll_includes/sharedParams.h \
    ../../FrontEnd_Api_Includes/dll_includes/sharedEnums.h \
    ../../FrontEnd_Api_Includes/dll_includes/baseStrategyWrapper.h \
    ../../FrontEnd_Api_Includes/dll_includes/BaseHandler.h \
    ../../FrontEnd_Api_Includes/dll_includes/dllDefines.h \
    icebergparams.h


symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xECEA4510
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = iceberg.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    icebergui.ui
