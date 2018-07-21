CC=arm-none-eabi-gcc
LD=arm-none-eabi-gcc
SIZE=arm-none-eabi-size
OBJCOPY=arm-none-eabi-objcopy
STFLASH=st-flash
OPENOCD=openocd
GDB=arm-none-eabi-gdb

#Pathes
OBJ_DIR=./obj
BIN_DIR=./bin
PROJ_ROOT=../../..
PERIPH_DIR=$(PROJ_ROOT)/bsp/stm32/stm32f05x/STM32F0xx_StdPeriph_Lib_V1.5.0
PERIPH_SRC_DIR=$(PERIPH_DIR)/Libraries/STM32F0xx_StdPeriph_Driver/src
PERIPH_CMSIS_DIR=$(PERIPH_DIR)/Libraries/CMSIS
BSP_DIR=$(PROJ_ROOT)/bsp/stm32/stm32f05x/ec_drive_v2_board

#Include pathes
INCLUDE+=-I$(PERIPH_DIR)/Libraries/STM32F0xx_StdPeriph_Driver/inc 
INCLUDE+=-I$(PERIPH_CMSIS_DIR)/Include
INCLUDE+=-I$(PERIPH_CMSIS_DIR)/Device/ST/STM32F0xx/Include
INCLUDE+=-I$(PROJ_ROOT)/bsp/stm32/stm32f05x/ec_drive_v2_board
INCLUDE+=-I$(BSP_DIR)
#INCLUDE+=-I$(STM32F10x_PERIPH_CMSIS_DIR)/DeviceSupport/ST/STM32F10x

#Source files
#Application source files
SRC_APP+=$(PROJ_ROOT)/app/main.c
VPATH+=$(PROJ_ROOT)/app

#BSP files
SRC_BSP+=$(BSP_DIR)/stm32f0xx_it.c
#SRC_BSP+=$(BSP_DIR)/system_stm32f0xx.c
VPATH+=$(BSP_DIR)

#Peripheral library files
SRC_PERIPH+=$(PERIPH_CMSIS_DIR)Device/ST/STM32F0xx/Source/Templates/system_stm32f0xx.c
SRC_PERIPH+=$(PERIPH_SRC_DIR)/stm32f0xx_adc.c
SRC_PERIPH+=$(PERIPH_SRC_DIR)/stm32f0xx_comp.c
SRC_PERIPH+=$(PERIPH_SRC_DIR)/stm32f0xx_dac.c
SRC_PERIPH+=$(PERIPH_SRC_DIR)/stm32f0xx_exti.c
SRC_PERIPH+=$(PERIPH_SRC_DIR)/stm32f0xx_i2c.c
SRC_PERIPH+=$(PERIPH_SRC_DIR)/stm32f0xx_pwr.c
SRC_PERIPH+=$(PERIPH_SRC_DIR)/stm32f0xx_spi.c
SRC_PERIPH+=$(PERIPH_SRC_DIR)/stm32f0xx_usart.c
SRC_PERIPH+=$(PERIPH_SRC_DIR)/stm32f0xx_can.c
SRC_PERIPH+=$(PERIPH_SRC_DIR)/stm32f0xx_crc.c
SRC_PERIPH+=$(PERIPH_SRC_DIR)/stm32f0xx_dbgmcu.c
SRC_PERIPH+=$(PERIPH_SRC_DIR)/stm32f0xx_flash.c
SRC_PERIPH+=$(PERIPH_SRC_DIR)/stm32f0xx_iwdg.c
SRC_PERIPH+=$(PERIPH_SRC_DIR)/stm32f0xx_rcc.c
SRC_PERIPH+=$(PERIPH_SRC_DIR)/stm32f0xx_syscfg.c
SRC_PERIPH+=$(PERIPH_SRC_DIR)/stm32f0xx_wwdg.c
SRC_PERIPH+=$(PERIPH_SRC_DIR)/stm32f0xx_cec.c
SRC_PERIPH+=$(PERIPH_SRC_DIR)/stm32f0xx_crs.c
SRC_PERIPH+=$(PERIPH_SRC_DIR)/stm32f0xx_dma.c
SRC_PERIPH+=$(PERIPH_SRC_DIR)/stm32f0xx_gpio.c
SRC_PERIPH+=$(PERIPH_SRC_DIR)/stm32f0xx_misc.c
SRC_PERIPH+=$(PERIPH_SRC_DIR)/stm32f0xx_rtc.c
SRC_PERIPH+=$(PERIPH_SRC_DIR)/stm32f0xx_tim.c
VPATH+=$(PERIPH_SRC_DIR)/

SRC+=$(SRC_PERIPH)
SRC+=$(SRC_APP)
SRC+=$(SRC_BSP)

STARTUP_FILE=$(PERIPH_CMSIS_DIR)/Device/ST/STM32F0xx/Source/Templates/gcc_ride7/startup_stm32f051.s
STARTUPOBJ:=$(patsubst %.s, $(OBJ_DIR)/%.o, $(notdir $(STARTUP_FILE)))

OBJ=$(patsubst %.c, $(OBJ_DIR)/%.o, $(notdir $(SRC)))

CFLAGS      = -Wall \
-g \
-mcpu=cortex-m0 \
-mthumb \
-D__HEAP_SIZE=0x0000 \
-D__STACK_SIZE=0x0100 \
-mfloat-abi=soft \
-fno-strict-aliasing \
-fdata-sections \
-ffunction-sections \
-DSTM32F051 \
-DUSE_STDPERIPH_DRIVER \

LDFLAGS     = \
--specs=nosys.specs \
--specs=nano.specs \
-mcpu=cortex-m0 \
-mthumb \
-Wl,--defsym=__HEAP_SIZE=0x0000 \
-Wl,--defsym=__STACK_SIZE=0x0100 \
-mfloat-abi=soft \
-fno-strict-aliasing \
-fdata-sections \
-ffunction-sections  \
-Wl,--gc-sections \
-Wl,-Map=$(TARGET).map \
-Wl,-script="stm32f0xx.ld"

#Target settings
TARGET_NAME=ec_drive
TARGET_BINARY=$(TARGET_NAME).elf
TARGET_HEX=$(TARGET_NAME).hex
TARGET=$(BIN_DIR)/$(TARGET_BINARY)
HEX=$(BIN_DIR)/$(TARGET_HEX)

.PHONY: $(TARGET) clean upload debug erase check-syntax

all: $(TARGET)

$(TARGET): $(OBJ_DIR) $(OBJ) $(STARTUPOBJ) $(BIN_DIR)
	echo $(STARTUP_FILE)
	echo $(STARTUPOBJ)
	$(LD) $(LDFLAGS) -o $@ $(OBJ) $(STARTUPOBJ)
	$(SIZE) --format=berkeley $(TARGET) 

$(OBJ_DIR)/%.o: %.c
	$(CC) -c $(CFLAGS) $(INCLUDE) $< -o $@

$(STARTUPOBJ):
	$(CC) -c $(CFLAGS) $(INCLUDES) $(STARTUP_FILE) -o $(STARTUPOBJ)

$(HEX): $(TARGET)
	$(OBJCOPY) -O ihex $(TARGET) $(HEX)

upload: $(HEX)
	$(STFLASH) --format ihex write $(HEX)

erase:
	$(STFLASH) erase

debug: $(TARGET)
	$(OPENOCD) -f interface/stlink-v2-1.cfg -f target/stm32f0x.cfg &
	$(GDB) -ex "target remote localhost:3333" -ex "b main" -ex "monitor reset halt" -ex "c" $(TARGET)
	pkill $(OPENOCD)

check-syntax:
	$(CC) -o nul -S ${CHK_SOURCES}

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(BIN_DIR):
	mkdir $(BIN_DIR)

clean: $(OBJ_DIR) $(BIN_DIR)
	rm -rf obj bin
