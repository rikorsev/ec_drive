CC=arm-none-eabi-gcc
LD=arm-none-eabi-gcc
SIZE=arm-none-eabi-size
OBJCOPY=arm-none-eabi-objcopy
STFLASH=st-flash
OPENOCD=openocd
GDB=arm-none-eabi-gdb

OBJ_DIR=./obj
BIN_DIR=./bin
TMP_DIR=./tmp

TARGET_NAME=ec_drive
TARGET_BINARY=$(TARGET_NAME).elf
TARGET_HEX=$(TARGET_NAME).hex
TARGET=$(BIN_DIR)/$(TARGET_BINARY)
HEX=$(BIN_DIR)/$(TARGET_HEX)

#Pathes
PROJ_ROOT=../..
STM32F10x_PERIPH_DIR=$(PROJ_ROOT)/bsp/stm32/stm32f10x/STM32F10x_StdPeriph_Lib_V3.5.0
STM32F10x_PERIPH_SRC_DIR=$(STM32F10x_PERIPH_DIR)/Libraries/STM32F10x_StdPeriph_Driver/src
STM32F10x_PERIPH_CMSIS_DIR=$(STM32F10x_PERIPH_DIR)/Libraries/CMSIS/CM3
STM32F10x_PERIPH_GPIO_TOGGLE_EXANPLE_DIR=$(STM32F10x_PERIPH_DIR)/Project/STM32F10x_StdPeriph_Examples/GPIO/IOToggle
STM32F10x_PERIPH_EVAL_DIR=$(STM32F10x_PERIPH_DIR)/Utilities/STM32_EVAL

#Include pathes
INCLUDE+=-I$(STM32F10x_PERIPH_DIR)/Libraries/STM32F10x_StdPeriph_Driver/inc 
INCLUDE+=-I$(STM32F10x_PERIPH_CMSIS_DIR)/CoreSupport 
INCLUDE+=-I$(STM32F10x_PERIPH_CMSIS_DIR)/DeviceSupport/ST/STM32F10x
INCLUDE+=-I$(STM32F10x_PERIPH_GPIO_TOGGLE_EXANPLE_DIR)
INCLUDE+=-I$(STM32F10x_PERIPH_EVAL_DIR)

#Source files
#Application source files
#SRC_APP+=$(STM32F10x_PERIPH_GPIO_TOGGLE_EXANPLE_DIR)/main.c
SRC_APP+=$(PROJ_ROOT)/app/main.c
SRC_APP+=$(STM32F10x_PERIPH_GPIO_TOGGLE_EXANPLE_DIR)/stm32f10x_it.c
SRC_APP+=$(STM32F10x_PERIPH_EVAL_DIR)/stm32_eval.c

VPATH+=$(PROJ_ROOT)/app
VPATH+=$(STM32F10x_PERIPH_GPIO_TOGGLE_EXANPLE_DIR)
VPATH+=$(STM32F10x_PERIPH_EVAL_DIR)

#Peripheral library files
SRC_PERIPH+=$(STM32F10x_PERIPH_CMSIS_DIR)/CoreSupport/core_cm3.c
SRC_PERIPH+=$(STM32F10x_PERIPH_CMSIS_DIR)/DeviceSupport/ST/STM32F10x/system_stm32f10x.c
SRC_PERIPH+=$(STM32F10x_PERIPH_SRC_DIR)/misc.c
SRC_PERIPH+=$(STM32F10x_PERIPH_SRC_DIR)/stm32f10x_can.c
SRC_PERIPH+=$(STM32F10x_PERIPH_SRC_DIR)/stm32f10x_dac.c
SRC_PERIPH+=$(STM32F10x_PERIPH_SRC_DIR)/stm32f10x_exti.c
SRC_PERIPH+=$(STM32F10x_PERIPH_SRC_DIR)/stm32f10x_gpio.c
SRC_PERIPH+=$(STM32F10x_PERIPH_SRC_DIR)/stm32f10x_pwr.c
SRC_PERIPH+=$(STM32F10x_PERIPH_SRC_DIR)/stm32f10x_sdio.c
SRC_PERIPH+=$(STM32F10x_PERIPH_SRC_DIR)/stm32f10x_usart.c
SRC_PERIPH+=$(STM32F10x_PERIPH_SRC_DIR)/stm32f10x_adc.c
SRC_PERIPH+=$(STM32F10x_PERIPH_SRC_DIR)/stm32f10x_cec.c
SRC_PERIPH+=$(STM32F10x_PERIPH_SRC_DIR)/stm32f10x_dbgmcu.c
SRC_PERIPH+=$(STM32F10x_PERIPH_SRC_DIR)/stm32f10x_flash.c
SRC_PERIPH+=$(STM32F10x_PERIPH_SRC_DIR)/stm32f10x_i2c.c
SRC_PERIPH+=$(STM32F10x_PERIPH_SRC_DIR)/stm32f10x_rcc.c
SRC_PERIPH+=$(STM32F10x_PERIPH_SRC_DIR)/stm32f10x_spi.c
SRC_PERIPH+=$(STM32F10x_PERIPH_SRC_DIR)/stm32f10x_wwdg.c
SRC_PERIPH+=$(STM32F10x_PERIPH_SRC_DIR)/stm32f10x_bkp.c
SRC_PERIPH+=$(STM32F10x_PERIPH_SRC_DIR)/stm32f10x_crc.c
SRC_PERIPH+=$(STM32F10x_PERIPH_SRC_DIR)/stm32f10x_dma.c
SRC_PERIPH+=$(STM32F10x_PERIPH_SRC_DIR)/stm32f10x_fsmc.c
SRC_PERIPH+=$(STM32F10x_PERIPH_SRC_DIR)/stm32f10x_iwdg.c
SRC_PERIPH+=$(STM32F10x_PERIPH_SRC_DIR)/stm32f10x_rtc.c
SRC_PERIPH+=$(STM32F10x_PERIPH_SRC_DIR)/stm32f10x_tim.c

VPATH+=$(STM32F10x_PERIPH_CMSIS_DIR)/CoreSupport
VPATH+=$(STM32F10x_PERIPH_CMSIS_DIR)/DeviceSupport/ST/STM32F10x
VPATH+=$(STM32F10x_PERIPH_SRC_DIR)/

SRC+=$(SRC_PERIPH)
SRC+=$(SRC_APP)

STARTUP_FILE=$(STM32F10x_PERIPH_CMSIS_DIR)/DeviceSupport/ST/STM32F10x/startup/gcc_ride7/startup_stm32f10x_md.s
STARTUPOBJ:=$(patsubst %.s, $(OBJ_DIR)/%.o, $(notdir $(STARTUP_FILE)))

OBJ=$(patsubst %.c, $(OBJ_DIR)/%.o, $(notdir $(SRC)))

CFLAGS      = -Wall \
-g \
-mcpu=cortex-m3 \
-mthumb \
-D__HEAP_SIZE=0x0000 \
-D__STACK_SIZE=0x0100 \
-mfloat-abi=soft \
-fno-strict-aliasing \
-fdata-sections \
-ffunction-sections \
-DSTM32F103C8 \
-DSTM32F10X_MD \
-DUSE_STDPERIPH_DRIVER \
-DUSE_STM32100B_EVAL

LDFLAGS     = \
--specs=nosys.specs \
--specs=nano.specs \
-mcpu=cortex-m3 \
-mthumb \
-Wl,--defsym=__HEAP_SIZE=0x0000 \
-Wl,--defsym=__STACK_SIZE=0x0100 \
-mfloat-abi=soft \
-fno-strict-aliasing \
-fdata-sections \
-ffunction-sections  \
-Wl,--gc-sections \
-Wl,-Map=$(TARGET).map \
-Wl,-script="stm32f103rb_flash.ld"

.PHONY: $(TARGET) clean upload debug

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

debug: $(TARGET)
	openocd -f interface/stlink-v2-1.cfg -f target/stm32f1x.cfg &
	$(GDB) -ex "target remote localhost:3333" -ex "b main" -ex "monitor reset halt" -ex "c" $(TARGET)
	pkill $(OPENOCD)
$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(BIN_DIR):
	mkdir $(BIN_DIR)

clean: $(OBJ_DIR) $(BIN_DIR)
	rm -rf obj bin
