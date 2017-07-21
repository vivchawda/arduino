////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_LORA_RFM69_h
#define _MITOV_LORA_RFM69_h

#include <Mitov.h>
#include <Mitov_Basic_SPI.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	enum TArduinoLoRaRFM69Modulation
	{
		mFrequencyShiftKeying_Gaussian_1_0,
		mFrequencyShiftKeying_Gaussian_0_5,
		mFrequencyShiftKeying_Gaussian_0_3,
		mOnOffKeying_NoShaping,
		mOnOffKeying_Cutoff_BitRate,
		mOnOffKeying_Cutoff_2_BitRate
	};
//---------------------------------------------------------------------------
	enum TArduinoLoRaRFM69DCCCutoffFrequency
	{
		rfmDCCCutoff16,
		rfmDCCCutoff8,
		rfmDCCCutoff4,
		rfmDCCCutoff2,
		rfmDCCCutoff1,
		rfmDCCCutoff0_5,
		rfmDCCCutoff0_25,
		rfmDCCCutoff0_125
	};
//---------------------------------------------------------------------------
	enum TArduinoLoRaRFM69ChannelFilterBandwidth
	{
		bfFSK_2_6_OOK_1_3,
		bfFSK_3_1_OOK_1_6,
		bfFSK_3_9_OOK_2_0,
		bfFSK_5_2_OOK_2_6,
		bfFSK_6_3_OOK_3_1,
		bfFSK_7_8_OOK_3_9,
		bfFSK_10_4_OOK_5_2,
		bfFSK_12_5_OOK_6_3,
		bfFSK_15_6_OOK_7_8,
		bfFSK_20_8_OOK_10_4,
		bfFSK_25_0_OOK_12_5,
		bfFSK_31_3_OOK_15_6,
		bfFSK_41_7_OOK_20_8,
		bfFSK_50_0_OOK_25_0,
		bfFSK_62_5_OOK_31_3,
		bfFSK_83_3_OOK_41_7,
		bfFSK_100_0_OOK_50_0,
		bfFSK_125_0_OOK_62_5,
		bfFSK_166_7_OOK_83_3,
		bfFSK_200_0_OOK_100_0,
		bfFSK_250_0_OOK_125_0,
		bfFSK_333_3_OOK_166_7,
		bfFSK_400_0_OOK_200_0,
		bfFSK_500_0_OOK_250_0
	};
//---------------------------------------------------------------------------
	enum TArduinoLoRaRFM69DCFreeEncoding
	{
		dcfeNone,
		dcfeManchester,
		dcfeWhitening
	};
//---------------------------------------------------------------------------
/*
	class MitovLoRaRFMBasic : public OpenWire::Component
	{
		typedef	OpenWire::Component	inherited;

	public:
		virtual void send( uint8_t ATxHeaderTo, const uint8_t* data, uint8_t len ) = 0;

	};
*/
	//---------------------------------------------------------------------------
	class MitovBasicLoRaRFM69 : public OpenWire::Component
	{
		typedef	OpenWire::Component	inherited;

	public:
		OpenWire::SourcePin	OutputPin;
		OpenWire::SourcePin	SentToOutputPin;
		OpenWire::TypedStartSourcePin<int32_t> SignalStrengthOutputPin;

	public:
		#include <Mitov_LoRa_RFM69_Registers.h>

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
		/// Sets the length of the preamble
		/// in bytes. 
		/// Caution: this should be set to the same 
		/// value on all nodes in your network. Default is 4.
		/// Sets the message preamble length in REG_0?_PREAMBLE?SB
		/// \param[in] bytes Preamble length in bytes.  
		uint16_t PreambleLength = 4;

		/// The selected output power in dBm
		// +13dBm, same as power-on default
		uint8_t Power = 14;
		float	Frequency = 915.0;
		uint8_t Address = 0;
		uint8_t BroadcastAddress = 0xFF;
		uint8_t MaxSendQueue = 10;

		ConstBytes	SyncWords;
//		uint8_t* SyncWords = nullptr;
//		uint8_t	SyncWords_Length = 0;

		uint8_t* EncryptionKey = nullptr;

//		uint8_t* ModemConfig;

		struct
		{
			uint16_t	BitRate = 0x80;
			float		FrequencyDeviation = 0.25;

			TArduinoLoRaRFM69Modulation	Modulation : 3;
			TArduinoLoRaRFM69DCCCutoffFrequency DCCCutoffFrequency : 3;
			TArduinoLoRaRFM69ChannelFilterBandwidth ChannelFilterBandwidth : 5;
			TArduinoLoRaRFM69DCFreeEncoding DCFreeEncoding : 2;
			bool	CRCEnabled : 1;

		} ModemSettings;

		bool	Promiscuous : 1;

	protected:
		bool	FModeChange : 1;
		RHMode	FMode : 3;

//		BasicSPI	&FSPI;

//		int	FInterruptPin;

		/// The value of the last received RSSI value, in some transport specific units
		volatile int8_t     FLastRssi = 0;

		Mitov::SimpleList<uint8_t *>	FReceivedPackets;
		Mitov::SimpleList<uint8_t *>	FSendQueue;

	public:
		// C++ level interrupt handler for this instance
		// RH_RF69 is unusual in that it has several interrupt lines, and not a single, combined one.
		// On Moteino, only one of the several interrupt lines (DI0) from the RH_RF69 is connnected to the processor.
		// We use this to get PACKETSDENT and PAYLOADRADY interrupts.
		void InterruptHandler( bool )
		{
//			Serial.println( "INT" );
			// Get the interrupt cause
			uint8_t irqflags2 = spiRead(RH_RF69_REG_28_IRQFLAGS2);
			if ( FMode == RHModeTx && (irqflags2 & RH_RF69_IRQFLAGS2_PACKETSENT))
			{
				// A transmitter message has been fully sent
/*
				if( OutputPin.IsConnected() )
					setModeRx(); // Clears FIFO

				else
*/
				setModeIdle(); // Clears FIFO

				//	Serial.println("PACKETSENT");
			}

			// Must look for PAYLOADREADY, not CRCOK, since only PAYLOADREADY occurs _after_ AES decryption
			// has been done
			if ( FMode == RHModeRx && (irqflags2 & RH_RF69_IRQFLAGS2_PAYLOADREADY))
			{
				// A complete message has been received with good CRC
				FLastRssi = -((int8_t)(spiRead(RH_RF69_REG_24_RSSIVALUE) >> 1));
//				_lastPreambleTime = millis();

				setModeIdle();
				// Save it in our buffer
				readFifo();
//				setModeRx();
			}
		}

	public:
		// set the frequency (in Hz)
/*
		void SynthesizeFrequency(uint32_t freqHz)
		{
//			uint8_t oldMode = FMode;
//			if (oldMode == RH_RF69_OPMODE_MODE_TX ) 
//				setModeRx();

			freqHz /= RF69_FSTEP; // divide down by FSTEP to get FRF
			writeReg(REG_FRFMSB, freqHz >> 16);
			writeReg(REG_FRFMID, freqHz >> 8);
			writeReg(REG_FRFLSB, freqHz);
//			if (oldMode == RF69_MODE_RX) 
			setModeFrequencySynthesizer();

//			setOpMode(oldMode);
		}
*/
	protected:
		/// If current mode is Rx or Tx changes it to Idle. If the transmitter or receiver is running, 
		/// disables them.
		void setModeIdle()
		{
			if ( FMode != RHModeIdle)
			{
				if ( Power >= 18)
				{
					// If high power boost, return power amp to receive mode
					spiWrite(RH_RF69_REG_5A_TESTPA1, RH_RF69_TESTPA1_NORMAL);
					spiWrite(RH_RF69_REG_5C_TESTPA2, RH_RF69_TESTPA2_NORMAL);
				}

				setOpMode( RH_RF69_OPMODE_MODE_STDBY );
				FMode = RHModeIdle;
			}
		}

/*
		void setModeFrequencySynthesizer()
		{
			if ( FMode != RHModeFrequencySynthesizer)
			{
				if ( Power >= 18)
				{
					// Set high power boost mode
					// Note that OCP defaults to ON so no need to change that.
					spiWrite(RH_RF69_REG_5A_TESTPA1, RH_RF69_TESTPA1_BOOST);
					spiWrite(RH_RF69_REG_5C_TESTPA2, RH_RF69_TESTPA2_BOOST);
				}

				setOpMode( RH_RF69_OPMODE_MODE_FS );
				FMode = RHModeFrequencySynthesizer;
			}
		}
*/
		void setModeRx()
		{
			if ( FMode != RHModeRx)
			{
//				Serial.println( "setModeRx" );
				if ( Power >= 18)
				{
					// If high power boost, return power amp to receive mode
					spiWrite(RH_RF69_REG_5A_TESTPA1, RH_RF69_TESTPA1_NORMAL);
					spiWrite(RH_RF69_REG_5C_TESTPA2, RH_RF69_TESTPA2_NORMAL);
				}

				spiWrite(RH_RF69_REG_25_DIOMAPPING1, RH_RF69_DIOMAPPING1_DIO0MAPPING_01); // Set interrupt line 0 PayloadReady
				setOpMode(RH_RF69_OPMODE_MODE_RX); // Clears FIFO
				FMode = RHModeRx;
			}
		}

		void setModeTx()
		{
			if ( FMode != RHModeTx)
			{
				if ( Power >= 18)
				{
					// Set high power boost mode
					// Note that OCP defaults to ON so no need to change that.
					spiWrite(RH_RF69_REG_5A_TESTPA1, RH_RF69_TESTPA1_BOOST);
					spiWrite(RH_RF69_REG_5C_TESTPA2, RH_RF69_TESTPA2_BOOST);
				}

				spiWrite(RH_RF69_REG_25_DIOMAPPING1, RH_RF69_DIOMAPPING1_DIO0MAPPING_00); // Set interrupt line 0 PacketSent
				setOpMode(RH_RF69_OPMODE_MODE_TX); // Clears FIFO
				FMode = RHModeTx;
			}
		}

		virtual uint8_t spiRead(uint8_t reg) = 0;

		virtual uint8_t spiWrite(uint8_t reg, uint8_t val) = 0;

/*
		uint8_t spiBurstRead(uint8_t reg, uint8_t* dest, uint8_t len)
		{
			uint8_t status = 0;
			FSPI.beginTransaction( SPISettings() );
			ChipSelectOutputPin.SendValue( false );
			status = FSPI.transfer(reg & ~0x80); // Send the start address with the write mask off
			while (len--)
				*dest++ = FSPI.transfer(0);

			ChipSelectOutputPin.SendValue( true );
			FSPI.endTransaction();
			return status;
		}
*/
		virtual uint8_t spiBurstWrite(uint8_t reg, const uint8_t* src, uint8_t len) = 0;

		void setOpMode(uint8_t mode)
		{
			uint8_t opmode = spiRead(RH_RF69_REG_01_OPMODE);
			opmode &= ~RH_RF69_OPMODE_MODE;
			opmode |= (mode & RH_RF69_OPMODE_MODE);
			spiWrite(RH_RF69_REG_01_OPMODE, opmode);

			FModeChange = true;

			// Wait for mode to change.
//			while (!(spiRead(RH_RF69_REG_27_IRQFLAGS1) & RH_RF69_IRQFLAGS1_MODEREADY))
//				;
		}

	public:
		void UpdateSyncWords()
		{
			uint8_t syncconfig = spiRead(RH_RF69_REG_2E_SYNCCONFIG);
			if ( SyncWords._BytesSize )
			{
//				uint8_t *ABuffer = new uint8_t[ SyncWords._BytesSize ];
//				memcpy_P( ABuffer, SyncWords._Bytes, SyncWords._BytesSize );
//				spiBurstWrite(RH_RF69_REG_2F_SYNCVALUE1, ABuffer, SyncWords._BytesSize );
//				delete [] ABuffer;
				spiBurstWrite(RH_RF69_REG_2F_SYNCVALUE1, SyncWords._Bytes, SyncWords._BytesSize );
				syncconfig |= RH_RF69_SYNCCONFIG_SYNCON;
			}
			else
				syncconfig &= ~RH_RF69_SYNCCONFIG_SYNCON;

			syncconfig &= ~RH_RF69_SYNCCONFIG_SYNCSIZE;
			if( SyncWords._BytesSize )
				syncconfig |= ( SyncWords._BytesSize - 1) << 3;

			spiWrite(RH_RF69_REG_2E_SYNCCONFIG, syncconfig);
		}

		void UpdatePreambleLength()
		{
			spiWrite(RH_RF69_REG_2C_PREAMBLEMSB, PreambleLength >> 8);
			spiWrite(RH_RF69_REG_2D_PREAMBLELSB, PreambleLength & 0xff);
		}

		void UpdateFrequency()
		{
			// Frf = FRF / FSTEP
			uint32_t frf = (Frequency * 1000000.0) / RH_RF69_FSTEP;
//			Serial.println( frf );
			spiWrite(RH_RF69_REG_07_FRFMSB, (frf >> 16) & 0xff);
			spiWrite(RH_RF69_REG_08_FRFMID, (frf >> 8) & 0xff);
			spiWrite(RH_RF69_REG_09_FRFLSB, frf & 0xff);
		}

		void UpdateEncryptionKey()
		{
			if ( EncryptionKey )
			{
//				uint8_t *ABuffer = new uint8_t[ 16 ];
//				memcpy_P( ABuffer, EncryptionKey, 16 );
//				spiBurstWrite( RH_RF69_REG_3E_AESKEY1, ABuffer, 16 );
//				delete [] ABuffer;
				spiBurstWrite( RH_RF69_REG_3E_AESKEY1, EncryptionKey, 16 );
				spiWrite(RH_RF69_REG_3D_PACKETCONFIG2, spiRead(RH_RF69_REG_3D_PACKETCONFIG2) | RH_RF69_PACKETCONFIG2_AESON);
			}
			else
				spiWrite(RH_RF69_REG_3D_PACKETCONFIG2, spiRead(RH_RF69_REG_3D_PACKETCONFIG2) & ~RH_RF69_PACKETCONFIG2_AESON);
		}

		void UpdateTransmitPower()
		{
			uint8_t palevel;
			if ( Power < -18)
				Power = -18;

			// See http://www.hoperf.com/upload/rfchip/RF69-V1.2.pdf section 3.3.6
			// for power formulas
			if (Power <= 13)
			{
				// -18dBm to +13dBm
				palevel = RH_RF69_PALEVEL_PA0ON | ((Power + 18) & RH_RF69_PALEVEL_OUTPUTPOWER);
			}
			else if (Power >= 18)
			{
				// +18dBm to +20dBm
				// Need PA1+PA2
				// Also need PA boost settings change when tx is turned on and off, see setModeTx()
				palevel = RH_RF69_PALEVEL_PA1ON | RH_RF69_PALEVEL_PA2ON | ((Power + 11) & RH_RF69_PALEVEL_OUTPUTPOWER);
			}
			else
			{
				// +14dBm to +17dBm
				// Need PA1+PA2
				palevel = RH_RF69_PALEVEL_PA1ON | RH_RF69_PALEVEL_PA2ON | ((Power + 14) & RH_RF69_PALEVEL_OUTPUTPOWER);
			}

			spiWrite(RH_RF69_REG_11_PALEVEL, palevel);
		}

		void UpdateAddress()
		{
			spiWrite( RH_RF69_REG_39_NODEADRS, Address );
			spiWrite( RH_RF69_REG_3A_BROADCASTADRS, BroadcastAddress );
		}

		// Sets registers from a canned modem configuration structure
		void UpdateModemRegisters()
		{
			const uint8_t CModulations[] =
			{
				RH_RF69_DATAMODUL_DATAMODE_PACKET | RH_RF69_DATAMODUL_MODULATIONTYPE_FSK | RH_RF69_DATAMODUL_MODULATIONSHAPING_FSK_BT1_0,
				RH_RF69_DATAMODUL_DATAMODE_PACKET | RH_RF69_DATAMODUL_MODULATIONTYPE_FSK | RH_RF69_DATAMODUL_MODULATIONSHAPING_FSK_BT0_5,
				RH_RF69_DATAMODUL_DATAMODE_PACKET | RH_RF69_DATAMODUL_MODULATIONTYPE_FSK | RH_RF69_DATAMODUL_MODULATIONSHAPING_FSK_BT0_3,

				RH_RF69_DATAMODUL_DATAMODE_PACKET | RH_RF69_DATAMODUL_MODULATIONTYPE_OOK | RH_RF69_DATAMODUL_MODULATIONSHAPING_OOK_NONE,
				RH_RF69_DATAMODUL_DATAMODE_PACKET | RH_RF69_DATAMODUL_MODULATIONTYPE_OOK | RH_RF69_DATAMODUL_MODULATIONSHAPING_OOK_BR,
				RH_RF69_DATAMODUL_DATAMODE_PACKET | RH_RF69_DATAMODUL_MODULATIONTYPE_OOK | RH_RF69_DATAMODUL_MODULATIONSHAPING_OOK_2BR
			};

			const uint8_t ChannelFilterBandwidths[] =
			{
				0b10000 | 7,
				0b01000 | 7,
				0b00000 | 7,
				0b10000 | 6,
				0b01000 | 6,
				0b00000 | 6,
				0b10000 | 5,
				0b01000 | 5,
				0b00000 | 5,
				0b10000 | 4,
				0b01000 | 4,
				0b00000 | 4,
				0b10000 | 3,
				0b01000 | 3,
				0b00000 | 3,
				0b10000 | 2,
				0b01000 | 2,
				0b00000 | 2,
				0b10000 | 1,
				0b01000 | 1,
				0b00000 | 1,
				0b10000 | 0,
				0b01000 | 0,
				0b00000 | 0
			};

			spiWrite( RH_RF69_REG_02_DATAMODUL, CModulations[ ModemSettings.Modulation ] );
			spiWrite( RH_RF69_REG_03_BITRATEMSB, ModemSettings.BitRate >> 8 );
			spiWrite( RH_RF69_REG_04_BITRATELSB, ModemSettings.BitRate );

			uint32_t frf = ( ModemSettings.FrequencyDeviation * 1000000.0) / RH_RF69_FSTEP;
			spiWrite( RH_RF69_REG_05_FDEVMSB, frf >> 8 );
			spiWrite( RH_RF69_REG_06_FDEVLSB, frf );

			uint8_t AValue = ModemSettings.DCCCutoffFrequency << 5 | ChannelFilterBandwidths[ ModemSettings.ChannelFilterBandwidth ];
			spiWrite( RH_RF69_REG_19_RXBW, AValue );

			AValue = RH_RF69_PACKETCONFIG1_PACKETFORMAT_VARIABLE | uint8_t( ModemSettings.DCFreeEncoding ) << 5 | ( ModemSettings.CRCEnabled ? RH_RF69_PACKETCONFIG1_CRC_ON : 0 );
			if( ! Promiscuous )
				AValue |= RH_RF69_PACKETCONFIG1_ADDRESSFILTERING_NODE_BC;

			spiWrite( RH_RF69_REG_37_PACKETCONFIG1, AValue );
		}

	protected:
		// Low level function reads the FIFO and checks the address
		// Caution: since we put our headers in what the RH_RF69 considers to be the payload, if encryption is enabled
		// we have to suffer the cost of decryption before we can determine whether the address is acceptable. 
		// Performance issue?
		virtual void readFifo() = 0;

		virtual void Reset() = 0;

	public:
		virtual void send( uint8_t ATxHeaderTo, const uint8_t* data, uint8_t len ) = 0;

	protected:
		virtual void SystemInit() override
		{
//			inherited::SystemInit();

			Reset();

			setModeIdle();

			// Configure important RH_RF69 registers
			// Here we set up the standard packet format for use by the RH_RF69 library:
			// 4 bytes preamble
			// 2 SYNC words 2d, d4
			// 2 CRC CCITT octets computed on the header, length and data (this in the modem config data)
			// 0 to 60 bytes data
			// RSSI Threshold -114dBm
			// We dont use the RH_RF69s address filtering: instead we prepend our own headers to the beginning
			// of the RH_RF69 payload
			spiWrite(RH_RF69_REG_3C_FIFOTHRESH, RH_RF69_FIFOTHRESH_TXSTARTCONDITION_NOTEMPTY | 0x0f); // thresh 15 is default
			// RSSITHRESH is default
		//    spiWrite(RH_RF69_REG_29_RSSITHRESH, 220); // -110 dbM
			// SYNCCONFIG is default. SyncSize is set later by setSyncWords()
		//    spiWrite(RH_RF69_REG_2E_SYNCCONFIG, RH_RF69_SYNCCONFIG_SYNCON); // auto, tolerance 0
			// PAYLOADLENGTH is default
		//    spiWrite(RH_RF69_REG_38_PAYLOADLENGTH, RH_RF69_FIFO_SIZE); // max size only for RX
			// PACKETCONFIG 2 is default 
			spiWrite(RH_RF69_REG_6F_TESTDAGC, RH_RF69_TESTDAGC_CONTINUOUSDAGC_IMPROVED_LOWBETAOFF);
			// If high power boost set previously, disable it
			spiWrite(RH_RF69_REG_5A_TESTPA1, RH_RF69_TESTPA1_NORMAL);
			spiWrite(RH_RF69_REG_5C_TESTPA2, RH_RF69_TESTPA2_NORMAL);

			UpdateSyncWords();
			UpdateModemRegisters();
//			setModemConfig(GFSK_Rb250Fd250);
			UpdatePreambleLength();
			UpdateFrequency();
			UpdateEncryptionKey();
			UpdateTransmitPower();
			UpdateAddress();

//			Serial.println( "START1" );

/*
			// The following can be changed later by the user if necessary.
			// Set up default configuration
			uint8_t syncwords[] = { 0x2d, 0xd4 };
			setSyncWords(syncwords, sizeof(syncwords)); // Same as RF22's
			// Reasnably fast and reliable default speed and modulation
			setModemConfig(GFSK_Rb250Fd250);
			// 3 would be sufficient, but this is the same as RF22's
			setPreambleLength(4);
			// An innocuous ISM frequency, same as RF22's
			setFrequency(434.0);
			// No encryption
			setEncryptionKey(NULL);
			// +13dBm, same as power-on default
			setTxPower(13); 
*/
			if( OutputPin.IsConnected() )
				setModeRx();
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
//			inherited::SystemLoopBegin( currentMicros );

			if( FModeChange )
			{
				if( !(spiRead(RH_RF69_REG_27_IRQFLAGS1) & RH_RF69_IRQFLAGS1_MODEREADY) )
					return;

				FModeChange = false;
			}


			// Wait for mode to change.
//			while (!(spiRead(RH_RF69_REG_27_IRQFLAGS1) & RH_RF69_IRQFLAGS1_MODEREADY))
//				;

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
				uint8_t ASentTo = APacket[ 0 ];
				uint8_t ASize = APacket[ 1 ];
//				Serial.println( "RECEIVED" );
//				Serial.println( ASize );
				SentToOutputPin.SendValue<int32_t>( ASentTo );
				OutputPin.SendValue( Mitov::TDataBlock( ASize, APacket + 2 ));
				delete [] APacket;
			}

			if( FMode != RHModeTx )
			{
				if( FSendQueue.size() )
				{
					uint8_t *ABuffer = FSendQueue[ 0 ];
					FSendQueue.pop_front();
					send( ABuffer[ 0 ], ABuffer + 2, ABuffer[ 1 ] );
					delete [] ABuffer;
				}
			}

			if( FMode == RHModeIdle )
				if( OutputPin.IsConnected() )
					setModeRx();

			SignalStrengthOutputPin.SetValue( FLastRssi );
//			Serial.print( "FLastRssi: " );	Serial.println( FLastRssi );
		}

	public:
/*
		MitovBasicLoRaRFM69( BasicSPI &ASPI, int AInterruptPin, void (*AInterruptRoutine)() ) :
			FSPI( ASPI ),
//			FInterruptPin( AInterruptPin ),
//			Modulation( mFrequencyShiftKeying_Gaussian_1_0 ),
			Promiscuous( false ),
			FModeChange( false ),
			FMode( RHModeInitialising )
		{
			ModemSettings.Modulation = mFrequencyShiftKeying_Gaussian_1_0;
			ModemSettings.DCCCutoffFrequency = rfmDCCCutoff4;
			ModemSettings.ChannelFilterBandwidth = bfFSK_500_0_OOK_250_0;
			ModemSettings.DCFreeEncoding = dcfeNone;
			ModemSettings.CRCEnabled = true;

			// Add by Adrien van den Bossche <vandenbo@univ-tlse2.fr> for Teensy
			// ARM M4 requires the below. else pin interrupt doesn't work properly.
			// On all other platforms, its innocuous, belt and braces
			pinMode( AInterruptPin, INPUT ); 

			int AInterruptNumber = digitalPinToInterrupt( AInterruptPin );
			attachInterrupt( AInterruptNumber, AInterruptRoutine, RISING );

			FSPI.usingInterrupt( AInterruptNumber );

			ChipSelectOutputPin.SendValue( true );
		}
*/
		MitovBasicLoRaRFM69() :
//			FInterruptPin( AInterruptPin ),
//			Modulation( mFrequencyShiftKeying_Gaussian_1_0 ),
			Promiscuous( false ),
			FModeChange( false ),
			FMode( RHModeInitialising )
		{
			ModemSettings.Modulation = mFrequencyShiftKeying_Gaussian_1_0;
			ModemSettings.DCCCutoffFrequency = rfmDCCCutoff4;
			ModemSettings.ChannelFilterBandwidth = bfFSK_500_0_OOK_250_0;
			ModemSettings.DCFreeEncoding = dcfeNone;
			ModemSettings.CRCEnabled = true;
		}

	};
//---------------------------------------------------------------------------
	class MitovLoRaSendPacket
	{
	public:
		uint8_t	Address = 0;

	protected:
		MitovBasicLoRaRFM69 &FOwner;

	public:
		void Print( String AValue )
		{
//			Serial.println( "Print" );
			AValue += "\r\n";
			FOwner.send( Address, (uint8_t *)AValue.c_str(), AValue.length() );
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
			FOwner.send( Address, (uint8_t*)&AValue, 1 );
		}

		void PrintChar( byte AValue )
		{
			FOwner.send( Address, &AValue, 1 );
		}

		void Write( uint8_t *AData, uint32_t ASize )
		{
			FOwner.send( Address, AData, ASize );
		}

	public:
		MitovLoRaSendPacket( MitovBasicLoRaRFM69 &AOwner ) :
			FOwner( AOwner )
		{		
		}
	};
//---------------------------------------------------------------------------
	class MitovLoRaRFM69 : public MitovBasicLoRaRFM69
	{
		typedef	MitovBasicLoRaRFM69	inherited;

	public:
		OpenWire::SourcePin	ChipSelectOutputPin;
		OpenWire::SourcePin	ResetOutputPin;

	protected:
		BasicSPI	&FSPI;
		int			FInterruptPin;

	protected:
		virtual uint8_t spiRead(uint8_t reg) override
		{
			FSPI.beginTransaction( SPISettings() );
			ChipSelectOutputPin.SendValue( false );
			FSPI.transfer(reg & ~0x80); // Send the address with the write mask off
			uint8_t val = FSPI.transfer(0); // The written value is ignored, reg value is read
			ChipSelectOutputPin.SendValue( true );
			FSPI.endTransaction();
			return val;
		}

		virtual uint8_t spiWrite(uint8_t reg, uint8_t val) override
		{
//Serial.print( "spiWrite: " ); Serial.print( reg, HEX ); Serial.print( " = " ); Serial.println( val, HEX );
			uint8_t status = 0;
			FSPI.beginTransaction( SPISettings() );
			ChipSelectOutputPin.SendValue( false );
			status = FSPI.transfer(reg | 0x80); // Send the address with the write mask on
			FSPI.transfer(val); // New value follows
			ChipSelectOutputPin.SendValue( true );
			FSPI.endTransaction();
			return status;
		}

		virtual uint8_t spiBurstWrite( uint8_t reg, const uint8_t* src, uint8_t len ) override
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
			uint8_t status = 0;
			FSPI.beginTransaction( SPISettings() );
			ChipSelectOutputPin.SendValue( false );
			status = FSPI.transfer(reg | 0x80); // Send the start address with the write mask on
			while (len--)
				FSPI.transfer(*src++);

			ChipSelectOutputPin.SendValue( true );
			FSPI.endTransaction();
			return status;
		}

		// Low level function reads the FIFO and checks the address
		// Caution: since we put our headers in what the RH_RF69 considers to be the payload, if encryption is enabled
		// we have to suffer the cost of decryption before we can determine whether the address is acceptable. 
		// Performance issue?
		virtual void readFifo() override
		{
			FSPI.beginTransaction( SPISettings() );
			ChipSelectOutputPin.SendValue( false );
			FSPI.transfer(RH_RF69_REG_00_FIFO); // Send the start address with the write mask off
			uint8_t payloadlen = FSPI.transfer(0); // First byte is payload len (counting the headers)
//			if ( ( payloadlen <= RH_RF69_MAX_ENCRYPTABLE_PAYLOAD_LEN ) && ( payloadlen >= RH_RF69_HEADER_LEN ) )
			if ((( EncryptionKey == nullptr ) && ( payloadlen <= RH_RF69_MAX_PAYLOAD_LEN ) ) || ( payloadlen <= RH_RF69_MAX_ENCRYPTABLE_PAYLOAD_LEN ))
			{
				uint8_t ARxHeaderTo = FSPI.transfer(0);
				// Check addressing
//				if ( Promiscuous ||
//					ARxHeaderTo == Address ||
//					ARxHeaderTo == BroadcastAddress )
				{
					uint8_t *ABuffer = new uint8_t[ 2 + payloadlen ];
					FReceivedPackets.push_back( ABuffer );
					*ABuffer++ = ARxHeaderTo;
					*ABuffer++ = payloadlen - 1; // Skip the ARxHeaderTo
					for ( int i = 0; i < payloadlen - 1; ++i )
						*ABuffer++ = FSPI.transfer(0);
/*
					uint8_t *ABuffer = new uint8_t[ 1 + (payloadlen - RH_RF69_HEADER_LEN) + 3 ];
					FReceivedPackets.push_back( ABuffer );
					*ABuffer++ = payloadlen;
					for ( int i = 0; i < (payloadlen - RH_RF69_HEADER_LEN) + 3; ++i )
						*ABuffer++ =FSPI.transfer(0);
*/
					/*
					// Get the rest of the headers
					_rxHeaderFrom  = FSPI.transfer(0);
					_rxHeaderId    = FSPI.transfer(0);
					_rxHeaderFlags = FSPI.transfer(0);
					// And now the real payload
					for (_bufLen = 0; _bufLen < (payloadlen - RH_RF69_HEADER_LEN); _bufLen++)
						_buf[_bufLen] = FSPI.transfer(0);
*/
//					_rxGood++;
//					_rxBufValid = true;
				}
			}

			ChipSelectOutputPin.SendValue( true );
			FSPI.endTransaction();
			// Any junk remaining in the FIFO will be cleared next time we go to receive mode.
		}

		virtual void Reset() override
		{
			ChipSelectOutputPin.SendValue( true );

			if( ResetOutputPin.IsConnected() )
			{
//				Serial.println( "RESET" );
				ResetOutputPin.SendValue( true );
				delay( 100 );
				ResetOutputPin.SendValue( false );
				delay( 100 );
			}
		}

	public:
		virtual void send( uint8_t ATxHeaderTo, const uint8_t* data, uint8_t len ) override
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
					;

				uint8_t *ABuffer = new uint8_t[ len + 2 ];
				ABuffer[ 0 ] = ATxHeaderTo;
				ABuffer[ 1 ] = len;
				memcpy( ABuffer + 2, data, len );
				FSendQueue.push_back( ABuffer );
				return;
			}

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
		}

	protected:
		virtual void SystemInit() override
		{
//			Serial.println( "SystemInit" );
			int AInterruptNumber = digitalPinToInterrupt( FInterruptPin );
			FSPI.usingInterrupt( AInterruptNumber );

			ChipSelectOutputPin.SendValue( true );

			inherited::SystemInit();
		}

	public:
		MitovLoRaRFM69( BasicSPI &ASPI, int AInterruptPin ) :
			FSPI( ASPI ),
			FInterruptPin( AInterruptPin )
		{
		}

	};
//---------------------------------------------------------------------------
	template<int PIN_CS, int PIN_RST, int PIN_INT> class MitovLoRaRFM69Shield : public MitovBasicLoRaRFM69
	{
		typedef	MitovBasicLoRaRFM69	inherited;

	protected:
		static MitovLoRaRFM69Shield<PIN_CS,PIN_RST,PIN_INT>	*FInstnce;

	protected:
		virtual uint8_t spiRead(uint8_t reg) override
		{
			uint8_t val;
			SPI.beginTransaction( SPISettings() );
			digitalWrite( PIN_CS, LOW );
			SPI.transfer(reg & ~0x80); // Send the address with the write mask off
			val = SPI.transfer(0); // The written value is ignored, reg value is read
			digitalWrite( PIN_CS, HIGH );
			SPI.endTransaction();
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

		virtual uint8_t spiBurstWrite( uint8_t reg, const uint8_t* src, uint8_t len ) override
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
			SPI.beginTransaction( SPISettings() );
			digitalWrite( PIN_CS, LOW );
			uint8_t status = SPI.transfer(reg | 0x80); // Send the start address with the write mask on
			while (len--)
				SPI.transfer(*src++);

			digitalWrite( PIN_CS, HIGH );
			SPI.endTransaction();
			return status;
		}

		// Low level function reads the FIFO and checks the address
		// Caution: since we put our headers in what the RH_RF69 considers to be the payload, if encryption is enabled
		// we have to suffer the cost of decryption before we can determine whether the address is acceptable. 
		// Performance issue?
		virtual void readFifo() override
		{
			SPI.beginTransaction( SPISettings() );
			digitalWrite( PIN_CS, LOW );
			SPI.transfer(RH_RF69_REG_00_FIFO); // Send the start address with the write mask off
			uint8_t payloadlen = SPI.transfer(0); // First byte is payload len (counting the headers)
//			if ( ( payloadlen <= RH_RF69_MAX_ENCRYPTABLE_PAYLOAD_LEN ) && ( payloadlen >= RH_RF69_HEADER_LEN ) )
			if ((( EncryptionKey == nullptr ) && ( payloadlen <= RH_RF69_MAX_PAYLOAD_LEN ) ) || ( payloadlen <= RH_RF69_MAX_ENCRYPTABLE_PAYLOAD_LEN ))
			{
				uint8_t ARxHeaderTo = SPI.transfer(0);
				// Check addressing
//				if ( Promiscuous ||
//					ARxHeaderTo == Address ||
//					ARxHeaderTo == BroadcastAddress )
				{
					uint8_t *ABuffer = new uint8_t[ 2 + payloadlen ];
					FReceivedPackets.push_back( ABuffer );
					*ABuffer++ = ARxHeaderTo;
					*ABuffer++ = payloadlen - 1; // Skip the ARxHeaderTo
					for ( int i = 0; i < payloadlen - 1; ++i )
						*ABuffer++ = SPI.transfer(0);
/*
					uint8_t *ABuffer = new uint8_t[ 1 + (payloadlen - RH_RF69_HEADER_LEN) + 3 ];
					FReceivedPackets.push_back( ABuffer );
					*ABuffer++ = payloadlen;
					for ( int i = 0; i < (payloadlen - RH_RF69_HEADER_LEN) + 3; ++i )
						*ABuffer++ =SPI.transfer(0);
*/
					/*
					// Get the rest of the headers
					_rxHeaderFrom  = SPI.transfer(0);
					_rxHeaderId    = SPI.transfer(0);
					_rxHeaderFlags = SPI.transfer(0);
					// And now the real payload
					for (_bufLen = 0; _bufLen < (payloadlen - RH_RF69_HEADER_LEN); _bufLen++)
						_buf[_bufLen] = SPI.transfer(0);
*/
//					_rxGood++;
//					_rxBufValid = true;
				}
			}

			digitalWrite( PIN_CS, HIGH );
			SPI.endTransaction();
			// Any junk remaining in the FIFO will be cleared next time we go to receive mode.
		}

		virtual void Reset() override
		{
			digitalWrite( PIN_CS, HIGH );

//				Serial.println( "RESET" );
			digitalWrite( PIN_RST, LOW );
			delay( 100 );
			digitalWrite( PIN_RST, HIGH );
			delay( 100 );
		}

	public:
		virtual void send( uint8_t ATxHeaderTo, const uint8_t* data, uint8_t len ) override
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
					;

				uint8_t *ABuffer = new uint8_t[ len + 2 ];
				ABuffer[ 0 ] = ATxHeaderTo;
				ABuffer[ 1 ] = len;
				memcpy( ABuffer + 2, data, len );
				FSendQueue.push_back( ABuffer );
				return;
			}

			setModeIdle(); // Prevent RX while filling the fifo
//			Serial.println( "SEND" );

			SPI.beginTransaction( SPISettings() );
			digitalWrite( PIN_CS, LOW );
			SPI.transfer( RH_RF69_REG_00_FIFO | 0x80 ); // Send the start address with the write mask on
			SPI.transfer( len + 1 ); // Include length of headers
			// First the 4 headers
			SPI.transfer( ATxHeaderTo );
//			SPI.transfer( Address );
//			SPI.transfer(_txHeaderId);
//			SPI.transfer(_txHeaderFlags);
			// Now the payload
			while( len-- )
				SPI.transfer( *data++ );

			digitalWrite( PIN_CS, HIGH );
			SPI.endTransaction();

			setModeTx(); // Start the transmitter
		}

	protected:
		static void StaticInterruptHandler()
		{
			FInstnce->InterruptHandler( true );
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
		MitovLoRaRFM69Shield()
		{
			FInstnce = this;
			pinMode( PIN_RST, OUTPUT );
			pinMode( PIN_CS, OUTPUT );
		}

	};
//---------------------------------------------------------------------------
	template<int PIN_CS, int PIN_RST, int PIN_INT> MitovLoRaRFM69Shield<PIN_CS,PIN_RST,PIN_INT>	*MitovLoRaRFM69Shield<PIN_CS,PIN_RST,PIN_INT>::FInstnce = nullptr;
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
