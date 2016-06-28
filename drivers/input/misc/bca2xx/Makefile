#
# Makefile for miscellaneous I2C chip drivers.
#
# Do not add new drivers to this directory! It is DEPRECATED.
#
# Device drivers are better grouped according to the functionality they
# implement rather than to the bus they are connected to. In particular:
# * Hardware monitoring chip drivers go to drivers/hwmon
# * RTC chip drivers go to drivers/rtc
# * I/O expander drivers go to drivers/gpio
#

obj-y	:= bstclass.o

obj-$(CONFIG_SENSORS_BMA2X2)	+= bma2x2.o bst_sensor_common.o

EXTRA_CFLAGS += -DBMA2X2_SENSOR_IDENTIFICATION_ENABLE

ifeq ($(CONFIG_SENSORS_BMA2X2_ENABLE_INT1),y)
EXTRA_CFLAGS += -DBMA2X2_ENABLE_INT1
endif

ifeq ($(CONFIG_BOSCH_BMA2X2_ENABLE_INT2),y)
EXTRA_CFLAGS += -DBMA2X2_ENABLE_INT2
endif
