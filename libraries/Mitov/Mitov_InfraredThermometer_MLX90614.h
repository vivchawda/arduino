////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_INFRARED_THERMOMETER_MLX90614_h
#define _MITOV_INFRARED_THERMOMETER_MLX90614_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	class TArduinoInfraredThermometerMLX90614Channel
	{
	public:
		OpenWire::TypedStartSourcePin<float>	OutputPin;
		OpenWire::TypedStartSourcePin<bool>		ErrorOutputPin;

	};
//---------------------------------------------------------------------------
	class InfraredThermometerMLX90614 : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
#ifdef _MITOV_MLX90614_CLOCK_PIN_
		OpenWire::ConnectSinkPin		ClockInputPin;
#endif
//		OpenWire::TypedStartSourcePin<float>	OutputPins[ 2 ];

#ifdef _MITOV_MLX90614_AMBIENT_PIN_
		OpenWire::TypedStartSourcePin<float>	AmbientOutputPin;
#endif

#ifdef _MITOV_MLX90614_EMISSIVITY_PIN_
		OpenWire::TypedStartSourcePin<float>	EmissivityOutputPin;
#endif

#ifdef _MITOV_MLX90614_RANGE_PINS_
		OpenWire::TypedStartSourcePin<float>	RangeOutputPins[ 2 ];
#endif

	public:
		TArduinoInfraredThermometerMLX90614Channel	Channels[ 2 ];

		bool	InFahrenheit = false;

	protected:
		uint8_t FAddress;
		TwoWire &FWire;

	public:
		static const uint16_t	I2C_READ_TIMEOUT = 1000;

		static const uint16_t MLX90614_REGISTER_TA		= 0x06;
		static const uint16_t MLX90614_REGISTER_TOBJ1	= 0x07;
		static const uint16_t MLX90614_REGISTER_TOBJ2	= 0x08;
		static const uint16_t MLX90614_REGISTER_TOMAX	= 0x20;
		static const uint16_t MLX90614_REGISTER_TOMIN	= 0x21;
		static const uint16_t MLX90614_REGISTER_PWMCTRL	= 0x22;
		static const uint16_t MLX90614_REGISTER_TARANGE = 0x23;
		static const uint16_t MLX90614_REGISTER_KE      = 0x24;
		static const uint16_t MLX90614_REGISTER_CONFIG  = 0x25;
		static const uint16_t MLX90614_REGISTER_ADDRESS = 0x2E;
		static const uint16_t MLX90614_REGISTER_ID0     = 0x3C;
		static const uint16_t MLX90614_REGISTER_ID1     = 0x3D;
		static const uint16_t MLX90614_REGISTER_ID2     = 0x3E;
		static const uint16_t MLX90614_REGISTER_ID3     = 0x3F;
		static const uint16_t MLX90614_COMMAND_SLEEP	= 0xFF;

	public:
		bool I2CReadWord(byte reg, int16_t &dest )
		{
			int timeout = I2C_READ_TIMEOUT;
	
			FWire.beginTransmission( FAddress );
			FWire.write(reg);
	
			FWire.endTransmission(false); // Send restart
			FWire.requestFrom( FAddress, (uint8_t) 3);
	
			while ((FWire.available() < 3) && (timeout-- > 0))
				delay(1);

			if (timeout <= 0)
				return false;
	
			uint8_t lsb = FWire.read();
			uint8_t msb = FWire.read();
			uint8_t pec = FWire.read();
	
			uint8_t crc = crc8(0, ( FAddress << 1));
			crc = crc8(crc, reg);
			crc = crc8(crc, ( FAddress << 1) + 1);
			crc = crc8(crc, lsb);
			crc = crc8(crc, msb);
	
			if (crc != pec)
				return false;

			dest = (msb << 8) | lsb;
			return true;
		}

		uint8_t I2CWriteWord(byte reg, int16_t data)
		{
			uint8_t crc;
			uint8_t lsb = data & 0x00FF;
			uint8_t msb = (data >> 8);
	
			crc = crc8(0, ( FAddress << 1));
			crc = crc8(crc, reg);
			crc = crc8(crc, lsb);
			crc = crc8(crc, msb);
	
			FWire.beginTransmission( FAddress );
			FWire.write(reg);
			FWire.write(lsb);
			FWire.write(msb);
			FWire.write(crc);
			return FWire.endTransmission(true);
		}

		bool writeEEPROM(byte reg, int16_t data)
		{	
			// Clear out EEPROM first:
			if (I2CWriteWord(reg, 0) != 0)
				return false; // If the write failed, return 0

			delay(5); // Delay tErase
	
			uint8_t i2cRet = I2CWriteWord(reg, data);
			delay(5); // Delay tWrite
	
			return (i2cRet == 0);
		}

		bool writeTemperatureEEPROM(byte reg, float AValue )
		{
			if( InFahrenheit )
				AValue = (AValue - 32.0) * 5.0 / 9.0;

			AValue = ( AValue + 273.15 ) * 50;
			int16_t rawTemp = (int16_t)AValue;

			writeEEPROM( reg, rawTemp );
		}

		void setAddress(uint8_t newAdd)
		{
			// Make sure the address is within the proper range:
			if ((newAdd >= 0x80) || (newAdd == 0x00))
				return; // Return fail if out of range

			int16_t tempAdd;

			// Read from the I2C address address first:
			if (I2CReadWord( MLX90614_REGISTER_ADDRESS, tempAdd))
			{
				tempAdd &= 0xFF00; // Mask out the address (MSB is junk?)
				tempAdd |= newAdd; // Add the new address
		
				// Write the new addres back to EEPROM:
				 writeEEPROM( MLX90614_REGISTER_ADDRESS, tempAdd );
			}	
		}

/*
		void setConfig( TArduinoInfraredThermometerMLX90614SetConfigBasic *AConfig )
		{
			static const uint8_t CIIRFilterValues[] = { 0, 0b111, 0b110, 0b101, 0b100 };
			static const float CGainCiefficients[] = 
			{
				1.0,
				3.0,
				6.0,
				12.5,
				25.0,
				50.0,
				100.0
			};

			uint16_t ACoeffOrder = MITOV_ARRAY_SIZE( CGainCiefficients ) - 1;
			for( int i = MITOV_ARRAY_SIZE( CGainCiefficients ); i--; )
			{
				if( AConfig->Gain > CGainCiefficients[ i ] )
					break;

				ACoeffOrder = i;

				if( AConfig->Gain == CGainCiefficients[ i ] )
					break;

			}

			uint16_t AValue =	CIIRFilterValues[ AConfig->IIRCoeficients ] |
								( AConfig->UsePositiveTemperatureCoefficientSensor ? 0b1000 : 0 ) |
								( uint16_t( AConfig->FIRCoeficientsOrder ) << 8 ) |
								( ACoeffOrder << 11 )
								( AConfig->NegativeThermoshockCompensation ? 0x8000 : 0 );

			uint16_t ARegisterValue;
			if( ! I2CReadWord( MLX90614_REGISTER_CONFIG, ARegisterValue ))
				return;

			ARegisterValue &=	( uint16_t( 1 ) << 14 ) | 
								( uint16_t( 1 ) << 7 ) |
								( uint16_t( 1 ) << 6 ) |
								( uint16_t( 1 ) << 5 ) |
								( uint16_t( 1 ) << 4 );

			ARegisterValue |= AValue;
//			writeEEPROM( MLX90614_REGISTER_CONFIG, ARegisterValue );
		}
*/
	protected:
		uint8_t crc8 (uint8_t inCrc, uint8_t inData)
		{
			uint8_t data = inCrc ^ inData;
			for ( uint8_t i = 0; i < 8; i++ )
			{
				if (( data & 0x80 ) != 0 )
				{
					data <<= 1;
					data ^= 0x07;
				}
				else
					data <<= 1;
			}

			return data;
		}

		float GetTemperature( int16_t ARawValue )
		{
			float AResult = float(ARawValue) * 0.02 - 273.15;
			if( InFahrenheit )
				return AResult * 9.0 / 5.0 + 32;

			return AResult;
		}

		void ReadSensor()
		{
			int16_t ARawValue;

//			if( Channels[ 0 ].OutputPin.IsConnected() || Channels[ 0 ].ErrorOutputPin.IsConnected() )
			{
				if (I2CReadWord( MLX90614_REGISTER_TOBJ1, ARawValue ))
				{
					// If the read succeeded
					if (ARawValue & 0x8000) // If there was a flag error
					{
						Channels[ 0 ].ErrorOutputPin.SendValue( true );
						return;	
					}

//					Serial.println( ARawValue );
					Channels[ 0 ].ErrorOutputPin.SendValue( false );
					Channels[ 0 ].OutputPin.SetValue( GetTemperature( ARawValue ));
				}
			}

			if( Channels[ 1 ].OutputPin.IsConnected() || Channels[ 1 ].ErrorOutputPin.IsConnected() )
			{
				if (I2CReadWord( MLX90614_REGISTER_TOBJ2, ARawValue ))
				{
					// If the read succeeded
					if (ARawValue & 0x8000) // If there was a flag error
					{
						Channels[ 1 ].ErrorOutputPin.SendValue( true );
						return;	
					}

					Channels[ 1 ].ErrorOutputPin.SendValue( false );
					Channels[ 1 ].OutputPin.SetValue( GetTemperature( ARawValue ));
				}
			}

//			if (I2CReadWord( MLX90614_REGISTER_CONFIG, ARawValue ))
//				Serial.println( ARawValue, HEX );


#ifdef _MITOV_MLX90614_AMBIENT_PIN_
			if (I2CReadWord( MLX90614_REGISTER_TA, ARawValue ))
				AmbientOutputPin.SetValue( GetTemperature( ARawValue ));
#endif

#ifdef _MITOV_MLX90614_EMISSIVITY_PIN_
			if (I2CReadWord( MLX90614_REGISTER_KE, ARawValue ))
			{
				float AValue = (((float)((uint16_t)ARawValue)) / 65535.0);
				EmissivityOutputPin.SetValue( AValue );
			}
#endif

#ifdef _MITOV_MLX90614_RANGE_PINS_
			if( RangeOutputPins[ 0 ].IsConnected() )
				if (I2CReadWord( MLX90614_REGISTER_TOMIN, ARawValue ))
					RangeOutputPins[ 0 ].SetValue( GetTemperature( ARawValue ));

			if( RangeOutputPins[ 1 ].IsConnected() )
				if (I2CReadWord( MLX90614_REGISTER_TOMAX, ARawValue ))
					RangeOutputPins[ 1 ].SetValue( GetTemperature( ARawValue ));
#endif
		}

#ifdef _MITOV_MLX90614_CLOCK_PIN_
		void DoClockReceive( void *_Data )
		{
			ReadSensor();
		}
#endif

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
#ifdef _MITOV_MLX90614_CLOCK_PIN_
			if( ! ClockInputPin.IsConnected() )
#endif
				ReadSensor();

//			inherited::SystemLoopBegin( currentMicros );
		}

	public:
		InfraredThermometerMLX90614( uint8_t AAddress, TwoWire &AWire ) :
			FAddress( AAddress ),
			FWire( AWire )
		{
#ifdef _MITOV_MLX90614_CLOCK_PIN_
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&InfraredThermometerMLX90614::DoClockReceive );
#endif
		}
	};
//---------------------------------------------------------------------------
	class TArduinoInfraredThermometerMLX90614SetEmissivityElement : public OpenWire::Component
	{
	protected:
		InfraredThermometerMLX90614	&FOwner;

	public:
		float Value = 1.0;

	protected:
		virtual void SystemStart() override
		{
			// Calculate the raw 16-bit value:
			uint16_t ke = uint16_t(65535.0 * Value );
			ke = constrain( ke, 0x2000, 0xFFFF );

			// Write that value to the ke register
			FOwner.writeEEPROM( InfraredThermometerMLX90614::MLX90614_REGISTER_KE, (int16_t)ke);		

//			inherited::SystemStart();
		}

	public:
		TArduinoInfraredThermometerMLX90614SetEmissivityElement( InfraredThermometerMLX90614 &AOwner ) :
			FOwner( AOwner )
		{
		}
	};
//---------------------------------------------------------------------------
	class TArduinoInfraredThermometerMLX90614SetEmissivityElement_Clocked : public OpenWire::Object
	{
	public:
		OpenWire::ConnectSinkPin	ClockInputPin;

	public:
		float Value = 1.0;

	protected:
		InfraredThermometerMLX90614	&FOwner;

	protected:
		void DoClockReceive( void *_Data )
		{
			// Calculate the raw 16-bit value:
			uint16_t ke = uint16_t(65535.0 * Value );
			ke = constrain( ke, 0x2000, 0xFFFF );

			// Write that value to the ke register
			FOwner.writeEEPROM( InfraredThermometerMLX90614::MLX90614_REGISTER_KE, (int16_t)ke);		
		}

	public:
		TArduinoInfraredThermometerMLX90614SetEmissivityElement_Clocked( InfraredThermometerMLX90614 &AOwner ) :
			FOwner( AOwner )
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&TArduinoInfraredThermometerMLX90614SetEmissivityElement_Clocked::DoClockReceive );
		}

	};
//---------------------------------------------------------------------------
	class TArduinoInfraredThermometerMLX90614SetRangeMinElement : public OpenWire::Component
	{
	protected:
		InfraredThermometerMLX90614	&FOwner;

	public:
		float Value = 0.0;

	protected:
		virtual void SystemStart() override
		{
			FOwner.writeTemperatureEEPROM( InfraredThermometerMLX90614::MLX90614_REGISTER_TOMIN, Value );

//			inherited::SystemStart();
		}

	public:
		TArduinoInfraredThermometerMLX90614SetRangeMinElement( InfraredThermometerMLX90614 &AOwner ) :
			FOwner( AOwner )
		{
		}
	};
//---------------------------------------------------------------------------
	class TArduinoInfraredThermometerMLX90614SetRangeMinElement_Clocked : public OpenWire::Object
	{
	public:
		OpenWire::ConnectSinkPin	ClockInputPin;

	public:
		float Value = 0.0;

	protected:
		InfraredThermometerMLX90614	&FOwner;

	protected:
		void DoClockReceive( void *_Data )
		{
			FOwner.writeTemperatureEEPROM( InfraredThermometerMLX90614::MLX90614_REGISTER_TOMIN, Value );
		}

	public:
		TArduinoInfraredThermometerMLX90614SetRangeMinElement_Clocked( InfraredThermometerMLX90614 &AOwner ) :
			FOwner( AOwner )
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&TArduinoInfraredThermometerMLX90614SetRangeMinElement_Clocked::DoClockReceive );
		}

	};
//---------------------------------------------------------------------------
	class TArduinoInfraredThermometerMLX90614SetRangeMaxElement : public OpenWire::Component
	{
	protected:
		InfraredThermometerMLX90614	&FOwner;

	public:
		float Value = 100.0;

	protected:
		virtual void SystemStart() override
		{
			FOwner.writeTemperatureEEPROM( InfraredThermometerMLX90614::MLX90614_REGISTER_TOMAX, Value );

//			inherited::SystemStart();
		}

	public:
		TArduinoInfraredThermometerMLX90614SetRangeMaxElement( InfraredThermometerMLX90614 &AOwner ) :
			FOwner( AOwner )
		{
		}
	};
//---------------------------------------------------------------------------
	class TArduinoInfraredThermometerMLX90614SetRangeMaxElement_Clocked : public OpenWire::Object
	{
	public:
		OpenWire::ConnectSinkPin	ClockInputPin;

	public:
		float Value = 100.0;

	protected:
		InfraredThermometerMLX90614	&FOwner;

	protected:
		void DoClockReceive( void *_Data )
		{
			FOwner.writeTemperatureEEPROM( InfraredThermometerMLX90614::MLX90614_REGISTER_TOMAX, Value );
		}

	public:
		TArduinoInfraredThermometerMLX90614SetRangeMaxElement_Clocked( InfraredThermometerMLX90614 &AOwner ) :
			FOwner( AOwner )
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&TArduinoInfraredThermometerMLX90614SetRangeMaxElement_Clocked::DoClockReceive );
		}

	};
//---------------------------------------------------------------------------
	class TArduinoInfraredThermometerMLX90614SetAddressElement : public OpenWire::Component
	{
	protected:
		InfraredThermometerMLX90614	&FOwner;

	public:
		uint8_t Value = 0x5A;

	protected:
		virtual void SystemStart() override
		{
			FOwner.setAddress( Value );

//			inherited::SystemStart();
		}

	public:
		TArduinoInfraredThermometerMLX90614SetAddressElement( InfraredThermometerMLX90614 &AOwner ) :
			FOwner( AOwner )
		{
		}
	};
//---------------------------------------------------------------------------
	class TArduinoInfraredThermometerMLX90614SetAddressElement_Clocked : public OpenWire::Object
	{
	public:
		OpenWire::ConnectSinkPin	ClockInputPin;

	public:
		uint8_t Value = 0x5A;

	protected:
		InfraredThermometerMLX90614	&FOwner;

	protected:
		void DoClockReceive( void *_Data )
		{
			FOwner.setAddress( Value );
		}

	public:
		TArduinoInfraredThermometerMLX90614SetAddressElement_Clocked( InfraredThermometerMLX90614 &AOwner ) :
			FOwner( AOwner )
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&TArduinoInfraredThermometerMLX90614SetAddressElement_Clocked::DoClockReceive );
		}

	};
//---------------------------------------------------------------------------
	enum TArduinoInfraredThermometerMLX90614IIRCoefficients { iir_a_0_5__b_0_5, iir_a_0_571__b_0_429, iir_a_0_667__b_0_333, iir_a_0_8__b_0_2, iir_Off };
//---------------------------------------------------------------------------
	class TArduinoInfraredThermometerMLX90614SetConfigBasic
	{
	protected:
		InfraredThermometerMLX90614	&FOwner;

	public:
		TArduinoInfraredThermometerMLX90614IIRCoefficients	IIRCoeficients : 3;
		bool	UsePositiveTemperatureCoefficientSensor : 1;
		bool	NegativeThermoshockCompensation : 1;
		uint8_t	FIRCoeficientsOrder	: 4;
		float Gain = 12.5;

	protected:
		void setConfig()
		{
			static const uint8_t CIIRFilterValues[] = { 0, 0b111, 0b110, 0b101, 0b100 };
			static const float CGainCiefficients[] = 
			{
				1.0,
				3.0,
				6.0,
				12.5,
				25.0,
				50.0,
				100.0
			};

			uint16_t ACoeffOrder = MITOV_ARRAY_SIZE( CGainCiefficients ) - 1;
			for( int i = MITOV_ARRAY_SIZE( CGainCiefficients ); i--; )
			{
				if( Gain > CGainCiefficients[ i ] )
					break;

				ACoeffOrder = i;

				if( Gain == CGainCiefficients[ i ] )
					break;

			}

			uint16_t AValue =	CIIRFilterValues[ IIRCoeficients ] |
								( UsePositiveTemperatureCoefficientSensor ? 0b1000 : 0 ) |
								( uint16_t( FIRCoeficientsOrder - 3 ) << 8 ) |
								( ACoeffOrder << 11 ) |
								( NegativeThermoshockCompensation ? 0x8000 : 0 );

			int16_t ARegisterValue;
			if( ! FOwner.I2CReadWord( InfraredThermometerMLX90614::MLX90614_REGISTER_CONFIG, ARegisterValue ))
				return;

			uint16_t AMask =	( uint16_t( 1 ) << 14 ) | 
								( uint16_t( 1 ) << 7 ) |
								( uint16_t( 1 ) << 6 ) |
								( uint16_t( 1 ) << 5 ) |
								( uint16_t( 1 ) << 4 );

//			Serial.println( ARegisterValue, BIN );
//			Serial.println( AMask, BIN );
//			Serial.println( AValue, BIN );

			ARegisterValue &= AMask;
			ARegisterValue |= AValue;

//			Serial.println( ARegisterValue, BIN );

			FOwner.writeEEPROM( InfraredThermometerMLX90614::MLX90614_REGISTER_CONFIG, ARegisterValue );
		}

	public:
		TArduinoInfraredThermometerMLX90614SetConfigBasic( InfraredThermometerMLX90614 &AOwner ) :
			FOwner( AOwner ),
			IIRCoeficients( iir_Off ),
			UsePositiveTemperatureCoefficientSensor( false ),
			NegativeThermoshockCompensation( true ),
			FIRCoeficientsOrder( 10 )
		{
		}
	};
//---------------------------------------------------------------------------
	class TArduinoInfraredThermometerMLX90614SetConfigElement : public TArduinoInfraredThermometerMLX90614SetConfigBasic, public OpenWire::Component
	{
		typedef TArduinoInfraredThermometerMLX90614SetConfigBasic inherited;

	protected:
		virtual void SystemStart() override
		{
			setConfig();

//			inherited::SystemStart();
		}

	public:
		using inherited::inherited;
	};
//---------------------------------------------------------------------------
	class TArduinoInfraredThermometerMLX90614SetConfigElement_Clocked : public TArduinoInfraredThermometerMLX90614SetConfigBasic, public OpenWire::Object
	{
		typedef TArduinoInfraredThermometerMLX90614SetConfigBasic inherited;

	public:
		OpenWire::ConnectSinkPin	ClockInputPin;

	protected:
		void DoClockReceive( void *_Data )
		{
			setConfig();
		}

	public:
		TArduinoInfraredThermometerMLX90614SetConfigElement_Clocked( InfraredThermometerMLX90614 &AOwner ) :
			inherited( AOwner )
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&TArduinoInfraredThermometerMLX90614SetConfigElement_Clocked::DoClockReceive );
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
