////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_LORA_NRF24L01_h
#define _MITOV_LORA_NRF24L01_h

#include <Mitov.h>
#include <Mitov_Basic_SPI.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	enum TArduinoNRF24L01DataRate
	{
		ndr250KBps, 
		ndr1MBps, 
		ndr2MBps
	};
//---------------------------------------------------------------------------
	class MitovNRF24L01 : public OpenWire::Component
	{
		typedef	OpenWire::Component	inherited;

	protected:
		/// \brief Defines different operating modes for the transport hardware
		///
		/// These are the different values that can be adopted by the _mode variable and 
		/// returned by the mode() member function,
		enum RHMode
		{
			RHModeInitialising,			///< Transport is initialising. Initial default value until init() is called..
			RHModeIdle,					///< Transport is idle.
			RHModeFrequencySynthesizer, ///< Frequency Synthesizer.
			RHModeTx,					///< Transport is in the process of transmitting a message.
			RHModeRx					///< Transport is in the process of receiving a message.
		};

	public:
		OpenWire::SourcePin	OutputPin;

		OpenWire::SourcePin	ChipSelectOutputPin;
		OpenWire::SourcePin	ChipEnableOutputPin;

#ifdef _MITOV_NRF24L01_CARRIER_DETECT_PIN_
		OpenWire::TypedStartSourcePin<bool>	CarrierDetectedOutputPin;
#endif

	public:
		uint8_t		MaxSendQueue = 10;
		int8_t		OutputPower = 0;
		uint16_t	Frequency : 12; // = 2402;
//		uint8_t		AddressSize = 5;
//		uint8_t		RetransmitCount = 0;
//		uint16_t	RetransmitDelay = 250;
		uint8_t		CRCBytes : 2; // = 2;
		TArduinoNRF24L01DataRate	DataRate : 2; // = ndr2MBps;

//		Enabled	: 1;

		RHMode	FMode : 3;

		Mitov::SimpleList<uint8_t *>	FSendQueue;

	protected:
		BasicSPI	&FSPI;

	protected:
		static const uint8_t RH_NRF24_MAX_PAYLOAD_LEN = 32;

		// SPI Command names
		static const uint8_t RH_NRF24_COMMAND_R_REGISTER                        = 0x00;
		static const uint8_t RH_NRF24_COMMAND_W_REGISTER                        = 0x20;
		static const uint8_t RH_NRF24_COMMAND_ACTIVATE                          = 0x50; // only on RFM73 ?
		static const uint8_t RH_NRF24_COMMAND_R_RX_PAYLOAD                      = 0x61;
		static const uint8_t RH_NRF24_COMMAND_W_TX_PAYLOAD                      = 0xa0;
		static const uint8_t RH_NRF24_COMMAND_FLUSH_TX                          = 0xe1;
		static const uint8_t RH_NRF24_COMMAND_FLUSH_RX                          = 0xe2;
		static const uint8_t RH_NRF24_COMMAND_REUSE_TX_PL                       = 0xe3;
		static const uint8_t RH_NRF24_COMMAND_R_RX_PL_WID                       = 0x60;
//		static const uint8_t RH_NRF24_COMMAND_W_ACK_PAYLOAD(pipe)               (= 0xa8|(pipe& 0x7));
		static const uint8_t RH_NRF24_COMMAND_W_ACK_PAYLOAD                     = 0xa8;
		static const uint8_t RH_NRF24_COMMAND_W_TX_PAYLOAD_NOACK                = 0xb0;
		static const uint8_t RH_NRF24_COMMAND_NOP                               = 0xff;

		// Register names
		static const uint8_t RH_NRF24_REGISTER_MASK                             = 0x1f;
		static const uint8_t RH_NRF24_REG_00_CONFIG                             = 0x00;
		static const uint8_t RH_NRF24_REG_01_EN_AA                              = 0x01;
		static const uint8_t RH_NRF24_REG_02_EN_RXADDR                          = 0x02;
		static const uint8_t RH_NRF24_REG_03_SETUP_AW                           = 0x03;
		static const uint8_t RH_NRF24_REG_04_SETUP_RETR                         = 0x04;
		static const uint8_t RH_NRF24_REG_05_RF_CH                              = 0x05;
		static const uint8_t RH_NRF24_REG_06_RF_SETUP                           = 0x06;
		static const uint8_t RH_NRF24_REG_07_STATUS                             = 0x07;
		static const uint8_t RH_NRF24_REG_08_OBSERVE_TX                         = 0x08;
		static const uint8_t RH_NRF24_REG_09_RPD                                = 0x09;
		static const uint8_t RH_NRF24_REG_0A_RX_ADDR_P0                         = 0x0a;
		static const uint8_t RH_NRF24_REG_0B_RX_ADDR_P1                         = 0x0b;
		static const uint8_t RH_NRF24_REG_0C_RX_ADDR_P2                         = 0x0c;
		static const uint8_t RH_NRF24_REG_0D_RX_ADDR_P3                         = 0x0d;
		static const uint8_t RH_NRF24_REG_0E_RX_ADDR_P4                         = 0x0e;
		static const uint8_t RH_NRF24_REG_0F_RX_ADDR_P5                         = 0x0f;
		static const uint8_t RH_NRF24_REG_10_TX_ADDR                            = 0x10;
		static const uint8_t RH_NRF24_REG_11_RX_PW_P0                           = 0x11;
		static const uint8_t RH_NRF24_REG_12_RX_PW_P1                           = 0x12;
		static const uint8_t RH_NRF24_REG_13_RX_PW_P2                           = 0x13;
		static const uint8_t RH_NRF24_REG_14_RX_PW_P3                           = 0x14;
		static const uint8_t RH_NRF24_REG_15_RX_PW_P4                           = 0x15;
		static const uint8_t RH_NRF24_REG_16_RX_PW_P5                           = 0x16;
		static const uint8_t RH_NRF24_REG_17_FIFO_STATUS                        = 0x17;
		static const uint8_t RH_NRF24_REG_1C_DYNPD                              = 0x1c;
		static const uint8_t RH_NRF24_REG_1D_FEATURE                            = 0x1d;

		// corresponding to the bit and field names in the nRF24L01 Product Specification
		// #define RH_NRF24_REG_00_CONFIG                             0x00
		static const uint8_t RH_NRF24_MASK_RX_DR                                = 0x40;
		static const uint8_t RH_NRF24_MASK_TX_DS                                = 0x20;
		static const uint8_t RH_NRF24_MASK_MAX_RT                               = 0x10;
		static const uint8_t RH_NRF24_EN_CRC                                    = 0x08;
		static const uint8_t RH_NRF24_CRCO                                      = 0x04;
		static const uint8_t RH_NRF24_PWR_UP                                    = 0x02;
		static const uint8_t RH_NRF24_PRIM_RX                                   = 0x01;

		// #define RH_NRF24_REG_05_RF_CH                              0x05
		static const uint8_t RH_NRF24_RF_CH                                     = 0x7f;

		// #define RH_NRF24_REG_07_STATUS                             0x07
		static const uint8_t RH_NRF24_RX_DR                                     = 0x40;
		static const uint8_t RH_NRF24_TX_DS                                     = 0x20;
		static const uint8_t RH_NRF24_MAX_RT                                    = 0x10;
		static const uint8_t RH_NRF24_RX_P_NO                                   = 0x0e;
		static const uint8_t RH_NRF24_STATUS_TX_FULL                            = 0x01;

		// #define RH_NRF24_REG_17_FIFO_STATUS                        0x17
		static const uint8_t RH_NRF24_TX_REUSE                                  = 0x40;
		static const uint8_t RH_NRF24_TX_FULL                                   = 0x20;
		static const uint8_t RH_NRF24_TX_EMPTY                                  = 0x10;
		static const uint8_t RH_NRF24_RX_FULL                                   = 0x02;
		static const uint8_t RH_NRF24_RX_EMPTY                                  = 0x01;

		// #define RH_NRF24_REG_1C_DYNPD                              0x1c
		static const uint8_t RH_NRF24_DPL_ALL                                   = 0x2f;
		static const uint8_t RH_NRF24_DPL_P5                                    = 0x20;
		static const uint8_t RH_NRF24_DPL_P4                                    = 0x10;
		static const uint8_t RH_NRF24_DPL_P3                                    = 0x08;
		static const uint8_t RH_NRF24_DPL_P2                                    = 0x04;
		static const uint8_t RH_NRF24_DPL_P1                                    = 0x02;
		static const uint8_t RH_NRF24_DPL_P0                                    = 0x01;

		// #define RH_NRF24_REG_1D_FEATURE                            0x1d
		static const uint8_t RH_NRF24_EN_DPL                                    = 0x04;
		static const uint8_t RH_NRF24_EN_ACK_PAY                                = 0x02;
		static const uint8_t RH_NRF24_EN_DYN_ACK                                = 0x01;

		// #define RH_NRF24_REG_06_RF_SETUP                           0x06
		static const uint8_t RH_NRF24_CONT_WAVE                                 = 0x80;
		static const uint8_t RH_NRF24_RF_DR_LOW                                 = 0x20;
		static const uint8_t RH_NRF24_PLL_LOCK                                  = 0x10;
		static const uint8_t RH_NRF24_RF_DR_HIGH                                = 0x08;

	public:
		void UpdateFrequency()
		{
			uint8_t AChannel = uint16_t( Frequency ) - 2400;
			spiWriteRegister(RH_NRF24_REG_05_RF_CH, AChannel & RH_NRF24_RF_CH);
		}

		void UpdateRF()
		{
			static const int8_t CPower[] =
			{
				-18,
				-12,
				-6,
				0
			};

			uint8_t AValue = 3;
			for( int i = 0; i < 4; ++i )
				if( OutputPower <= CPower[ i ] )
				{
					AValue = i;
					break;
				}

			AValue <<= 1;
			if ( DataRate == ndr250KBps )
				AValue |= RH_NRF24_RF_DR_LOW;

			else if ( DataRate == ndr2MBps )
				AValue |= RH_NRF24_RF_DR_HIGH;

			spiWriteRegister(RH_NRF24_REG_06_RF_SETUP, AValue );
		}

		void UpdateConfiguration()
		{
			// TODO: Also include the RX/TX settings!!!!
			spiWriteRegister(RH_NRF24_REG_00_CONFIG, GetConfiguration());
		}

		void UpdateRetransmit()
		{
		}

		void UpdateAddress()
		{
//			uint_t AValue = AddressSize - 2;
//			RH_NRF24_REG_03_SETUP_AW
		}

	public:
		void Print( String AValue )
		{
			AValue += "\r\n";
			send( (uint8_t *)AValue.c_str(), AValue.length() );
		}

		void Print( float AValue )
		{
			char AText[ 16 ];
			dtostrf( AValue,  1, 2, AText );
			Print( String( AText ));
		}

		void Print( int32_t AValue )
		{
			char AText[ 16 ];
			itoa( AValue, AText, 10 );
			Print( String( AText ));
		}

		void Print( uint32_t AValue )
		{
			char AText[ 16 ];
			itoa( AValue, AText, 10 );
			Print( String( AText ));
		}

		void PrintChar( char AValue )
		{
			send( (uint8_t*)&AValue, 1 );
		}

		void PrintChar( byte AValue )
		{
			send( &AValue, 1 );
		}

		void Write( uint8_t *AData, uint32_t ASize )
		{
			send( AData, ASize );
		}

	public:
		void send( const uint8_t* data, uint8_t len )
		{
			send( nullptr, data, len );
		}

		void send( const uint8_t *ATxAddressTo, const uint8_t* data, uint8_t len ) // Address is always 5 bytes!
		{
//				Serial.println( "TRY SEND" );
//				Serial.println( len );
//				Serial.println( FTestCount );
//				Serial.println( int( FMode ));
//			if (len > RH_RF69_MAX_MESSAGE_LEN)
//				return;

//			waitPacketSent(); // Make sure we dont interrupt an outgoing message // _mode == RHModeTx

			if( FMode == RHModeTx )
			{
				while( FSendQueue.size() > MaxSendQueue )
					SystemLoopBegin( micros() );

				uint8_t *ABuffer;
				if( ATxAddressTo )
				{
					ABuffer = new uint8_t[ len + 5 + 1 ];
					ABuffer[ 0 ] = len | 0x80; // The high significant bit indicates address
					memcpy( ABuffer + 1, ATxAddressTo, 5 );
					memcpy( ABuffer + 5 + 1, data, len );
				}

				else
				{
					ABuffer = new uint8_t[ len + 1 ];
					ABuffer[ 0 ] = len;
					memcpy( ABuffer + 1, data, len );
				}

				FSendQueue.push_back( ABuffer );
				return;
			}

			if ( len > RH_NRF24_MAX_PAYLOAD_LEN )
				return;

//			if( ATxAddressTo )
				// Set transmit address
//				spiBurstWriteRegister( RH_NRF24_REG_0A_RX_ADDR_P0, ATxAddressTo, AddressSize );

			setModeTx();
			if( ATxAddressTo )
				spiBurstWrite(RH_NRF24_COMMAND_W_ACK_PAYLOAD, data, len ); // Optionally add pipe #

			else
				spiBurstWrite(RH_NRF24_COMMAND_W_TX_PAYLOAD_NOACK, data, len );

			// Radio will return to Standby II mode after transmission is complete
//			_txGood++;

/*
			setModeIdle(); // Prevent RX while filling the fifo
//			Serial.println( "SEND" );

			FSPI.beginTransaction( SPISettings() );
			ChipSelectOutputPin.SendValue( false );
			FSPI.transfer( RH_RF69_REG_00_FIFO | 0x80 ); // Send the start address with the write mask on
			FSPI.transfer( len + 1 ); // Include length of headers
			// First the 4 headers
			FSPI.transfer( ATxHeaderTo );
//			FSPI.transfer( Address );
//			FSPI.transfer(_txHeaderId);
//			FSPI.transfer(_txHeaderFlags);
			// Now the payload
			while( len-- )
				FSPI.transfer( *data++ );

			ChipSelectOutputPin.SendValue( true );
			FSPI.endTransaction();

			setModeTx(); // Start the transmitter
*/
		}

	protected:
		inline uint8_t statusRead()
		{
			// status is a side-effect of NOP, faster than reading reg 07
			return spiCommand(RH_NRF24_COMMAND_NOP); 
		}

		uint8_t GetConfiguration()
		{
			static const uint8_t CCRCModes [] =
			{
				0,
				RH_NRF24_EN_CRC,
				RH_NRF24_EN_CRC | RH_NRF24_CRCO
			};

			return CCRCModes[ CRCBytes ];
		}

		void setModeIdle()
		{
			if ( FMode != RHModeIdle )
			{
				UpdateConfiguration();
				ChipEnableOutputPin.SendValue( false );
				FMode = RHModeIdle;
			}
		}

		void setModeTx()
		{
			if (FMode != RHModeTx)
			{
				// Its the pulse high that puts us into TX mode
				ChipEnableOutputPin.SendValue( false );
//				Serial.print( "CONFIG: " ); Serial.println( GetConfiguration(), BIN );
				spiWriteRegister(RH_NRF24_REG_00_CONFIG, GetConfiguration() | RH_NRF24_PWR_UP);
//				Serial.print( "RH_NRF24_REG_00_CONFIG: " ); Serial.println( (unsigned int)spiReadRegister( RH_NRF24_REG_00_CONFIG ), BIN );
				ChipEnableOutputPin.SendValue( true );
				FMode = RHModeTx;
			}
		}

		void setModeRx()
		{
			if ( FMode != RHModeRx)
			{
//				Serial.println( "setModeRx" );
				spiWriteRegister(RH_NRF24_REG_00_CONFIG, GetConfiguration() | RH_NRF24_PWR_UP | RH_NRF24_PRIM_RX);
				ChipEnableOutputPin.SendValue( true );
				FMode = RHModeRx;
			}
		}

		inline uint8_t flushTx()
		{
			return spiCommand(RH_NRF24_COMMAND_FLUSH_TX);
		}

		inline uint8_t flushRx()
		{
			return spiCommand(RH_NRF24_COMMAND_FLUSH_RX);
		}

	protected:
		uint8_t spiRead(uint8_t reg)
		{
//			FSPI.beginTransaction( SPISettings( 1000000, MSBFIRST, SPI_MODE0 ) );
			FSPI.beginTransaction( SPISettings() );
			ChipSelectOutputPin.SendValue( false );
			FSPI.transfer( reg );
			uint8_t val = FSPI.transfer(0); // The written value is ignored, reg value is read
			ChipSelectOutputPin.SendValue( true );
			FSPI.endTransaction();
			return val;
		}

		uint8_t spiWrite(uint8_t reg, uint8_t val)
		{
//Serial.print( "spiWrite: " ); Serial.print( reg, HEX ); Serial.print( " = " ); Serial.println( val, HEX );
//			FSPI.beginTransaction( SPISettings( 1000000, MSBFIRST, SPI_MODE0 ) );
			FSPI.beginTransaction( SPISettings() );
			ChipSelectOutputPin.SendValue( false );
			uint8_t status = FSPI.transfer( reg ); // Send the address with the write mask on
			FSPI.transfer(val); // New value follows
			ChipSelectOutputPin.SendValue( true );
			FSPI.endTransaction();
			return status;
		}


		uint8_t spiBurstRead(uint8_t reg, uint8_t* dest, uint8_t len)
		{
			FSPI.beginTransaction( SPISettings() );
			ChipSelectOutputPin.SendValue( false );
			uint8_t status = FSPI.transfer(reg); // Send the start address
			while (len--)
				*dest++ = FSPI.transfer(0);

			ChipSelectOutputPin.SendValue( true );
			FSPI.endTransaction();
			return status;
		}

		uint8_t spiBurstWrite( uint8_t reg, const uint8_t* src, uint8_t len )
		{
/*
			Serial.print( "spiBurstWrite: " ); Serial.print( reg, HEX ); Serial.print( " =" );
			for( int i = 0; i < len; ++i )
			{
				Serial.print( " " );
				Serial.print( src[ i ], HEX );
			}

			Serial.println( "" );
*/
//			FSPI.beginTransaction( SPISettings( 1000000, MSBFIRST, SPI_MODE0 ) );
/*
			FSPI.beginTransaction( SPISettings() );
			ChipSelectOutputPin.SendValue( false );
			uint8_t status = FSPI.transfer(reg); // Send the start address with the write mask on
			while (len--)
				FSPI.transfer(*src++);

			ChipSelectOutputPin.SendValue( true );
			FSPI.endTransaction();
			return status;
*/
			FSPI.beginTransaction( SPISettings() );
			ChipSelectOutputPin.SendValue( false );
			uint8_t status = FSPI.transfer(reg); // Send the start address
			while (len--)
				FSPI.transfer(*src++);

			ChipSelectOutputPin.SendValue( true );
			FSPI.endTransaction();
			return status;

		}

		inline uint8_t spiReadRegister(uint8_t reg)
		{
			return spiRead((reg & RH_NRF24_REGISTER_MASK) | RH_NRF24_COMMAND_R_REGISTER);
		}

		inline uint8_t spiWriteRegister(uint8_t reg, uint8_t val)
		{
//			Serial.print( "spiWriteRegister: " ); Serial.print( reg, HEX ); Serial.print( " = " ); Serial.println( val, BIN );
			return spiWrite((reg & RH_NRF24_REGISTER_MASK) | RH_NRF24_COMMAND_W_REGISTER, val);
		}

/*
		inline uint8_t spiBurstReadRegister(uint8_t reg, uint8_t* dest, uint8_t len)
		{
			return spiBurstRead((reg & RH_NRF24_REGISTER_MASK) | RH_NRF24_COMMAND_R_REGISTER, dest, len);
		}
*/
		inline uint8_t spiBurstWriteRegister(uint8_t reg, const uint8_t* src, uint8_t len)
		{
			return spiBurstWrite((reg & RH_NRF24_REGISTER_MASK) | RH_NRF24_COMMAND_W_REGISTER, src, len);
		}

		// Low level commands for interfacing with the device
		uint8_t spiCommand(uint8_t command)
		{
//			FSPI.beginTransaction( SPISettings( 1000000, MSBFIRST, SPI_MODE0 ) );
			FSPI.beginTransaction( SPISettings() );
			ChipSelectOutputPin.SendValue( false );
			uint8_t status = FSPI.transfer(command);
			ChipSelectOutputPin.SendValue( true );
			FSPI.endTransaction();
			return status;
		}

	protected:
		virtual void SystemInit() override
		{
//			inherited::SystemInit();

			ChipEnableOutputPin.SendValue( false );

			// Clear interrupts
			spiWriteRegister(RH_NRF24_REG_07_STATUS, RH_NRF24_RX_DR | RH_NRF24_TX_DS | RH_NRF24_MAX_RT);
			// Enable dynamic payload length on all pipes
			spiWriteRegister(RH_NRF24_REG_1C_DYNPD, RH_NRF24_DPL_ALL);
			// Enable dynamic payload length, disable payload-with-ack, enable noack
			spiWriteRegister(RH_NRF24_REG_1D_FEATURE, RH_NRF24_EN_DPL | RH_NRF24_EN_DYN_ACK);
			// Test if there is actually a device connected and responding
			if (spiReadRegister(RH_NRF24_REG_1D_FEATURE) != (RH_NRF24_EN_DPL | RH_NRF24_EN_DYN_ACK))
				return;

			// Make sure we are powered down
			setModeIdle();

			// Flush FIFOs
			flushTx();
			flushRx();

			// On RFM73, try to figure out if we need to ACTIVATE to enable W_TX_PAYLOAD_NOACK
			uint8_t testWriteData  = 0x77; // Anything will do here
			spiBurstWrite(RH_NRF24_COMMAND_W_TX_PAYLOAD_NOACK, &testWriteData, 1);
			// If RH_NRF24_REG_17_FIFO_STATUS still thinks the Tx fifo is empty, we need to ACTIVATE
			if (spiReadRegister(RH_NRF24_REG_17_FIFO_STATUS) & RH_NRF24_TX_EMPTY)
				spiWrite(RH_NRF24_COMMAND_ACTIVATE, 0x73);

			// Flush Tx FIFO again after using the tx fifo in our test above
			flushTx();

			UpdateFrequency();
			UpdateRetransmit();
//			UpdateAddressSize();


//			Serial.println( "START" );
//			Serial.print( "RH_NRF24_REG_1C_DYNPD: " ); Serial.println( (unsigned int)spiReadRegister( RH_NRF24_REG_1C_DYNPD ), BIN );
//			Serial.print( "RH_NRF24_REG_1D_FEATURE: " ); Serial.println( (unsigned int)spiReadRegister( RH_NRF24_REG_1D_FEATURE ), BIN );
//			Serial.print( "RH_NRF24_REG_05_RF_CH: " ); Serial.println( (unsigned int)spiReadRegister( RH_NRF24_REG_05_RF_CH ), BIN );
//			Serial.print( "RH_NRF24_REG_06_RF_SETUP: " ); Serial.println( (unsigned int)spiReadRegister( RH_NRF24_REG_06_RF_SETUP ), BIN );

//			setChannel(2); // The default, in case it was set by another app without powering down
//			setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm);

		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
//			inherited::SystemLoopBegin( currentMicros );

//			if( ! Enabled )
//				return;

			if ( FMode == RHModeTx )
			{
//				Serial.println( "SENDING" );
				uint8_t status = statusRead();
//				Serial.println( status, HEX );
				if( status & (RH_NRF24_TX_DS | RH_NRF24_MAX_RT))
				{
					// Must clear RH_NRF24_MAX_RT if it is set, else no further comm
					spiWriteRegister(RH_NRF24_REG_07_STATUS, RH_NRF24_TX_DS | RH_NRF24_MAX_RT);
					if (status & RH_NRF24_MAX_RT)
						flushTx();

//					Serial.println( "SENT" );
					setModeIdle();
				}
			}

			if ( FMode == RHModeRx )
			{
				if( ! ( spiReadRegister(RH_NRF24_REG_17_FIFO_STATUS) & RH_NRF24_RX_EMPTY ))
				{
					// Manual says that messages > 32 octets should be discarded
					uint8_t ASize = spiRead(RH_NRF24_COMMAND_R_RX_PL_WID);
					if (ASize > 32)
					{
						flushRx();
						setModeIdle();
					}

					else
					{
						// Clear read interrupt
						spiWriteRegister(RH_NRF24_REG_07_STATUS, RH_NRF24_RX_DR);

						uint8_t *APacket = new uint8_t[ ASize ];

						// Get the message into the RX buffer, so we can inspect the headers
						spiBurstRead(RH_NRF24_COMMAND_R_RX_PAYLOAD, APacket, ASize );

						OutputPin.SendValue( Mitov::TDataBlock( ASize, APacket ));

						delete [] APacket;

						// 140 microsecs (32 octet payload)
//						validateRxBuf(); 
//						Serial.println( "CONFIRMED" );
//						if (_rxBufValid)

						if( FSendQueue.size() )
							setModeIdle(); // Set to idle so we can try to send
					}
				}
			}

			if( FMode != RHModeTx )
			{
				if( FSendQueue.size() )
				{
					uint8_t *ABuffer = FSendQueue[ 0 ];
					FSendQueue.pop_front();

					if( ABuffer[ 0 ] & 0x80 )
						send( ABuffer + 1, ABuffer + 5 + 1, ABuffer[ 0 ] & 0x7F );

					else
						send( ABuffer + 1, ABuffer[ 0 ] );

					delete [] ABuffer;
				}
			}

			if( FMode == RHModeIdle )
				if( OutputPin.IsConnected() )
					setModeRx();

#ifdef _MITOV_NRF24L01_CARRIER_DETECT_PIN_
			CarrierDetectedOutputPin.SetValue( ( spiReadRegister( RH_NRF24_REG_09_RPD ) & 1 ) != 0 );
#endif
		}

	public:
		MitovNRF24L01( BasicSPI &ASPI ) :
			Frequency( 2402 ),
			CRCBytes( 2 ),
			DataRate( ndr2MBps ),
//			Enabled( true ),
			FSPI( ASPI ),
			FMode( RHModeInitialising )
		{
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
