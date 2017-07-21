////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_RTC_DS3231_h
#define _MITOV_RTC_DS3231_h

#include <Mitov.h>
#include <Wire.h>
//#include <Mitov_Basic_RTC.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
//	enum DS3231ClockFrequency { rtc3231fDisabled, rtc3231f1Hz, rtc3231f1024Hz, rtc3231f4096Hz, rtc3231f8192Hz };
//---------------------------------------------------------------------------
	class RTC_DS3231 : public OpenWire::Component
#ifdef _MITOV_RTC_DS3231_CLOCK_PIN_
		, public Mitov::ClockingSupport
#endif	// _MITOV_RTC_DS3231_CLOCK_PIN_
	{
		typedef OpenWire::Component inherited;

		enum Register 
		{
			kSecondReg  = 0,
			kMinuteReg  = 1,
			kHourReg    = 2,
			kDayReg     = 3,
			kDateReg    = 4,
			kMonthReg   = 5,
			kYearReg    = 6,
			kControlReg = 0xE
//			kClockReg	= 7,

			// The RAM register space follows the clock register space.
//			kRamAddress0     = 8
		};

		const uint8_t	DS3231_ADDRESS  = 0x68;

		const uint8_t	RTC_STATUS = 0x0F;

//Status register bits
		const uint8_t	OSF		= 7;
		const uint8_t	BB32KHZ = 6;
		const uint8_t	CRATE1	= 5;
		const uint8_t	CRATE0	= 4;
		const uint8_t	EN32KHZ = 3;
		const uint8_t	BSY		= 2;
		const uint8_t	A2F		= 1;
		const uint8_t	A1F		= 0;

	public:
		OpenWire::SourcePin	OutputPin;
#ifdef _MITOV_RTC_DS3231_SET_PIN_
		OpenWire::SinkPin	SetInputPin;
#endif	// _MITOV_RTC_DS3231_SET_PIN_

#ifdef _MITOV_RTC_DS3231_TEMPERATURE_PIN_
		OpenWire::TypedStartSourcePin<float>	TemperatureOutputPin;
#endif	// _MITOV_RTC_DS3231_TEMPERATURE_PIN_

	public:
		bool	Halt = false;

	public:
		void SetHalt( bool AValue )
		{
			if( Halt == AValue )
				return;

			Halt = AValue;
			UpdateHalt();
		}

	protected:
		TwoWire	&FWire;

	protected:
		Mitov::TDateTime FLastDateTime;

	protected:
		uint8_t readRegister(const uint8_t reg) 
		{
			FWire.beginTransmission(DS3231_ADDRESS);
			FWire.write( reg );	
			FWire.endTransmission();

			FWire.requestFrom(DS3231_ADDRESS, (uint8_t)1 );

			return FWire.read();
		}

		void writeRegister( const uint8_t reg, const uint8_t value ) 
		{
			FWire.beginTransmission(DS3231_ADDRESS);
			FWire.write( reg );	
			FWire.write( value );	
			FWire.endTransmission();
		}

		uint8_t hourFromRegisterValue(const uint8_t value) 
		{
			uint8_t adj;
			if (value & 128)  // 12-hour mode
				adj = 12 * ((value & 32) >> 5);

			else           // 24-hour mode
				adj = 10 * ((value & (32 + 16)) >> 4);

			return (value & 15) + adj;
		}

		void ReadTime()
		{
//			Serial.println( "ReadTime" );
			if( ! OutputPin.IsConnected() )
				return;

			FWire.beginTransmission(DS3231_ADDRESS);
			FWire.write((byte)0);	
			FWire.endTransmission();

			FWire.requestFrom(DS3231_ADDRESS, (byte)7);
			uint16_t ASecond = Func::FromBcdToDec(FWire.read() & 0x7F);
			uint16_t AMinute = Func::FromBcdToDec(FWire.read());
			uint16_t AHour = hourFromRegisterValue( FWire.read() );
			uint16_t ADay = FWire.read();
			uint16_t ADate = Func::FromBcdToDec(FWire.read());
			uint16_t AMonth = Func::FromBcdToDec(FWire.read());
			uint16_t AYear = Func::FromBcdToDec(FWire.read()) + 2000;

#ifdef _MITOV_RTC_DS3231_TEMPERATURE_PIN_
			FWire.beginTransmission(DS3231_ADDRESS);
			FWire.write((byte)0x11);	
			FWire.endTransmission();
			FWire.requestFrom(DS3231_ADDRESS, (byte)2);

			int16_t ATemp = int16_t( FWire.read() ) << 8;
			ATemp |= FWire.read();

			float ATemperature = float( ATemp ) / 256;
			TemperatureOutputPin.SetValue( ATemperature );
//			Serial.println( ATemperature );
#endif

			Mitov::TDateTime ADateTime;

			if( ADateTime.TryEncodeDateTime( AYear, AMonth, ADate, AHour, AMinute, ASecond, 0 ))
			{
				if( FLastDateTime != ADateTime )
				{
					FLastDateTime = ADateTime;
					OutputPin.Notify( &ADateTime );
				}
			}

		}

		void UpdateHalt()
		{
			uint8_t sec = readRegister( kSecondReg );

			sec &= ~(1 << 7);
			sec |= ( ( Halt & 1 ) << 7 );
			writeRegister( kSecondReg, sec );
		}

/*
		void UpdateClockFrequency()
		{
			const uint8_t CValues [] = { 0x00, 0x80, 0x40, 0x41, 0x42, 0x43 };

			uint8_t AValue = readRegister( kControlReg );

			sec &= ~(1 << 7);
			sec |= ( ( Halt & 1 ) << 7 );
			writeRegister( kSecondReg, sec );

			writeRegister( kControlReg, CValues[ ClockFrequency ] );
		}
*/
	protected:
#ifdef _MITOV_RTC_DS3231_SET_PIN_
		void DoSetReceive( void *_Data )
		{
			Mitov::TDateTime &ADateTime = *(Mitov::TDateTime *)_Data;

			uint16_t AYear;
			uint16_t AMonth;
			uint16_t ADay;
			uint16_t AWeekDay;
			uint16_t AHour;
			uint16_t AMinute;
			uint16_t ASecond;
			uint16_t AMilliSecond;
			ADateTime.DecodeDateTime( AYear, AMonth, ADay, AWeekDay, AHour, AMinute, ASecond, AMilliSecond );

			FWire.beginTransmission(DS3231_ADDRESS);
			FWire.write((byte)0); // start at location 0

			FWire.write( ( ( Halt & 1 ) << 7 ) | Func::FromDecToBcd( ASecond ));
			FWire.write(Func::FromDecToBcd( AMinute ));
			FWire.write(Func::FromDecToBcd( AHour ));
			FWire.write(Func::FromDecToBcd( AWeekDay ));
			FWire.write(Func::FromDecToBcd( ADay ));
			FWire.write(Func::FromDecToBcd( AMonth ));
			FWire.write(Func::FromDecToBcd( AYear % 100 ));

			FWire.endTransmission();

			uint8_t s = readRegister(RTC_STATUS);        //read the status register
			writeRegister( RTC_STATUS, s & ~( 1 << OSF) );  //clear the Oscillator Stop Flag
		}
#endif // _MITOV_RTC_DS3231_SET_PIN_

#ifdef _MITOV_RTC_DS3231_CLOCK_PIN_
		virtual void DoClockReceive( void *_Data ) override
		{
			ReadTime();
		}
#endif // _MITOV_RTC_DS3231_CLOCK_PIN_

	protected:
		virtual void SystemInit() override
		{
			UpdateHalt();
//			UpdateClockFrequency();
//			inherited::SystemInit();
		}

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
#ifdef _MITOV_RTC_DS3231_CLOCK_PIN_
			if( ! ClockInputPin.IsConnected() )
#endif // _MITOV_RTC_DS3231_CLOCK_PIN_
				ReadTime();

//			inherited::SystemLoopBegin( currentMicros );
		}

	public:
		RTC_DS3231( TwoWire &AWire ) :
			FWire( AWire )
		{
#ifdef _MITOV_RTC_DS3231_SET_PIN_
			SetInputPin.SetCallback( MAKE_CALLBACK( RTC_DS3231::DoSetReceive ));
#endif // _MITOV_RTC_DS3231_SET_PIN_
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
