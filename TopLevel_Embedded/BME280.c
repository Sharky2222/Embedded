//this file takes inspiration from demo arduino code for the Arduino + BME280 implementation
//also used info from bosch datasheet+internet research to create these functions

#include <stddef.h>
#include <stdint.h>

#include "BME280.h"
#include "i2c.h"

static void BME280_dummyDelay(uint32_t delay);
static uint8_t BME280_readReg(uint8_t reg);
void BME280_readRegArray(uint8_t startAddress, uint8_t* buffer, uint8_t len);
static void BME280_writeReg(uint8_t reg, uint8_t value);
static void BME280_readCalibrationValues(void); //set up your method headers

static volatile BME280_CalibrationData mCalibrationData;

BME280_S32_t t_fine; //set up calibration on startup

void BME280_init(void)
{
    t_fine = 0x00;  //set ur granularity
    BME280_dummyDelay(20000);
    BME280_reset();
    BME280_dummyDelay(20000); //clear old calibration data

    BME280_writeReg(BME280_REG_CTRL_HUM, 0x01); //set up samplingg

    BME280_writeReg(BME280_REG_CTRL_MEAS, 0x54); //start in sleep mode
    BME280_writeReg(BME280_REG_CONFIG, 0x10); //only read once data is set
    BME280_writeReg(BME280_REG_CTRL_MEAS, 0x57);//wake up bme
    BME280_readCalibrationValues();//calibrate bme
}


void BME280_reset(void)
{
    BME280_writeReg(BME280_REG_RESET, 0xB6);
}


uint8_t BME280_readChipID(void)
{
    uint8_t id = BME280_readReg(BME280_REG_ID);
    return id;
}

BME280_Data BME280_read(void)
{
    uint8_t data[8] = {0x00};
    uint32_t msb, lsb, xlsb = 0x00;
    uint32_t uPressure, uTemperature, uHumidity = 0x00; //create the vars for the different elements


    BME280_Data result; //result vector
    BME280_readRegArray(BME280_REG_PRESS_MSB, data, 8); //read the data array

    msb = ((uint32_t)data[0]) & 0xFF;
    lsb = ((uint32_t)data[1]) & 0xFF;
    xlsb = ((uint32_t)(data[2] >> 4)) & 0x0F;
    uPressure = ((msb << 12) | (lsb << 4) | (xlsb)); //pressure data, take the msb and lsb of ur pressure data and then set the int

    msb = ((uint32_t)data[3]) & 0xFF;
    lsb = ((uint32_t)data[4]) & 0xFF;
    xlsb = ((uint32_t)(data[5] >> 4)) & 0x0F;
    uTemperature = ((msb << 12) | (lsb << 4) | (xlsb)); //same thing as pressure just for temp data

    msb = ((uint32_t)data[6]) & 0xFF;
    lsb = ((uint32_t)data[7]) & 0xFF;
    uHumidity = ((msb << 8) | (lsb)); //same thing for humididty data

    result.uPressure = uPressure;
    result.uTemperature = uTemperature;
    result.uHumidity = uHumidity; //set the result params to the local vars

    BME280_S32_t cTemperature = BME280_compensate_T_int32((BME280_S32_t)uTemperature);
    BME280_U32_t cPressure = BME280_compensate_P_int64((BME280_S32_t)uPressure);
    BME280_U32_t cHumidity = BME280_compensate_H_int32((BME280_S32_t)uHumidity);

    result.cTemperatureCInt = cTemperature / 100;
    result.cTemperatureCFrac = cTemperature % 100;

    BME280_S32_t cTemperatureF = cTemperature * 9 / 5;
    cTemperatureF += 3200;
    result.cTemperatureFInt = cTemperatureF / 100;
    result.cTemperatureFFrac = cTemperatureF % 100;

    result.cPressureInt = cPressure / 256;
    result.cPressureFrac = cPressure % 256;

    result.cHumidityInt = cHumidity / 1024;
    result.cHumidityFrac = cHumidity % 1024; //bring everything to int values

    return result;
}

void BME280_sleep(void) //activate the sleep state
{
    uint8_t value = BME280_readReg(BME280_REG_CTRL_MEAS);
    value &=~ 0x03;

    BME280_writeReg(BME280_REG_CTRL_MEAS, value);
}

void BME280_wakeup(void) //exit sleep
{
    uint8_t value = BME280_readReg(BME280_REG_CTRL_MEAS);
    value |= 0x03;              //bits 0 and 1 high

    BME280_writeReg(BME280_REG_CTRL_MEAS, value);
}

void BME280_dummyDelay(uint32_t delay) //this is the bme internal delay
{
    volatile uint32_t temp = delay;
    while (temp > 0)
        temp--;
}

uint8_t BME280_readReg(uint8_t reg) //bme i2c transmission
{
    uint8_t result = 0x00;
    i2c_readAddress(BME280_I2C_ADDRESS, reg, &result, 1);
    return result;
}

void BME280_writeReg(uint8_t reg, uint8_t value) //write the data to the register and write to i2c buffer
{
    uint8_t tx[2] = {reg, value};
    i2c_write(BME280_I2C_ADDRESS, tx, 2);
}

void BME280_readRegArray(uint8_t startAddress, uint8_t* buffer, uint8_t len) //u need this for i2c to work
{
    i2c_readAddress(BME280_I2C_ADDRESS, startAddress, buffer, len);
}

void BME280_readCalibrationValues(void) //read calibration --> translated from ardunio
{
    //read calibration registers
    uint8_t reg[24] = {0x00};
    uint8_t a1 = 0x00;
    uint8_t e1[7] = {0x00};

    BME280_readRegArray(0x88, reg, 24);
    BME280_readRegArray(0xA1, &a1, 1);
    BME280_readRegArray(0xE1, e1, 7);
    mCalibrationData.dig_T1 = ((uint16_t)reg[0]) + (((uint16_t)reg[1]) * 256);
    mCalibrationData.dig_T2 = ((int16_t)reg[2]) + (((int16_t)reg[3]) * 256);
    mCalibrationData.dig_T3 = ((int16_t)reg[4]) + (((int16_t)reg[5]) * 256);

    mCalibrationData.dig_P1 = ((uint16_t)reg[6]) + (((uint16_t)reg[7]) * 256);
    mCalibrationData.dig_P2 = ((int16_t)reg[8]) + (((int16_t)reg[9]) * 256);
    mCalibrationData.dig_P3 = ((int16_t)reg[10]) + (((int16_t)reg[11]) * 256);
    mCalibrationData.dig_P4 = ((int16_t)reg[12]) + (((int16_t)reg[13]) * 256);
    mCalibrationData.dig_P5 = ((int16_t)reg[14]) + (((int16_t)reg[15]) * 256);
    mCalibrationData.dig_P6 = ((int16_t)reg[16]) + (((int16_t)reg[17]) * 256);
    mCalibrationData.dig_P7 = ((int16_t)reg[18]) + (((int16_t)reg[19]) * 256);
    mCalibrationData.dig_P8 = ((int16_t)reg[20]) + (((int16_t)reg[21]) * 256);
    mCalibrationData.dig_P9 = ((int16_t)reg[22]) + (((int16_t)reg[23]) * 256);

    mCalibrationData.dig_H1 = a1;
    mCalibrationData.dig_H2 = ((int16_t)e1[0]) + (((int16_t)e1[1]) * 256);
    mCalibrationData.dig_H3 = e1[2];

    mCalibrationData.dig_H4 = ((int16_t)(e1[4] & 0x0F)) + (((int16_t)e1[3]) * 16);
    mCalibrationData.dig_H5 = ((int16_t)((e1[4] & 0xF0) >> 4)) + (((int16_t)e1[5]) * 16);
    mCalibrationData.dig_H6 = e1[6];

}

BME280_S32_t BME280_compensate_T_int32(BME280_S32_t adc_T) //temperature compensation function taken from BME280 datasheet
{
    BME280_S32_t var1, var2, T;

    var1 = ((((adc_T>>3) - ((BME280_S32_t)mCalibrationData.dig_T1 << 1))) * ((BME280_S32_t)mCalibrationData.dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((BME280_S32_t)mCalibrationData.dig_T1)) * ((adc_T >> 4) - ((BME280_S32_t)mCalibrationData.dig_T1))) >> 12) * ((BME280_S32_t)mCalibrationData.dig_T3)) >> 14;

    t_fine = var1 + var2;
    T  = (t_fine * 5 + 128) >> 8;
    return T;
}

BME280_U32_t BME280_compensate_P_int64(BME280_S32_t adc_P) //pressure compensation function which was taken from the BME280 datasheet
{
    BME280_S64_t var1, var2, p;
    var1 = ((BME280_S64_t)t_fine) - 128000;

    var2 = var1 * var1 * (BME280_S64_t)mCalibrationData.dig_P6;
    var2 = var2 + ((var1*(BME280_S64_t)mCalibrationData.dig_P5)<<17);
    var2 = var2 + (((BME280_S64_t)mCalibrationData.dig_P4)<<35);
    var1 = ((var1 * var1 * (BME280_S64_t)mCalibrationData.dig_P3)>>8) + ((var1 * (BME280_S64_t)mCalibrationData.dig_P2)<<12);
    var1 = (((((BME280_S64_t)1)<<47)+var1))*((BME280_S64_t)mCalibrationData.dig_P1)>>33;
    if (var1 == 0)
    {
        return 0;
    }

    p = 1048576-adc_P;
    p = (((p<<31)-var2)*3125)/var1;
    var1 = (((BME280_S64_t)mCalibrationData.dig_P9) * (p>>13) * (p>>13)) >> 25;
    var2 = (((BME280_S64_t)mCalibrationData.dig_P8) * p) >> 19;  p = ((p + var1 + var2) >> 8) + (((BME280_S64_t)mCalibrationData.dig_P7)<<4);
    return (BME280_U32_t)p;
}

BME280_U32_t BME280_compensate_H_int32(BME280_S32_t adc_H) //humidity compensation function taken from datasheet
{
    BME280_S32_t v_x1_u32r;
    v_x1_u32r = (t_fine - ((BME280_S32_t)76800));
    v_x1_u32r = (((((adc_H << 14) - (((BME280_S32_t)mCalibrationData.dig_H4) << 20) - (((BME280_S32_t)mCalibrationData.dig_H5) * v_x1_u32r)) + ((BME280_S32_t)16384)) >> 15) * (((((((v_x1_u32r * ((BME280_S32_t)mCalibrationData.dig_H6)) >> 10) * (((v_x1_u32r *
        ((BME280_S32_t)mCalibrationData.dig_H3)) >> 11) + ((BME280_S32_t)32768))) >> 10) + ((BME280_S32_t)2097152)) * ((BME280_S32_t)mCalibrationData.dig_H2) + 8192) >> 14));
    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((BME280_S32_t)mCalibrationData.dig_H1)) >> 4));
    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
    return (BME280_U32_t)(v_x1_u32r>>12);
}
