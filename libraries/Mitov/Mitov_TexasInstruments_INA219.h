////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_TEXAS_INSTRUMENTS_INA219_h
#define _MITOV_TEXAS_INSTRUMENTS_INA219_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	enum TINA219ResolutionAveragingMode
	{
      ram9_Bit,
      ram10_Bit,
      ram11_Bit,
      ram12_Bit,
      ram2_Samples,
      ram4_Samples,
      ram8_Samples,
      ram16_Samples,
      ram32_Samples,
      ram64_Samples,
      ram128_Samples
	};
//---------------------------------------------------------------------------
	class TexasInstrumentsINA219 : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
#ifdef _MITOV_TEXAS_INSTRUMENTS_INA219_CLOCK_PIN_
		OpenWire::ConnectSinkPin		ClockInputPin;
#endif

#ifdef _MITOV_TEXAS_INSTRUMENTS_INA219_RESET_PIN_
		OpenWire::SinkPin				ResetInputPin;
#endif
		OpenWire::TypedSourcePin<float>	OutputPins[ 4 ];

//#ifdef _MITOV_TEXAS_INSTRUMENTS_INA219_OVERFLOW_PIN_
//		OpenWire::TypedSourcePin<bool>	OverflowOutputPin;
//#endif

	public:
		float	ShuntResistor = 0.1;
//		uint16_t	FCalibrationValue;

		TINA219ResolutionAveragingMode	BusVoltageMode : 4;
		TINA219ResolutionAveragingMode	ShuntVoltageMode : 4;

		bool	Enabled : 1;
		bool	Bus32VoltsRange : 1;
		bool	SingleShot : 1;
		uint8_t	ShuntVoltageGainOrder : 2;


	protected:
		uint8_t	FAddress : 4;

		TwoWire &FWire;

	protected:
		static const uint8_t INA219_REG_CONFIG			= 0x00;
		static const uint8_t INA219_REG_SHUNTVOLTAGE	= 0x01;
		static const uint8_t INA219_REG_BUSVOLTAGE		= 0x02;
		static const uint8_t INA219_REG_POWER			= 0x03;
		static const uint8_t INA219_REG_CURRENT			= 0x04;
		static const uint8_t INA219_REG_CALIBRATION		= 0x05;

		enum TPinIndexes
		{
			piPOWER,
			piBUS_VOLTAGE,
			piCURRENT,
			piSHUNT_VOLTAGE
		};

	protected:
		void IntUpdateConfig( bool AReset )
		{
			uint8_t mode;
			bool ANeedsShunt = OutputPins[ piPOWER ].IsConnected() || OutputPins[ piSHUNT_VOLTAGE ].IsConnected() || OutputPins[ piCURRENT ].IsConnected();
			bool ANeedsBus = OutputPins[ piPOWER ].IsConnected() || OutputPins[ piBUS_VOLTAGE ].IsConnected();

#ifdef _MITOV_TEXAS_INSTRUMENTS_INA219_CLOCK_PIN_
			if( SingleShot && ClockInputPin.IsConnected() )
			{
				if( ANeedsShunt && ANeedsBus )
					mode = 0b11;

				else if( ANeedsShunt )
					mode = 0b01;

				else
					mode = 0b10;
			}

			else
#endif
			{
				if( ANeedsShunt && ANeedsBus )
					mode = 0b111;

				else if( ANeedsShunt )
					mode = 0b101;

				else
					mode = 0b110;
			}

//			mode = 0b111; // For now continuous

			uint16_t config =	(( AReset )				? 0b10000000 : 0 ) |
								(( Bus32VoltsRange )	? 0b00100000 : 0 ) |
								( uint16_t( ShuntVoltageGainOrder ) << 11 ) |
								( ResolutionToBits( BusVoltageMode ) << 7 ) |
								( ResolutionToBits( ShuntVoltageMode ) << 3 ) |
								mode;

//			Serial.println( "" );
//			Serial.println( config, BIN );
			wireWriteRegister( INA219_REG_CONFIG, config );
/*
			// Compute Callibration Register
			float VSHUNT_MAX = 0.04 * ( 1 << ShuntVoltageGainOrder );

//			Serial.println( "" );
//			Serial.println( "VSHUNT_MAX" );
//			Serial.println( VSHUNT_MAX );

			float MaxPossible_I = VSHUNT_MAX / ShuntResistor;

//			Serial.println( "MaxPossible_I" );
//			Serial.println( MaxPossible_I );

			float MinimumLSB = MaxPossible_I / 32767;
			float MaximumLSB = MaxPossible_I / 4096;

//			Serial.println( "MinimumLSB" );
//			Serial.println( MinimumLSB, 8 );

//			Serial.println( "MaximumLSB" );
//			Serial.println( MaximumLSB, 8 );

//			float ATest = MinimumLSB * 10000;
//			Serial.println( "ATest" );
//			Serial.println( ATest, 8 );

//			float ATest1 = MinimumLSB / 0.0001;
//			Serial.println( "ATest1" );
//			Serial.println( ATest1, 8 );

// 4. Choose an LSB between the min and max values
//    (Preferrably a roundish number close to MinLSB)
			float AMultiplier = 1;
			float CurrentLSB = MinimumLSB;
			while( CurrentLSB * AMultiplier < 1.0 )
				AMultiplier *= 10;

//			Serial.println( "AMultiplier" );
//			Serial.println( AMultiplier );

			CurrentLSB = trunc( CurrentLSB * AMultiplier );
//			Serial.println( "CurrentLSB" );
//			Serial.println( CurrentLSB, 8 );

			while( CurrentLSB / AMultiplier < MinimumLSB )
				CurrentLSB += 1;

//			Serial.println( "CurrentLSB" );
//			Serial.println( CurrentLSB, 8 );

			CurrentLSB /= AMultiplier;

// 5. Compute the calibration register
			FCalibrationValue = trunc(0.04096 / (CurrentLSB * ShuntResistor ));

//			Serial.println( "FCalibrationValue" );
//			Serial.println( FCalibrationValue );

// 6. Calculate the power LSB
			float PowerLSB = 20 * CurrentLSB;

			wireWriteRegister(INA219_REG_CALIBRATION, FCalibrationValue );
*/
		}

		uint16_t	ResolutionToBits( TINA219ResolutionAveragingMode AMode )
		{
			static const uint8_t AResults[] =
			{
				0b0000, // ram9_Bit
				0b0001, // ram10_Bit
				0b0010, // ram11_Bit
				0b0011, // ram12_Bit
				0b1001, // ram2_Samples
				0b1010, // ram4_Samples
				0b1011, // ram8_Samples
				0b1100, // ram16_Samples
				0b1101, // ram32_Samples
				0b1110, // ram64_Samples
				0b1111  // ram128_Samples
			};

			return AResults[ AMode ];
		}

	public:
		void UpdateConfig()
		{
			IntUpdateConfig( false );
		}

	protected:
		uint16_t wireReadRegister(uint8_t reg )
		{
			uint8_t	AAddress = 0x40 | FAddress;
			FWire.beginTransmission( AAddress );
			FWire.write(reg);                       // Register
			FWire.endTransmission();
  
//			delay(1); // Max 12-bit conversion time is 586us per sample

			FWire.requestFrom( AAddress, (uint8_t)2);  
			// Shift values to create properly formed integer
			uint16_t AValue = FWire.read();
			return (( AValue << 8 ) | FWire.read());
		}

		void wireWriteRegister(uint8_t reg, uint16_t value)
		{
			FWire.beginTransmission( 0x40 | FAddress );
			FWire.write(reg);                       // Register
			FWire.write((value >> 8) & 0xFF);       // Upper 8-bits
			FWire.write(value & 0xFF);              // Lower 8-bits
			FWire.endTransmission();
		}

	protected:
#ifdef _MITOV_TEXAS_INSTRUMENTS_INA219_RESET_PIN_
		void DoResetReceive( void *_Data )
		{
			IntUpdateConfig( true );
		}
#endif

		void DoClockReceive( void *_Data )
		{
			ReadData( SingleShot );
		}

		void ReadData( bool ASingleShot )
		{
			if( ! Enabled )
				return;

			if( ASingleShot )
				IntUpdateConfig( false ); // Force reading!

			bool APowerConnected = OutputPins[ piPOWER ].IsConnected();
			bool ACurrentConnected = OutputPins[ piCURRENT ].IsConnected();
			bool AShuntVoltageConnected = OutputPins[ piSHUNT_VOLTAGE ].IsConnected();

			bool ABusVoltageConnected = OutputPins[ piBUS_VOLTAGE ].IsConnected()
//#ifdef _MITOV_TEXAS_INSTRUMENTS_INA219_OVERFLOW_PIN_
//				|| OverflowOutputPin.IsConnected()
//#endif
				;

			bool ANeedsShuntVoltage = AShuntVoltageConnected | ACurrentConnected | APowerConnected;
			bool ANeedsCurrent = ACurrentConnected | APowerConnected;
			bool ANeedsBusVoltage = APowerConnected | ABusVoltageConnected | ASingleShot;

/*
			if( APowerConnected || ACurrentConnected )
			{
				// Sometimes a sharp load will reset the INA219, which will
				// reset the cal register, meaning CURRENT and POWER will
				// not be available ... avoid this by always setting a cal
				// value even if it's an unfortunate extra step
				wireWriteRegister(INA219_REG_CALIBRATION, FCalibrationValue );
			}
*/
			uint16_t ARawBusValue;
			float ABusVoltage;
			if( ANeedsBusVoltage )
			{
				do
				{
					ARawBusValue = wireReadRegister( INA219_REG_BUSVOLTAGE );
				}
				while( ASingleShot && (( ARawBusValue & 0b10 ) == 0 ) );

				ABusVoltage = ( ARawBusValue >> 1 ) & 0b1111111111111100;
				ABusVoltage *= 0.001;
			}

			float AShuntVoltage;
			float ACurrent;
			if( ANeedsShuntVoltage )
			{
				AShuntVoltage = wireReadRegister( INA219_REG_SHUNTVOLTAGE );
				AShuntVoltage *= 0.01;

				if( ANeedsCurrent )
					ACurrent = ( AShuntVoltage / ShuntResistor );
			}

			if( APowerConnected ) // Power
			{
//				float AValue = wireReadRegister( INA219_REG_POWER );
				float AValue = ( ACurrent ) * ABusVoltage;
				OutputPins[ piPOWER ].Notify( &AValue );
			}

			if( ABusVoltageConnected ) // Bus Voltage
			{
//				uint16_t ARawValue = wireReadRegister( INA219_REG_BUSVOLTAGE );
//#ifdef _MITOV_TEXAS_INSTRUMENTS_INA219_OVERFLOW_PIN_
//				OverflowOutputPin.SendValue( ( ARawBusValue & 1 ) != 0 );
//#endif
				OutputPins[ piBUS_VOLTAGE ].Notify( &ABusVoltage );
			}

			if( ACurrentConnected ) // Current
			{
				// Now we can safely read the CURRENT register!
//				float AValue = wireReadRegister( INA219_REG_CURRENT );
//				Serial.println( AShuntVoltage );
				OutputPins[ piCURRENT ].Notify( &ACurrent );
			}

			if( AShuntVoltageConnected ) // Shunt Voltage
			{
//				float AValue = wireReadRegister( INA219_REG_SHUNTVOLTAGE );
//				AValue *= 0.01;
				OutputPins[ piSHUNT_VOLTAGE ].Notify( &AShuntVoltage );
			}
		}

	protected:
		virtual void SystemStart() override
		{
//			inherited::SystemStart();

			UpdateConfig();
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
#ifdef _MITOV_TEXAS_INSTRUMENTS_INA219_CLOCK_PIN_
			if( ! ClockInputPin.IsConnected() )
#endif
				ReadData( false );

//			inherited::SystemLoopBegin( currentMicros );
		}

	public:
		TexasInstrumentsINA219( uint8_t AAddress, TwoWire &AWire ) :
			BusVoltageMode( ram12_Bit ),
			ShuntVoltageMode( ram12_Bit ),
			Enabled( true ),
			Bus32VoltsRange( true ),
			SingleShot( false ),
			ShuntVoltageGainOrder( 3 ),
			FAddress( AAddress & 0x0F ),
			FWire( AWire )
		{
#ifdef _MITOV_TEXAS_INSTRUMENTS_INA219_CLOCK_PIN_
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&TexasInstrumentsINA219::DoClockReceive );
#endif

#ifdef _MITOV_TEXAS_INSTRUMENTS_INA219_RESET_PIN_
			ResetInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&TexasInstrumentsINA219::DoResetReceive );
#endif
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
