OBJ_DIR=./build/obj
BIN_DIR=./build/bin
DBG_OBJ_DIR=./build/dbg
STM8L15x_PERIPH_SRC_DIR=./stm8l15x_periph_lib/src
TARGET=stm8l152c6
PROGRAMMER=stlinkv2
OPENOCD_ITF=stlink-v2.cfg
OPENOCD_TARGET=stm8l152.cfg

SRC_APP=main.c

SRC_EC_MOTOR=./bldc/bldc_motor.c \
	./bldc/stm8/bldc_motor_pwm.c \
	./bldc/stm8/bldc_motor_hall.c \
	./bldc/stm8/bldc_motor_load.c \
	./bldc/stm8/bldc_motor_speed.c \
	./bldc/stm8/stm8_bldc_motor.c

SRC_PERIPH+=$(STM8L15x_PERIPH_SRC_DIR)/stm8l15x_adc.c
#SRC_PERIPH+=$(STM8L15x_PERIPH_SRC_DIR)/stm8l15x_comp.c
SRC_PERIPH+=$(STM8L15x_PERIPH_SRC_DIR)/stm8l15x_flash.c
SRC_PERIPH+=$(STM8L15x_PERIPH_SRC_DIR)/stm8l15x_itc.c
SRC_PERIPH+=$(STM8L15x_PERIPH_SRC_DIR)/stm8l15x_rst.c
SRC_PERIPH+=$(STM8L15x_PERIPH_SRC_DIR)/stm8l15x_tim1.c
SRC_PERIPH+=$(STM8L15x_PERIPH_SRC_DIR)/stm8l15x_tim5.c
#SRC_PERIPH+=$(STM8L15x_PERIPH_SRC_DIR)/stm8l15x_aes.c
#SRC_PERIPH+=$(STM8L15x_PERIPH_SRC_DIR)/stm8l15x_dac.c
SRC_PERIPH+=$(STM8L15x_PERIPH_SRC_DIR)/stm8l15x_gpio.c
#SRC_PERIPH+=$(STM8L15x_PERIPH_SRC_DIR)/stm8l15x_iwdg.c
#SRC_PERIPH+=$(STM8L15x_PERIPH_SRC_DIR)/stm8l15x_rtc.c
SRC_PERIPH+=$(STM8L15x_PERIPH_SRC_DIR)/stm8l15x_tim2.c
SRC_PERIPH+=$(STM8L15x_PERIPH_SRC_DIR)/stm8l15x_usart.c
#SRC_PERIPH+=$(STM8L15x_PERIPH_SRC_DIR)/stm8l15x_beep.c
#SRC_PERIPH+=$(STM8L15x_PERIPH_SRC_DIR)/stm8l15x_dma.c
SRC_PERIPH+=$(STM8L15x_PERIPH_SRC_DIR)/stm8l15x_i2c.c
#SRC_PERIPH+=$(STM8L15x_PERIPH_SRC_DIR)/stm8l15x_lcd.c
SRC_PERIPH+=$(STM8L15x_PERIPH_SRC_DIR)/stm8l15x_spi.c
SRC_PERIPH+=$(STM8L15x_PERIPH_SRC_DIR)/stm8l15x_tim3.c
#SRC_PERIPH+=$(STM8L15x_PERIPH_SRC_DIR)/stm8l15x_wfe.c
SRC_PERIPH+=$(STM8L15x_PERIPH_SRC_DIR)/stm8l15x_clk.c
SRC_PERIPH+=$(STM8L15x_PERIPH_SRC_DIR)/stm8l15x_exti.c
#SRC_PERIPH+=$(STM8L15x_PERIPH_SRC_DIR)/stm8l15x_irtim.c
SRC_PERIPH+=$(STM8L15x_PERIPH_SRC_DIR)/stm8l15x_pwr.c
SRC_PERIPH+=$(STM8L15x_PERIPH_SRC_DIR)/stm8l15x_syscfg.c
SRC_PERIPH+=$(STM8L15x_PERIPH_SRC_DIR)/stm8l15x_tim4.c
#SRC_PERIPH+=$(STM8L15x_PERIPH_SRC_DIR)/stm8l15x_wwdg.c

SRC+=$(SRC_APP)
SRC+=$(SRC_EC_MOTOR)
SRC+=$(SRC_PERIPH)

OBJ=$(SRC:.c=.rel)

INC=-I./ \
	-I./stm8l15x_periph_lib/inc \
	-I./bldc \
	-I./bldc/stm8 \

CFLAGS= -mstm8 
CFLAGS_DEBUG= -mstm8 --out-fmt-elf --debug --all-callee-saves --verbose --stack-auto --fverbose-asm  --float-reent --no-peep
CC=sdcc

all: ec_drive

ec_drive: $(OBJ)
	$(shell if [ ! -d $(BIN_DIR) ];	then mkdir -p $(BIN_DIR); fi)
	$(CC) $(CFLAGS) $(addprefix $(OBJ_DIR)/, $(notdir $(OBJ))) -o $(BIN_DIR)/$@.ihx
	$(CC) $(CFLAGS_DEBUG) $(addprefix $(DBG_OBJ_DIR)/, $(notdir $(OBJ))) -o $(BIN_DIR)/$@.elf
	stm8-size $(BIN_DIR)/$@.elf

debug: ec_drive
	openocd -f interface/$(OPENOCD_ITF) -f target/$(OPENOCD_TARGET) -c "init" -c "reset halt" &
	stm8-gdb -iex "set auto-load safe-path ." $(BIN_DIR)/$<.elf 
	pkill openocd

%.rel: %.c
	$(shell if [ ! -d $(OBJ_DIR) ];	then mkdir -p $(OBJ_DIR); fi)
	$(shell if [ ! -d $(DBG_OBJ_DIR) ];	then mkdir -p $(DBG_OBJ_DIR); fi)
	$(CC) $(CFLAGS) $(INC) -c $< -o $(OBJ_DIR)/$(notdir $@)
	$(CC) $(CFLAGS_DEBUG) $(INC) -c $< -o $(DBG_OBJ_DIR)/$(notdir $@)

flash: ec_drive
#	stm8flash -c $(PROGRAMMER) -p $(TARGET) -w $(BIN_DIR)/$<.elf
	stm8flash -c $(PROGRAMMER) -p $(TARGET) -w $(BIN_DIR)/$<.ihx

clean:
	rm -rf build
