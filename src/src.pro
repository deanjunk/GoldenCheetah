
TEMPLATE = app
TARGET = GoldenCheetah
DEPENDPATH += .
INCLUDEPATH += /usr/local/qwt/include /sw/include
CONFIG += static debug
QT += xml
LIBS += /usr/local/qwt/lib/libqwt.a 
LIBS += -lm -lz -lftd2xx
macx {
    LIBS += -framework Carbon
}

macx || unix {
    HEADERS += Serial.h
    SOURCES += Serial.cpp
    QMAKE_CXXFLAGS = -DGC_BUILD_DATE="`date +'\"%a_%b_%d,_%Y\"'`"
    RC_FILE = images/gc.icns
}

win32 { 
    LIBS = ..\lib\libregex.a \
        C:\qwt-5.1.0\lib\libqwtd5.a \
        -lws2_32 \
        C:\ftdi\libftd2xx.a
    QMAKE_LFLAGS = -Wl,--enable-runtime-pseudo-reloc \
        -Wl,--script,i386pe.x-no-rdata
    QMAKE_CXXFLAGS = -fdata-sections
    INCLUDEPATH += C:\qwt-5.1.0\include \
        C:\boost
}

HEADERS += \
	AllPlot.h \
	BestIntervalDialog.h \
	ChooseCyclistDialog.h \
	CpintPlot.h \
	CsvRideFile.h \
	DownloadRideDialog.h \
	MainWindow.h \
	PfPvPlot.h \
	PowerHist.h \
	RawRideFile.h \
	RideFile.h \
	RideItem.h \
	RideMetric.h \
	SrmRideFile.h \
	TcxParser.h \
	TcxRideFile.h \
	TimeUtils.h \
        ConfigDialog.h \
        D2XX.h \
        DatePickerDialog.h \
        Device.h \
        LogTimeScaleDraw.h \
        LogTimeScaleEngine.h \
        Pages.h \
        PowerTap.h \
        Zones.h \
        cpint.h \
        srm.h

SOURCES += \
	AllPlot.cpp \
	BestIntervalDialog.cpp \
	ChooseCyclistDialog.cpp \
	CpintPlot.cpp \
	CsvRideFile.cpp \
	DownloadRideDialog.cpp \
	MainWindow.cpp \
	PfPvPlot.cpp \
	PowerHist.cpp \
	RawRideFile.cpp \
	RideFile.cpp \
	RideItem.cpp \
	RideMetric.cpp \
	SrmRideFile.cpp \
	TcxParser.cpp \
	TcxRideFile.cpp \
	TimeUtils.cpp \
	cpint.cpp \
        BasicRideMetrics.cpp \
        BikeScore.cpp \
        ConfigDialog.cpp \
        D2XX.cpp \
        DatePickerDialog.cpp \
        Device.cpp \
        LogTimeScaleDraw.cpp \
        LogTimeScaleEngine.cpp \
        Pages.cpp \
        PowerTap.cpp \
        Zones.cpp \
	main.cpp \
        srm.cpp

RESOURCES = application.qrc
