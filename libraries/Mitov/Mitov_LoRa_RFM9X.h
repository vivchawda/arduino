////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_LORA_RFM9X_h
#define _MITOV_LORA_RFM9X_h

#include <Mitov.h>
#include <Mitov_Basic_SPI.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

//#define ___MITOV_LORA_RFM9X_DEBUG__

namespace Mitov
{
//---------------------------------------------------------------------------
	enum TRFM9XGainControl { gcAuto, gcG1, gcG2, gcG3, gcG4, gcG5, gcG6 };
	enum TRFM9XCodingRate { cr4_5, cr4_6, cr4_7, cr4_8 };
//---------------------------------------------------------------------------
	class MitovBasicLoRaRFM9X : public OpenWire::Component
	{
		typedef	OpenWire::Component	inherited;

	public:
		OpenWire::SourcePin	OutputPin;
		OpenWire::TypedStartSourcePin<int32_t> SignalStrengthOutputPin;

	public:
		#include <Mitov_LoRa_RFM9X_Registers.h>

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
		uint16_t	PreambleLength = 8;
		uint8_t		Power = 13;
		float		Frequency = 915.0;
		float		SignalBandwidth = 125.0;
		uint8_t		MaxSendQueue = 10;

	public:
		uint16_t	SymbolTimeout : 10;
		bool	LoRaTMMode : 1;
		bool	MobileNode : 1;
		bool	LowNoiseAmplifierHighFrequencyBoost : 1;
		TRFM9XGainControl	Gain : 3;
		TRFM9XCodingRate	CodingRate : 2;
		uint8_t	SpreadingFactorOrder : 4;

	protected:
		bool	FReady : 1;
		bool	FModeChange : 1;
		RHMode	FMode : 3;

		/// The value of the last received RSSI value, in some transport specific units
		volatile int8_t     FLastRssi = 0;

		Mitov::SimpleList<uint8_t *>	FReceivedPackets;
		Mitov::SimpleList<uint8_t *>	FSendQueue;

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
			if( ! FReady )
				return;

#ifdef ___MITOV_LORA_RFM9X_DEBUG__
			Serial.println( "TRY SEND" );
			Serial.println( len );
#endif
//				Serial.println( FTestCount );
//				Serial.println( int( FMode ));
//			if (len > RH_RF69_MAX_MESSAGE_LEN)
//				return;

//			waitPacketSent(); // Make sure we dont interrupt an outgoing message // _mode == RHModeTx

			if( FMode == RHModeTx )
			{
				while( FSendQueue.size() > MaxSendQueue )
					;

				uint8_t *ABuffer = new uint8_t[ len + 1 ];
				ABuffer[ 0 ] = len;
				memcpy( ABuffer + 1, data, len );
				FSendQueue.push_back( ABuffer );
				return;
			}

			setModeIdle(); // Prevent RX while filling the fifo
#ifdef ___MITOV_LORA_RFM9X_DEBUG__
			Serial.println( "SEND" );
#endif

			// Position at the beginning of the FIFO
			spiWrite(RH_RF95_REG_0D_FIFO_ADDR_PTR, 0);

			TransferPacket( data, len );

			spiWrite( RH_RF95_REG_22_PAYLOAD_LENGTH, len );

			setModeTx(); // Start the transmitter
		}

	protected:
		virtual void SystemInit() override
		{

//			ChipSelectOutputPin.SendValue( true );
//			inherited::SystemInit();

			Reset();
//			Serial.println( "START1" );

			// Set sleep mode, so we can also set LORA mode:
			spiWrite(RH_RF95_REG_01_OP_MODE, RH_RF95_MODE_SLEEP | RH_RF95_LONG_RANGE_MODE);
			delay(10); // Wait for sleep mode to take over from say, CAD

			// Check we are in sleep mode, with LORA set
			if (spiRead(RH_RF95_REG_01_OP_MODE) != (RH_RF95_MODE_SLEEP | RH_RF95_LONG_RANGE_MODE))
			{
		//	Serial.println(spiRead(RH_RF95_REG_01_OP_MODE), HEX);
				return; // No device present?
			}

			FReady = true;

//			Serial.println( spiRead( RH_RF95_REG_1F_SYMB_TIMEOUT_LSB ), HEX );

//			Serial.println( "START2" );
			// Set up FIFO
			// We configure so that we can use the entire 256 byte FIFO for either receive
			// or transmit, but not both at the same time
			spiWrite(RH_RF95_REG_0E_FIFO_TX_BASE_ADDR, 0);
			spiWrite(RH_RF95_REG_0F_FIFO_RX_BASE_ADDR, 0);

			// Packet format is preamble + explicit-header + payload + crc
			// Explicit Header Mode
			// payload is TO + FROM + ID + FLAGS + message data
			// RX mode is implmented with RXCONTINUOUS
			// max message data length is 255 - 4 = 251 octets

			setModeIdle();

/*
			// Set up default configuration
			// No Sync Words in LORA mode.
			setModemConfig(Bw125Cr45Sf128); // Radio default
		//    setModemConfig(Bw125Cr48Sf4096); // slow and reliable?
			setPreambleLength(8); // Default is 8
			// An innocuous ISM frequency, same as RF22's
			setFrequency(434.0);
			// Lowish power
			setTxPower(13);
		//    setTxPower(20);
*/
			UpdateModemRegisters();
			UpdatePreambleLength();
			UpdateFrequency();
			UpdateTransmitPower();
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
//			inherited::SystemLoopBegin( currentMicros );
			if( ! FReady )
				return;

			if( FModeChange )
			{
				if( ! LoRaTMMode )
					if( !(spiRead(RH_RF95_REG_3E_IRQFLAGS1) & RH_RF95_IRQFLAGS1_MODEREADY) )
						return;

				FModeChange = false;
			}

			uint8_t *APacket = nullptr;
			noInterrupts(); // Protect the FReceivedPackets from being corrupted
			if( FReceivedPackets.size() )
			{
				APacket = FReceivedPackets[ 0 ];
				FReceivedPackets.pop_front();
			}

			interrupts();

			if( APacket )
			{
//				uint8_t ASentTo = APacket[ 0 ];
				uint8_t ASize = APacket[ 0 ];
#ifdef ___MITOV_LORA_RFM9X_DEBUG__
				Serial.println( "RECEIVED" );
				Serial.println( ASize );
#endif
				OutputPin.SendValue( Mitov::TDataBlock( ASize, APacket + 1 ));
				delete [] APacket;
			}

			if( FMode != RHModeTx )
			{
				if( FSendQueue.size() )
				{
					uint8_t *ABuffer = FSendQueue[ 0 ];
					FSendQueue.pop_front();
					send( ABuffer + 1, ABuffer[ 0 ] );
					delete [] ABuffer;
				}
			}

			if( FMode == RHModeIdle )
				if( OutputPin.IsConnected() )
					setModeRx();

			SignalStrengthOutputPin.SetValue( FLastRssi );
		}

	protected:
		virtual void Reset() = 0;
		virtual uint8_t spiRead(uint8_t reg) = 0;
		virtual uint8_t spiWrite(uint8_t reg, uint8_t val) = 0;
		virtual void readFifo( uint8_t ALength ) = 0;
		virtual void TransferPacket( const uint8_t* data, uint8_t len ) = 0;

	protected:
		/// If current mode is Rx or Tx changes it to Idle. If the transmitter or receiver is running, 
		/// disables them.
		void setModeIdle()
		{
			if ( FMode != RHModeIdle)
			{
				setOpMode( RH_RF95_MODE_STDBY );
				FMode = RHModeIdle;
			}
		}

		void setModeRx()
		{
			if (FMode != RHModeRx)
			{
				spiWrite(RH_RF95_REG_01_OP_MODE, RH_RF95_MODE_RXCONTINUOUS);
				spiWrite(RH_RF95_REG_40_DIO_MAPPING1, 0x00); // Interrupt on RxDone
				FMode = RHModeRx;
			}
		}

		void setModeTx()
		{
			if (FMode != RHModeTx)
			{
				spiWrite(RH_RF95_REG_01_OP_MODE, RH_RF95_MODE_TX);
				spiWrite(RH_RF95_REG_40_DIO_MAPPING1, 0x40); // Interrupt on TxDone
				FMode = RHModeTx;
			}
		}

		void setOpMode(uint8_t mode)
		{
			uint8_t opmode = spiRead(RH_RF95_REG_01_OP_MODE);
			opmode &= ~RH_RF95_MODE;
			opmode |= (mode & RH_RF95_MODE);
			spiWrite(RH_RF95_REG_01_OP_MODE, opmode);

			FModeChange = true;

			// Wait for mode to change.
//			while (!(spiRead(RH_RF69_REG_27_IRQFLAGS1) & RH_RF69_IRQFLAGS1_MODEREADY))
//				;
		}

	public:
		void UpdatePreambleLength()
		{
//			spiWrite(RH_RF69_REG_2C_PREAMBLEMSB, PreambleLength >> 8);
//			spiWrite(RH_RF69_REG_2D_PREAMBLELSB, PreambleLength & 0xff);

			spiWrite(RH_RF95_REG_20_PREAMBLE_MSB, PreambleLength >> 8);
		    spiWrite(RH_RF95_REG_21_PREAMBLE_LSB, PreambleLength & 0xff);
		}

		void UpdateFrequency()
		{
			// Frf = FRF / FSTEP
/*
			uint32_t frf = (Frequency * 1000000.0) / RH_RF69_FSTEP;
//			Serial.println( frf );
			spiWrite(RH_RF69_REG_07_FRFMSB, (frf >> 16) & 0xff);
			spiWrite(RH_RF69_REG_08_FRFMID, (frf >> 8) & 0xff);
			spiWrite(RH_RF69_REG_09_FRFLSB, frf & 0xff);
*/
			uint32_t frf = (Frequency * 1000000.0) / RH_RF95_FSTEP;
			spiWrite(RH_RF95_REG_06_FRF_MSB, (frf >> 16) & 0xff);
			spiWrite(RH_RF95_REG_07_FRF_MID, (frf >> 8) & 0xff);
			spiWrite(RH_RF95_REG_08_FRF_LSB, frf & 0xff);
		}

		void UpdateTransmitPower()
		{
			int8_t aPower = Power;
			if (aPower > 20)
				aPower = 20;

			if (aPower < 5)
				aPower = 5;

			// RFM95/96/97/98 does not have RFO pins connected to anything. ONly PA_BOOST
			// pin is connected, so must use PA_BOOST
			// Pout = 2 + OutputPower.
			// The documentation is pretty confusing on this topic: PaSelect says the max poer is 20dBm,
			// but OutputPower claims it would be 17dBm.
			// My measurements show 20dBm is correct
			spiWrite(RH_RF95_REG_09_PA_CONFIG, RH_RF95_PA_SELECT | (aPower-5));
		//    spiWrite(RH_RF95_REG_09_PA_CONFIG, 0); // no power
		}

		// Sets registers from a canned modem configuration structure
		void UpdateModemRegisters()
		{
//    { 0x72,   0x74,    0x00}, // Bw125Cr45Sf128 (the chip default)

//			spiWrite(RH_RF95_REG_1D_MODEM_CONFIG1,       config->reg_1d);
//			spiWrite(RH_RF95_REG_1E_MODEM_CONFIG2,       config->reg_1e);

			uint8_t	AValue;

			if( Gain == gcAuto )
				AValue = 0b11000000;

			else
				AValue = Gain << 5;

			AValue |= (( LowNoiseAmplifierHighFrequencyBoost ) ? 0b11 : 0 );

			spiWrite( RH_RF95_REG_0C_LNA, AValue );

			static const float CSignalBandwidths[] =
			{
				7.8,
				10.4,
				15.6,
				20.8,
				31.25,
				41.7,
				62.5,
				125.0,
				250.0,
				500.0			
			};

			AValue = 9;
			for( int i = 0; i < sizeof( CSignalBandwidths ) / sizeof( CSignalBandwidths[ 0 ] ); ++i )
				if( SignalBandwidth <= CSignalBandwidths[ i ] )
				{
					AValue = i;
					break;
				}

//			Serial.println( AValue );

			AValue <<= 4;
			AValue |= ( CodingRate + 1 ) << 1;

//			Serial.println( AValue, HEX );

//			spiWrite(RH_RF95_REG_1D_MODEM_CONFIG1,       0x72 );
			spiWrite(RH_RF95_REG_1D_MODEM_CONFIG1,       AValue );

			AValue = ( SpreadingFactorOrder << 4 ) |
				     0b100 | // Header indicates CRC on
					 ( SymbolTimeout >> 8 );

//			Serial.println( AValue, HEX );

//			spiWrite(RH_RF95_REG_1E_MODEM_CONFIG2,       0x74 );
			spiWrite(RH_RF95_REG_1E_MODEM_CONFIG2,       AValue );

			AValue =	(( Gain == gcAuto ) ? 0 : 0b100 ) |
								(( MobileNode ) ? 0b1000 : 0 );
			spiWrite(RH_RF95_REG_26_MODEM_CONFIG3,       AValue );

			spiWrite( RH_RF95_REG_1F_SYMB_TIMEOUT_LSB, uint8_t( SymbolTimeout ));

		}

	public:
		// C++ level interrupt handler for this instance
		// RH_RF69 is unusual in that it has several interrupt lines, and not a single, combined one.
		// On Moteino, only one of the several interrupt lines (DI0) from the RH_RF69 is connnected to the processor.
		// We use this to get PACKETSDENT and PAYLOADRADY interrupts.
		void InterruptHandler( bool )
		{
			// Read the interrupt register
			uint8_t irq_flags = spiRead(RH_RF95_REG_12_IRQ_FLAGS);
			if ( FMode == RHModeRx && irq_flags & (RH_RF95_RX_TIMEOUT | RH_RF95_PAYLOAD_CRC_ERROR))
				; //_rxBad++;

			else if ( FMode == RHModeRx && irq_flags & RH_RF95_RX_DONE)
			{
				// Have received a packet
				uint8_t len = spiRead(RH_RF95_REG_13_RX_NB_BYTES);

				// Reset the fifo read ptr to the beginning of the packet
//				spiWrite(RH_RF95_REG_0D_FIFO_ADDR_PTR, spiRead(RH_RF95_REG_10_FIFO_RX_CURRENT_ADDR));
//				spiBurstRead(RH_RF95_REG_00_FIFO, _buf, len);
//				_bufLen = len;
//				spiWrite(RH_RF95_REG_12_IRQ_FLAGS, 0xff); // Clear all IRQ flags

				// Remember the RSSI of this packet
				// this is according to the doc, but is it really correct?
				// weakest receiveable signals are reported RSSI at about -66
				FLastRssi = spiRead(RH_RF95_REG_1A_PKT_RSSI_VALUE) - 137;

				readFifo( len );

				// We have received a message.
//				validateRxBuf(); 
//				if (_rxBufValid)
					setModeIdle(); // Got one 
			}
			else if ( FMode == RHModeTx && irq_flags & RH_RF95_TX_DONE)
			{
//				_txGood++;
				setModeIdle();
			}
    
			spiWrite(RH_RF95_REG_12_IRQ_FLAGS, 0xff); // Clear all IRQ flags
		}

	public:
		MitovBasicLoRaRFM9X() :
			SymbolTimeout( 0x64 ),
			LoRaTMMode( true ),
			MobileNode( false ),
			LowNoiseAmplifierHighFrequencyBoost( false ),
			Gain( gcAuto ),
			CodingRate( cr4_5 ),
			SpreadingFactorOrder( 7 ),
			FReady( false ),
			FModeChange( false ),
			FMode( RHModeInitialising )
		{
		}
	};
//---------------------------------------------------------------------------
	class MitovLoRaRFM9X : public MitovBasicLoRaRFM9X
	{
		typedef	MitovBasicLoRaRFM9X	inherited;

	public:
		OpenWire::SourcePin	ChipSelectOutputPin;
		OpenWire::SourcePin	ResetOutputPin;

	protected:
		BasicSPI	&FSPI;
		int			FInterruptPin;

	protected:
		virtual void Reset() override
		{
			ChipSelectOutputPin.SendValue( true );

			if( ResetOutputPin.IsConnected() )
			{
//				Serial.println( "RESET" );
				ResetOutputPin.SendValue( false );
				delay( 100 );
				ResetOutputPin.SendValue( true );
				delay( 100 );
			}
		}

		virtual uint8_t spiRead(uint8_t reg) override
		{
			uint8_t val;
			FSPI.beginTransaction( SPISettings() );
			ChipSelectOutputPin.SendValue( false );
			FSPI.transfer(reg & ~0x80); // Send the address with the write mask off
			val = FSPI.transfer(0); // The written value is ignored, reg value is read
			ChipSelectOutputPin.SendValue( true );
			FSPI.endTransaction();
//Serial.print( "spiRead: " ); Serial.print( reg, HEX ); Serial.print( " = " ); Serial.println( val, HEX );
			return val;
		}

		virtual uint8_t spiWrite(uint8_t reg, uint8_t val) override
		{
//Serial.print( "spiWrite: " ); Serial.print( reg, HEX ); Serial.print( " = " ); Serial.println( val, HEX );
			FSPI.beginTransaction( SPISettings() );
			ChipSelectOutputPin.SendValue( false );
			uint8_t status = FSPI.transfer(reg | 0x80); // Send the address with the write mask on
			FSPI.transfer(val); // New value follows
			ChipSelectOutputPin.SendValue( true );
			FSPI.endTransaction();
			return status;
		}

	protected:
		virtual void readFifo( uint8_t ALength ) override
		{
//			Serial.print( "LN: " ); Serial.println( ALength );
			// Reset the fifo read ptr to the beginning of the packet
			spiWrite(RH_RF95_REG_0D_FIFO_ADDR_PTR, spiRead(RH_RF95_REG_10_FIFO_RX_CURRENT_ADDR));

			FSPI.beginTransaction( SPISettings() );
			ChipSelectOutputPin.SendValue( false );

			FSPI.transfer(RH_RF95_REG_00_FIFO); // Send the start address with the write mask off

			uint8_t *ABuffer = new uint8_t[ 1 + ALength ];
			FReceivedPackets.push_back( ABuffer );
			*ABuffer++ = ALength; // Skip the ARxHeaderTo

			for ( int i = 0; i < ALength; ++i )
				*ABuffer++ = FSPI.transfer(0);

			ChipSelectOutputPin.SendValue( true );
			FSPI.endTransaction();
		}

		virtual void TransferPacket( const uint8_t* data, uint8_t len ) override
		{
			FSPI.beginTransaction( SPISettings() );
			ChipSelectOutputPin.SendValue( false );
			FSPI.transfer( RH_RF95_REG_00_FIFO | 0x80 ); // Send the start address with the write mask on
//			FSPI.transfer( len + 1 ); // Include length of headers
			// First the 4 headers
//			FSPI.transfer( ATxHeaderTo );
//			FSPI.transfer( Address );
//			FSPI.transfer(_txHeaderId);
//			FSPI.transfer(_txHeaderFlags);
			// Now the payload
			while( len-- )
				FSPI.transfer( *data++ );

			ChipSelectOutputPin.SendValue( true );
			FSPI.endTransaction();
		}

	protected:
		virtual void SystemInit() override
		{
			// Do in startup for M0 compatibility! (Fails in constructor!)
			int AInterruptNumber = digitalPinToInterrupt( FInterruptPin );
			FSPI.usingInterrupt( AInterruptNumber );
			inherited::SystemInit();
		}

	public:
		MitovLoRaRFM9X( BasicSPI &ASPI, int AInterruptPin ) :
			FInterruptPin( AInterruptPin ),
			FSPI( ASPI )
		{
		}

	};
//---------------------------------------------------------------------------
	template<int PIN_CS, int PIN_RST, int PIN_INT> class MitovLoRaRFM9XShield : public MitovBasicLoRaRFM9X
	{
		typedef	MitovBasicLoRaRFM9X	inherited;

	protected:
		static MitovLoRaRFM9XShield<PIN_CS,PIN_RST,PIN_INT>	*FInstnce;

	protected:
		static void StaticInterruptHandler()
		{
			FInstnce->InterruptHandler( true );
		}

	protected:
		virtual void Reset() override
		{
			digitalWrite( PIN_CS, HIGH );

//				Serial.println( "RESET" );
			digitalWrite( PIN_RST, LOW );
			delay( 100 );
			digitalWrite( PIN_RST, HIGH );
			delay( 100 );
		}

		virtual void readFifo( uint8_t ALength ) override
		{
//			Serial.print( "LN: " ); Serial.println( ALength );

			// Reset the fifo read ptr to the beginning of the packet
			spiWrite(RH_RF95_REG_0D_FIFO_ADDR_PTR, spiRead(RH_RF95_REG_10_FIFO_RX_CURRENT_ADDR));

			SPI.beginTransaction( SPISettings() );
			digitalWrite( PIN_CS, LOW );

			SPI.transfer(RH_RF95_REG_00_FIFO); // Send the start address with the write mask off

			uint8_t *ABuffer = new uint8_t[ 1 + ALength ];
			FReceivedPackets.push_back( ABuffer );
			*ABuffer++ = ALength; // Skip the ARxHeaderTo

			for ( int i = 0; i < ALength; ++i )
				*ABuffer++ = SPI.transfer(0);

			digitalWrite( PIN_CS, HIGH );
			SPI.endTransaction();
		}

		virtual void TransferPacket( const uint8_t* data, uint8_t len ) override
		{
			SPI.beginTransaction( SPISettings() );
			digitalWrite( PIN_CS, LOW );
			SPI.transfer( RH_RF95_REG_00_FIFO | 0x80 ); // Send the start address with the write mask on
//			SPI.transfer( len + 1 ); // Include length of headers
			// First the 4 headers
//			SPI.transfer( ATxHeaderTo );
//			SPI.transfer( Address );
//			SPI.transfer(_txHeaderId);
//			SPI.transfer(_txHeaderFlags);
			// Now the payload
			while( len-- )
				SPI.transfer( *data++ );

			digitalWrite( PIN_CS, HIGH );
			SPI.endTransaction();
		}

		virtual uint8_t spiRead(uint8_t reg) override
		{
			SPI.beginTransaction( SPISettings() );
			digitalWrite( PIN_CS, LOW );
			SPI.transfer(reg & ~0x80); // Send the address with the write mask off
			uint8_t val = SPI.transfer(0); // The written value is ignored, reg value is read
			digitalWrite( PIN_CS, HIGH );
			SPI.endTransaction();
//Serial.print( "spiRead: " ); Serial.print( reg, HEX ); Serial.print( " = " ); Serial.println( val, HEX );
			return val;
		}

		virtual uint8_t spiWrite(uint8_t reg, uint8_t val) override
		{
//Serial.print( "spiWrite: " ); Serial.print( reg, HEX ); Serial.print( " = " ); Serial.println( val, HEX );
			SPI.beginTransaction( SPISettings() );
			digitalWrite( PIN_CS, LOW );
			uint8_t status = SPI.transfer(reg | 0x80); // Send the address with the write mask on
			SPI.transfer(val); // New value follows
			digitalWrite( PIN_CS, HIGH );
			SPI.endTransaction();
			return status;
		}

	protected:
		virtual void SystemInit() override
		{
			SPI.begin();
			int AInterruptNumber = digitalPinToInterrupt( PIN_INT );
			attachInterrupt( AInterruptNumber, StaticInterruptHandler, RISING );
			SPI.usingInterrupt( AInterruptNumber );
			inherited::SystemInit();
		}

	public:
		MitovLoRaRFM9XShield()
		{
			FInstnce = this;
			pinMode( PIN_RST, OUTPUT );
			pinMode( PIN_CS, OUTPUT );
		}
	};
//---------------------------------------------------------------------------
	template<int PIN_CS, int PIN_RST, int PIN_INT> MitovLoRaRFM9XShield<PIN_CS,PIN_RST,PIN_INT>	*MitovLoRaRFM9XShield<PIN_CS,PIN_RST,PIN_INT>::FInstnce = nullptr;
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
