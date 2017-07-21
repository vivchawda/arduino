////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_BASIC_I2C_h
#define _MITOV_BASIC_I2C_h

#include <Mitov.h>
#include <Wire.h>

#ifndef BUFFER_LENGTH
	#define BUFFER_LENGTH 256
#endif

namespace Mitov
{
//	class I2C : public OpenWire::Component
	namespace I2C
	{
		const uint16_t	I2C_DEFAULT_READ_TIMEOUT	= 1000;
//	public:
		template<typename T> bool ReadBytes( T AWire, uint8_t devAddr, uint8_t regAddr, uint8_t length, void *data, uint16_t timeout = I2C_DEFAULT_READ_TIMEOUT )
		{
			int8_t count = 0;
			uint32_t t1 = millis();

            // Arduino v1.0.1+, Wire library
            // Adds official support for repeated start condition, yay!

            // I2C/TWI subsystem uses internal buffer that breaks with large data requests
            // so if user requests more than BUFFER_LENGTH bytes, we have to do it in
            // smaller chunks instead of all at once
            for (uint8_t k = 0; k < length; k += MitovMin<uint8_t>(length, uint8_t( BUFFER_LENGTH )))
			{
                AWire.beginTransmission(devAddr);
                AWire.write(regAddr);
                AWire.endTransmission();
                AWire.beginTransmission(devAddr);
                AWire.requestFrom(devAddr, (uint8_t)MitovMin(length - k, int( BUFFER_LENGTH )));

				regAddr += BUFFER_LENGTH;
        
                for (; AWire.available() && (timeout == 0 || millis() - t1 < timeout); count++)
                    ((uint8_t *)data )[count] = AWire.read();
        
                AWire.endTransmission();
            }

			return ( count == length );
		}

		inline bool ReadBytes( uint8_t devAddr, uint8_t regAddr, uint8_t length, void *data, uint16_t timeout = I2C_DEFAULT_READ_TIMEOUT )
		{
			return ReadBytes( Wire, devAddr, regAddr, length, data, timeout );
		}

		template<typename T> bool ReadBytes_16bitAddress( T AWire, uint8_t devAddr, bool ABigIndianAddr, uint16_t regAddr, uint8_t length, void *data, uint16_t timeout = I2C_DEFAULT_READ_TIMEOUT )
		{
			int8_t count = 0;
			uint32_t t1 = millis();

            // Arduino v1.0.1+, Wire library
            // Adds official support for repeated start condition, yay!

            // I2C/TWI subsystem uses internal buffer that breaks with large data requests
            // so if user requests more than BUFFER_LENGTH bytes, we have to do it in
            // smaller chunks instead of all at once
            for (uint8_t k = 0; k < length; k += MitovMin(length, uint8_t( BUFFER_LENGTH ))) 
			{
                AWire.beginTransmission(devAddr);
				if( ABigIndianAddr )
				{
					AWire.write(regAddr >> 8);
					AWire.write(regAddr & 0xFF);
				}
				else
				{
					AWire.write(regAddr & 0xFF);
					AWire.write(regAddr >> 8);
				}

                AWire.endTransmission();
                AWire.beginTransmission(devAddr);
                AWire.requestFrom(devAddr, (uint8_t)MitovMin(length - k, int( BUFFER_LENGTH )));

				regAddr += BUFFER_LENGTH;
        
                for (; AWire.available() && (timeout == 0 || millis() - t1 < timeout); count++)
                    ((uint8_t *)data )[count] = AWire.read();
        
                AWire.endTransmission();
            }

			return ( count == length );
		}

		inline bool ReadBytes_16bitAddress( uint8_t devAddr, bool ABigIndianAddr, uint16_t regAddr, uint8_t length, void *data, uint16_t timeout = I2C_DEFAULT_READ_TIMEOUT )
		{
			return ReadBytes_16bitAddress( Wire, devAddr, ABigIndianAddr, regAddr, length, data, timeout );
		}

		template<typename T> void WriteByte( T AWire, uint8_t devAddr, uint8_t regAddr, uint8_t AValue )
		{
//			Serial.print( "Address: " ); Serial.print( devAddr, HEX ); Serial.print( " Reg: " ); Serial.print( regAddr, HEX );  Serial.print( " = " ); Serial.println( AValue, BIN );
			AWire.beginTransmission( devAddr );
			AWire.write( regAddr );
			AWire.write( AValue );
			AWire.endTransmission();
		}

		inline void WriteByte( uint8_t devAddr, uint8_t regAddr, uint8_t AValue )
		{
			WriteByte( Wire, devAddr, regAddr, AValue );
		}

		template<typename T> void WriteByte_16bitAddress( T AWire, uint8_t devAddr, bool ABigIndianAddr, uint16_t regAddr, uint8_t AValue )
		{
//			Serial.print( "Address: " ); Serial.print( devAddr, HEX ); Serial.print( " Reg: " ); Serial.print( regAddr, HEX );  Serial.print( " = " ); Serial.println( AValue, BIN );
			AWire.beginTransmission( devAddr );
			if( ABigIndianAddr )
			{
				AWire.write(regAddr >> 8);
				AWire.write(regAddr & 0xFF);
			}

			else
			{
				AWire.write(regAddr & 0xFF);
				AWire.write(regAddr >> 8);
			}

			AWire.write( AValue );
			AWire.endTransmission();
		}

		inline void WriteByte_16bitAddress( uint8_t devAddr, bool ABigIndianAddr, uint16_t regAddr, uint8_t AValue )
		{
			WriteByte_16bitAddress( Wire, devAddr, ABigIndianAddr, regAddr, AValue );
		}

		template<typename T> void WriteBytes_16bitAddress( T AWire, uint8_t devAddr, bool ABigIndianAddr, uint16_t regAddr, uint8_t length, void *data )
		{
//			Serial.print( "Address: " ); Serial.print( devAddr, HEX ); Serial.print( " Reg: " ); Serial.print( regAddr, HEX );  Serial.print( " = " ); Serial.println( AValue, BIN );
			AWire.beginTransmission( devAddr );
			if( ABigIndianAddr )
			{
				AWire.write(regAddr >> 8);
				AWire.write(regAddr & 0xFF);
			}

			else
			{
				AWire.write(regAddr & 0xFF);
				AWire.write(regAddr >> 8);
			}

			AWire.write( (byte *)data, length );
			AWire.endTransmission();
		}

		inline void WriteBytes_16bitAddress( uint8_t devAddr, bool ABigIndianAddr, uint16_t regAddr, uint8_t length, void *data )
		{
			WriteBytes_16bitAddress( Wire, devAddr, ABigIndianAddr, regAddr, length, data );
		}
	};
//---------------------------------------------------------------------------
	class Basic_I2CChannel;
//---------------------------------------------------------------------------
	class Basic_MultiChannel_SourceI2C : public OpenWire::Component, public ClockingSupport
	{
		typedef OpenWire::Component inherited;

	public:
		bool	Enabled : 1;

	public:
		bool	FModified : 1;

	public:
		Mitov::SimpleList<Basic_I2CChannel *>	FChannels;

//	protected:
//		virtual void SystemInit();

//	protected:
//		virtual void DoClockReceive( void * );

	public:
		Basic_MultiChannel_SourceI2C() :
			Enabled( true ),
			FModified( false )
		{
		}
	};
//---------------------------------------------------------------------------
	class Basic_I2CChannel : public Mitov::CommonSink
	{
		typedef Mitov::CommonSink	inherited;

	public:
		float	FValue = 0.0f;
		float	FNewValue = 0.0f;

//	public:
//		virtual void InitChannel() {}
//		virtual void SendOutput() = 0;

	};
//---------------------------------------------------------------------------
	template<typename T_OWNER> class Basic_Typed_I2CChannel : public Mitov::Basic_I2CChannel
	{
		typedef Mitov::Basic_I2CChannel	inherited;

	public:
		float	InitialValue = 0.0f;

	protected:
		int		FIndex;

	protected:
		T_OWNER	&FOwner;

/*
	protected:
		virtual void DoReceive( void *_Data )
		{
			FNewValue = constrain( *((float *)_Data), 0, 1 );
			if( FNewValue == FValue )
				return;

			FOwner.FModified = true;

			if( FOwner.ClockInputPin.IsConnected() )
				FOwner.FModified = true;

			else
				SendOutput();

		}
*/

	public:
		Basic_Typed_I2CChannel( T_OWNER &AOwner, int AIndex ) :
			FOwner( AOwner ),
			FIndex( AIndex )
		{
			AOwner.FChannels.push_back( this );
		}

	};
//---------------------------------------------------------------------------
	class ArduinoI2CInput
	{
	public:
		const static bool Enabled = true;

	protected:
		TwoWire &FWire;

	public:
		void Print( const String AValue )
		{
#ifdef VISUINO_ESP32
			FWire.write( (uint8_t *)AValue.c_str(), AValue.length());
#else
			FWire.write( AValue.c_str(), AValue.length());
#endif
			FWire.write( '\r' );
			FWire.write( '\n' );
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
			FWire.write( AValue );
		}

		void PrintChar( byte AValue )
		{
			FWire.write( AValue );
		}

		void Write( uint8_t *AData, uint32_t ASize )
		{
			FWire.write( AData, ASize );
		}

	public:
		ArduinoI2CInput( TwoWire &AWire ) :
			FWire( AWire )
		{
		}

	};
//---------------------------------------------------------------------------
	template<TwoWire &IWIRE> class ArduinoI2COutput : public OpenWire::Component
	{
		typedef OpenWire::Component	inherited;

	public:
		static OpenWire::SourcePin	OutputPin;
		static OpenWire::SourcePin	RequestOutputPin;

	protected:
#if defined( VISUINO_ARDUINO_101 ) || defined ( VISUINO_ESP32 )
	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( IWIRE.available() )
			{
				int AData = IWIRE.read();
				if( AData >= 0 )
				{
					byte AByte = AData;
					OutputPin.SendValue( Mitov::TDataBlock( 1, &AByte ));
				}
			}
		}
#else // VISUINO_ARDUINO_101
		virtual void SystemInit() override
		{
			inherited::SystemInit();
			IWIRE.onReceive( receiveEvent );
			IWIRE.onRequest( requestEvent );
		}

	protected:
		static void receiveEvent(int howMany) 
		{
//			Serial.println( "Test1" );
			while( 1 < IWIRE.available())
			{ // loop through all but the last
//  				Serial.println( "Test2" );
				byte AByte = IWIRE.read();
				OutputPin.SendValue( Mitov::TDataBlock( 1, &AByte ));
			}

			byte AByte = IWIRE.read();
			OutputPin.SendValue( Mitov::TDataBlock( 1, &AByte ));
		}

		static void requestEvent()
		{
			RequestOutputPin.Notify( nullptr );
		}

#endif // VISUINO_ARDUINO_101

	};
//---------------------------------------------------------------------------
template<TwoWire &IWIRE> OpenWire::SourcePin	ArduinoI2COutput<IWIRE>::OutputPin;
template<TwoWire &IWIRE> OpenWire::SourcePin	ArduinoI2COutput<IWIRE>::RequestOutputPin;
//---------------------------------------------------------------------------
	class I2CDevice : public OpenWire::Object // : public Mitov::CommonSink
	{
//		typedef Mitov::CommonSink	inherited;

	public:
		uint8_t	Address = 0;

	public:
		const static bool Enabled = true;

	public:
		TwoWire &FWire;

	public:
	/*
		template<typename T> void Print( T AValue )
		{
		}
	*/
		void Print( const String AValue )
		{
			FWire.beginTransmission( Address );
#ifdef VISUINO_ESP32
			FWire.write( (uint8_t *)AValue.c_str(), AValue.length());
#else
			FWire.write( AValue.c_str(), AValue.length());
#endif
			FWire.write( '\r' );
			FWire.write( '\n' );
			FWire.endTransmission();
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
			FWire.beginTransmission( Address );
			FWire.write( AValue );
			FWire.endTransmission();
		}

		void PrintChar( byte AValue )
		{
			FWire.beginTransmission( Address );
			FWire.write( AValue );
			FWire.endTransmission();
		}

		void Write( uint8_t *AData, uint32_t ASize )
		{
			FWire.beginTransmission( Address );
			FWire.write( AData, ASize );
			FWire.endTransmission();
		}

//	protected:
//		virtual void DoReceive( void *_Data ) override
//		{
//			FWire.beginTransmission( Address );
//		}
	public:
		I2CDevice( TwoWire &AWire ) :
			FWire( AWire )
		{
		}

	};
//---------------------------------------------------------------------------
	class I2CDeviceRequest : public Mitov::ClockingSupport
	{
	protected:
		I2CDevice &FOwner;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		uint8_t	Size = 1;

	protected:
		virtual void DoClockReceive( void *_Data )
		{
			FOwner.FWire.requestFrom( FOwner.Address, Size );

			for( int i = 0; i < Size; ++i )
			{
				if( FOwner.FWire.available() )
				{
					byte AByte = FOwner.FWire.read();
//					Serial.println( AByte );
					OutputPin.SendValue( Mitov::TDataBlock( 1, &AByte ));
				}
			}
		}

	public:
		I2CDeviceRequest( I2CDevice &AOwner ) :
			FOwner( AOwner )
		{
		}

	};
//---------------------------------------------------------------------------
/*
	void Basic_MultiChannel_SourceI2C::DoClockReceive( void * )
	{
		if( ! FModified )
			return;

		for( int i =0; i < FChannels.size(); ++i )
			FChannels[ i ]->SendOutput();
	}
*/
//---------------------------------------------------------------------------
/*
	void Basic_MultiChannel_SourceI2C::SystemInit()
	{
		inherited::SystemInit();

		for( int i =0; i < FChannels.size(); ++i )
			FChannels[ i ]->InitChannel();
	}
*/
//---------------------------------------------------------------------------
}
#endif
