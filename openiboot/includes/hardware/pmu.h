#ifndef HW_PMU_H
#define HW_PMU_H

#include "hardware/s5l8900.h"

#ifdef CONFIG_IPOD
#define PMU_I2C_BUS 1
#else
#define PMU_I2C_BUS 0
#endif

// Power
#define PMU_SETADDR	0xE6
#define PMU_GETADDR	0xE7
// Registers
#define PMU_MAXREG	0xF

// Values

#define PMU_GPIO_CHARGER_IDENTIFY_DN_IPHONE 0x1404
#define PMU_GPIO_CHARGER_IDENTIFY_DP_IPHONE 0x1405
#define PMU_GPIO_CHARGER_IDENTIFY_DN_IPOD 0x1603
#define PMU_GPIO_CHARGER_IDENTIFY_DP_IPOD 0x1604

#ifdef CONFIG_IPOD
#define PMU_GPIO_CHARGER_IDENTIFY_DN PMU_GPIO_CHARGER_IDENTIFY_DN_IPOD
#define PMU_GPIO_CHARGER_IDENTIFY_DP PMU_GPIO_CHARGER_IDENTIFY_DN_IPOD
#else
#define PMU_GPIO_CHARGER_IDENTIFY_DN PMU_GPIO_CHARGER_IDENTIFY_DN_IPHONE
#define PMU_GPIO_CHARGER_IDENTIFY_DP PMU_GPIO_CHARGER_IDENTIFY_DN_IPHONE
#endif

#define PMU_GPIO_CHARGER_USB_SUSPEND 0x1704
#define PMU_GPIO_CHARGER_SUSPEND 0x1706
#define PMU_GPIO_CHARGER_SHUTDOWN 0x1707
#define PMU_GPIO_CHARGER_USB_500_1000 0x1705
#define PMU_GPIO_CHARGER_USB_1000 0x704

#define PMU_OOCSHDWN 0xC
#define PMU_GPIOCTL 0x13
#define PMU_GPIO1CFG 0x14
#define PMU_ADCC3 0x52
#define PMU_ADCC2 0x53
#define PMU_ADCC1 0x54
#define PMU_ADCS3 0x57
#define PMU_ADCS1 0x55
#define PMU_MBCS1 0x4B
#define PMU_RTCSC 0x59
#define PMU_RTCMN 0x5A
#define PMU_RTCHR 0x5B
#define PMU_RTCWD 0x5C
#define PMU_RTCDT 0x5D
#define PMU_RTCMT 0x5E
#define PMU_RTCYR 0x5F

#define PMU_RTCSC_MASK 0x7F
#define PMU_RTCMN_MASK 0x7F
#define PMU_RTCHR_MASK 0x3F
#define PMU_RTCWD_MASK 0x07
#define PMU_RTCDT_MASK 0x3F
#define PMU_RTCMT_MASK 0x1F
#define PMU_RTCYR_MASK 0xFF

#define PMU_OOCSHDWN_GOSTBY (1 << 0)

#define PMU_MBCS1_ADAPTPRES (1 << 2)
#define PMU_MBCS1_USBOK (1 << 0)

#define PMU_ADCC1_ADCINMUX_SHIFT 4
#define PMU_ADCC1_ADCINMUX_MASK 0xF
#define PMU_ADCC1_ADC_AV_SHIFT 2
#define PMU_ADCC1_ADCSTART 0x1
#define PMU_ADCC1_ADCRES 0x2

#define PMU_ADCC1_ADC_AV_1 0x0
#define PMU_ADCC1_ADC_AV_4 0x1
#define PMU_ADCC1_ADC_AV_8 0x2
#define PMU_ADCC1_ADC_AV_16 0x3

#define PMU_ADCC1_ADCINMUX_BATSNS_DIV 0x0
#define PMU_ADCC1_ADCINMUX_BATSNS_SUB 0x1
#define PMU_ADCC1_ADCINMUX_ADCIN2_DIV 0x2
#define PMU_ADCC1_ADCINMUX_ADCIN2_SUB 0x3
#define PMU_ADCC1_ADCINMUX_BATTEMP 0x6
#define PMU_ADCC1_ADCINMUX_ADCIN1 0x7

#endif

