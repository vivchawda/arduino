////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_LORA_RFM9X_REGISTERS_h
#define _MITOV_LORA_RFM9X_REGISTERS_h

// The crystal oscillator frequency of the module
static constexpr float RH_RF95_FXOSC = 32000000.0;

// The Frequency Synthesizer step = RH_RF95_FXOSC / 2^^19
static constexpr float RH_RF95_FSTEP = (RH_RF95_FXOSC / 524288);

// Register names (LoRa Mode, from table 85)
static const uint8_t RH_RF95_REG_00_FIFO                                = 0x00;
static const uint8_t RH_RF95_REG_01_OP_MODE                             = 0x01;
static const uint8_t RH_RF95_REG_02_RESERVED                            = 0x02;
static const uint8_t RH_RF95_REG_03_RESERVED                            = 0x03;
static const uint8_t RH_RF95_REG_04_RESERVED                            = 0x04;
static const uint8_t RH_RF95_REG_05_RESERVED                            = 0x05;
static const uint8_t RH_RF95_REG_06_FRF_MSB                             = 0x06;
static const uint8_t RH_RF95_REG_07_FRF_MID                             = 0x07;
static const uint8_t RH_RF95_REG_08_FRF_LSB                             = 0x08;
static const uint8_t RH_RF95_REG_09_PA_CONFIG                           = 0x09;
static const uint8_t RH_RF95_REG_0A_PA_RAMP                             = 0x0a;
static const uint8_t RH_RF95_REG_0B_OCP                                 = 0x0b;
static const uint8_t RH_RF95_REG_0C_LNA                                 = 0x0c;
static const uint8_t RH_RF95_REG_0D_FIFO_ADDR_PTR                       = 0x0d;
static const uint8_t RH_RF95_REG_0E_FIFO_TX_BASE_ADDR                   = 0x0e;
static const uint8_t RH_RF95_REG_0F_FIFO_RX_BASE_ADDR                   = 0x0f;
static const uint8_t RH_RF95_REG_10_FIFO_RX_CURRENT_ADDR                = 0x10;
static const uint8_t RH_RF95_REG_11_IRQ_FLAGS_MASK                      = 0x11;
static const uint8_t RH_RF95_REG_12_IRQ_FLAGS                           = 0x12;
static const uint8_t RH_RF95_REG_13_RX_NB_BYTES                         = 0x13;
static const uint8_t RH_RF95_REG_14_RX_HEADER_CNT_VALUE_MSB             = 0x14;
static const uint8_t RH_RF95_REG_15_RX_HEADER_CNT_VALUE_LSB             = 0x15;
static const uint8_t RH_RF95_REG_16_RX_PACKET_CNT_VALUE_MSB             = 0x16;
static const uint8_t RH_RF95_REG_17_RX_PACKET_CNT_VALUE_LSB             = 0x17;
static const uint8_t RH_RF95_REG_18_MODEM_STAT                          = 0x18;
static const uint8_t RH_RF95_REG_19_PKT_SNR_VALUE                       = 0x19;
static const uint8_t RH_RF95_REG_1A_PKT_RSSI_VALUE                      = 0x1a;
static const uint8_t RH_RF95_REG_1B_RSSI_VALUE                          = 0x1b;
static const uint8_t RH_RF95_REG_1C_HOP_CHANNEL                         = 0x1c;
static const uint8_t RH_RF95_REG_1D_MODEM_CONFIG1                       = 0x1d;
static const uint8_t RH_RF95_REG_1E_MODEM_CONFIG2                       = 0x1e;
static const uint8_t RH_RF95_REG_1F_SYMB_TIMEOUT_LSB                    = 0x1f;
static const uint8_t RH_RF95_REG_20_PREAMBLE_MSB                        = 0x20;
static const uint8_t RH_RF95_REG_21_PREAMBLE_LSB                        = 0x21;
static const uint8_t RH_RF95_REG_22_PAYLOAD_LENGTH                      = 0x22;
static const uint8_t RH_RF95_REG_23_MAX_PAYLOAD_LENGTH                  = 0x23;
static const uint8_t RH_RF95_REG_24_HOP_PERIOD                          = 0x24;
static const uint8_t RH_RF95_REG_25_FIFO_RX_BYTE_ADDR                   = 0x25;
static const uint8_t RH_RF95_REG_26_MODEM_CONFIG3                       = 0x26;

static const uint8_t RH_RF95_REG_40_DIO_MAPPING1                        = 0x40;
static const uint8_t RH_RF95_REG_41_DIO_MAPPING2                        = 0x41;
static const uint8_t RH_RF95_REG_42_VERSION                             = 0x42;


// Register names (FSK/OOK Mode, from table 85)
static const uint8_t RH_RF95_REG_3E_IRQFLAGS1                           = 0x3E;

// RH_RF95_REG_3E_IRQFLAGS1
static const uint8_t RH_RF95_IRQFLAGS1_MODEREADY                         = 0x80;
static const uint8_t RH_RF95_IRQFLAGS1_RXREADY                           = 0x40;
static const uint8_t RH_RF95_IRQFLAGS1_TXREADY                           = 0x20;
static const uint8_t RH_RF95_IRQFLAGS1_PLLLOCK                           = 0x10;
static const uint8_t RH_RF95_IRQFLAGS1_RSSI                              = 0x08;
static const uint8_t RH_RF95_IRQFLAGS1_TIMEOUT                           = 0x04;
static const uint8_t RH_RF95_IRQFLAGS1_AUTOMODE                          = 0x02;
static const uint8_t RH_RF95_IRQFLAGS1_SYNADDRESSMATCH                   = 0x01;

// RH_RF95_REG_01_OP_MODE                             0x01
static const uint8_t RH_RF95_LONG_RANGE_MODE                       = 0x80;
static const uint8_t RH_RF95_ACCESS_SHARED_REG                     = 0x40;
static const uint8_t RH_RF95_MODE                                  = 0x07;
static const uint8_t RH_RF95_MODE_SLEEP                            = 0x00;
static const uint8_t RH_RF95_MODE_STDBY                            = 0x01;
static const uint8_t RH_RF95_MODE_FSTX                             = 0x02;
static const uint8_t RH_RF95_MODE_TX                               = 0x03;
static const uint8_t RH_RF95_MODE_FSRX                             = 0x04;
static const uint8_t RH_RF95_MODE_RXCONTINUOUS                     = 0x05;
static const uint8_t RH_RF95_MODE_RXSINGLE                         = 0x06;
static const uint8_t RH_RF95_MODE_CAD                              = 0x07;

// RH_RF95_REG_09_PA_CONFIG                           0x09
static const uint8_t RH_RF95_PA_SELECT                             = 0x80;
static const uint8_t RH_RF95_OUTPUT_POWER                          = 0x0f;

// RH_RF95_REG_0A_PA_RAMP                             0x0a
static const uint8_t RH_RF95_LOW_PN_TX_PLL_OFF                     = 0x10;
static const uint8_t RH_RF95_PA_RAMP                               = 0x0f;
static const uint8_t RH_RF95_PA_RAMP_3_4MS                         = 0x00;
static const uint8_t RH_RF95_PA_RAMP_2MS                           = 0x01;
static const uint8_t RH_RF95_PA_RAMP_1MS                           = 0x02;
static const uint8_t RH_RF95_PA_RAMP_500US                         = 0x03;
static const uint8_t RH_RF95_PA_RAMP_250US                         = 0x0;
static const uint8_t RH_RF95_PA_RAMP_125US                         = 0x05;
static const uint8_t RH_RF95_PA_RAMP_100US                         = 0x06;
static const uint8_t RH_RF95_PA_RAMP_62US                          = 0x07;
static const uint8_t RH_RF95_PA_RAMP_50US                          = 0x08;
static const uint8_t RH_RF95_PA_RAMP_40US                          = 0x09;
static const uint8_t RH_RF95_PA_RAMP_31US                          = 0x0a;
static const uint8_t RH_RF95_PA_RAMP_25US                          = 0x0b;
static const uint8_t RH_RF95_PA_RAMP_20US                          = 0x0c;
static const uint8_t RH_RF95_PA_RAMP_15US                          = 0x0d;
static const uint8_t RH_RF95_PA_RAMP_12US                          = 0x0e;
static const uint8_t RH_RF95_PA_RAMP_10US                          = 0x0f;

// RH_RF95_REG_0B_OCP                                 0x0b
static const uint8_t RH_RF95_OCP_ON                                = 0x20;
static const uint8_t RH_RF95_OCP_TRIM                              = 0x1f;

// RH_RF95_REG_0C_LNA                                 0x0c
static const uint8_t RH_RF95_LNA_GAIN                              = 0xe0;
static const uint8_t RH_RF95_LNA_BOOST                             = 0x03;
static const uint8_t RH_RF95_LNA_BOOST_DEFAULT                     = 0x00;
static const uint8_t RH_RF95_LNA_BOOST_150PC                       = 0x11;

// RH_RF95_REG_11_IRQ_FLAGS_MASK                      0x11
static const uint8_t RH_RF95_RX_TIMEOUT_MASK                       = 0x80;
static const uint8_t RH_RF95_RX_DONE_MASK                          = 0x40;
static const uint8_t RH_RF95_PAYLOAD_CRC_ERROR_MASK                = 0x20;
static const uint8_t RH_RF95_VALID_HEADER_MASK                     = 0x10;
static const uint8_t RH_RF95_TX_DONE_MASK                          = 0x08;
static const uint8_t RH_RF95_CAD_DONE_MASK                         = 0x04;
static const uint8_t RH_RF95_FHSS_CHANGE_CHANNEL_MASK              = 0x02;
static const uint8_t RH_RF95_CAD_DETECTED_MASK                     = 0x01;

// RH_RF95_REG_12_IRQ_FLAGS                           0x12
static const uint8_t RH_RF95_RX_TIMEOUT                            = 0x80;
static const uint8_t RH_RF95_RX_DONE                               = 0x40;
static const uint8_t RH_RF95_PAYLOAD_CRC_ERROR                     = 0x20;
static const uint8_t RH_RF95_VALID_HEADER                          = 0x10;
static const uint8_t RH_RF95_TX_DONE                               = 0x08;
static const uint8_t RH_RF95_CAD_DONE                              = 0x04;
static const uint8_t RH_RF95_FHSS_CHANGE_CHANNEL                   = 0x02;
static const uint8_t RH_RF95_CAD_DETECTED                          = 0x01;

// RH_RF95_REG_18_MODEM_STAT                          0x18
static const uint8_t RH_RF95_RX_CODING_RATE                        = 0xe0;
static const uint8_t RH_RF95_MODEM_STATUS_CLEAR                    = 0x10;
static const uint8_t RH_RF95_MODEM_STATUS_HEADER_INFO_VALID        = 0x08;
static const uint8_t RH_RF95_MODEM_STATUS_RX_ONGOING               = 0x04;
static const uint8_t RH_RF95_MODEM_STATUS_SIGNAL_SYNCHRONIZED      = 0x02;
static const uint8_t RH_RF95_MODEM_STATUS_SIGNAL_DETECTED          = 0x01;

// RH_RF95_REG_1C_HOP_CHANNEL                         0x1c
static const uint8_t RH_RF95_PLL_TIMEOUT                           = 0x80;
static const uint8_t RH_RF95_RX_PAYLOAD_CRC_IS_ON                  = 0x40;
static const uint8_t RH_RF95_FHSS_PRESENT_CHANNEL                  = 0x3f;

// RH_RF95_REG_1D_MODEM_CONFIG1                       0x1d
static const uint8_t RH_RF95_BW                                    = 0xc0;
static const uint8_t RH_RF95_BW_125KHZ                             = 0x00;
static const uint8_t RH_RF95_BW_250KHZ                             = 0x40;
static const uint8_t RH_RF95_BW_500KHZ                             = 0x80;
static const uint8_t RH_RF95_BW_RESERVED                           = 0xc0;
static const uint8_t RH_RF95_CODING_RATE                           = 0x38;
static const uint8_t RH_RF95_CODING_RATE_4_5                       = 0x00;
static const uint8_t RH_RF95_CODING_RATE_4_6                       = 0x08;
static const uint8_t RH_RF95_CODING_RATE_4_7                       = 0x10;
static const uint8_t RH_RF95_CODING_RATE_4_8                       = 0x18;
static const uint8_t RH_RF95_IMPLICIT_HEADER_MODE_ON               = 0x04;
static const uint8_t RH_RF95_RX_PAYLOAD_CRC_ON                     = 0x02;
static const uint8_t RH_RF95_LOW_DATA_RATE_OPTIMIZE                = 0x01;

// RH_RF95_REG_1E_MODEM_CONFIG2                       0x1e
static const uint8_t RH_RF95_SPREADING_FACTOR                      = 0xf0;
static const uint8_t RH_RF95_SPREADING_FACTOR_64CPS                = 0x60;
static const uint8_t RH_RF95_SPREADING_FACTOR_128CPS               = 0x70;
static const uint8_t RH_RF95_SPREADING_FACTOR_256CPS               = 0x80;
static const uint8_t RH_RF95_SPREADING_FACTOR_512CPS               = 0x90;
static const uint8_t RH_RF95_SPREADING_FACTOR_1024CPS              = 0xa0;
static const uint8_t RH_RF95_SPREADING_FACTOR_2048CPS              = 0xb0;
static const uint8_t RH_RF95_SPREADING_FACTOR_4096CPS              = 0xc0;
static const uint8_t RH_RF95_TX_CONTINUOUS_MOE                     = 0x08;
static const uint8_t RH_RF95_AGC_AUTO_ON                           = 0x04;
static const uint8_t RH_RF95_SYM_TIMEOUT_MSB                       = 0x03;

#endif
