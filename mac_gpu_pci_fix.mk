#
# pcie_fix.mak
#

TARGETNAME=pcie_fix
TARGETTYPE=DRIVER
TARGETPATH=obj

DRIVERTYPE=WDM

INCLUDES=$(WDK_INC_PATH)

SOURCES=pcie_fix.c

TARGETLIBS=$(SDK_LIB_PATH)\ntoskrnl.lib \
           $(SDK_LIB_PATH)\hal.lib \
           $(SDK_LIB_PATH)\wdmsec.lib