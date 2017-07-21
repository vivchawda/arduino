////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_RTC_DS1307_h
#define _MITOV_RTC_DS1307_h

#include <Mitov.h>
#include <Mitov_Basic_RTC.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	enum DS1307ClockFrequency { rtc1307fDisabledLow, rtc1307fDisabledHigh, rtc1307f1Hz, rtc1307f4096Hz, rtc1307f8192Hz, rtc1307f32768Hz };
//---------------------------------------------------------------------------
	class RTC_DS1307 : public BasicHaltRTC
	{
		typedef BasicHaltRTC inherited;

		enum Register 
		{
			kSecondReg  = 0,
			kMinuteReg  = 1,
			kHourReg    = 2,
			kDayReg     = 3,
			kDateReg    = 4,
			kMonthReg   = 5,
			kYearReg    = 6,
			kClockReg	= 7,

			// The RAM register space follows the clock register space.
			kRamAddress0     = 8
		};

		const uint8_t	DS1307_ADDRESS = 0x68;

	public:
		DS1307ClockFrequency	ClockFrequency : 3;

	public:
		void SetClockFrequency( DS1307ClockFrequency AValue )
		{
			if( ClockFrequency == AValue )
				return;

			ClockFrequency = AValue;
			UpdateClockFrequency();
		}

	protected:
		TwoWire	&FWire;

	protected:
		Mitov::TDateTime FLastDateTime;

	protected:
		uint8_t readRegister(const uint8_t reg) 
		{
			FWire.beginTransmission(DS1307_ADDRESS);
			FWire.write( reg );	
			FWire.endTransmission();

			FWire.requestFrom(DS1307_ADDRESS, (uint8_t)1 );

			return FWire.read();
		}

		void writeRegister( const uint8_t reg, const uint8_t value ) 
		{
			FWire.beginTransmission(DS1307_ADDRESS);
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

		void UpdateClockFrequency()
		{
			const uint8_t CValues [] = { 0x00, 0x80, 0x40, 0x41, 0x42, 0x43 };
			writeRegister( kClockReg, CValues[ ClockFrequency ] );
		}

		virtual void ReadTime()
		{
			if( ! OutputPin.IsConnected() )
				return;

			FWire.beginTransmission(DS1307_ADDRESS);
			FWire.write((byte)0);	
			FWire.endTransmission();

			FWire.requestFrom(DS1307_ADDRESS, (byte)7);
			uint16_t ASecond = Func::FromBcdToDec(FWire.read() & 0x7F);
			uint16_t AMinute = Func::FromBcdToDec(FWire.read());
			uint16_t AHour = hourFromRegisterValue( FWire.read() );
			uint16_t ADay = FWire.read();
			uint16_t ADate = Func::FromBcdToDec(FWire.read());
			uint16_t AMonth = Func::FromBcdToDec(FWire.read());
			uint16_t AYear = Func::FromBcdToDec(FWire.read()) + 2000;

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

		virtual void UpdateHalt() override
		{
			uint8_t sec = readRegister( kSecondReg );

			sec &= ~(1 << 7);
			sec |= ( ( Halt & 1 ) << 7 );
			writeRegister( kSecondReg, sec );
		}

	protected:
		virtual void DoSetReceive( void *_Data ) override
		{
//			if( WriteProtect )
//				return;

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

			FWire.beginTransmission(DS1307_ADDRESS);
			FWire.write((byte)0); // start at location 0

			FWire.write( ( ( Halt & 1 ) << 7 ) | ( Func::FromDecToBcd( ASecond ) & 0x7F ));
			FWire.write(Func::FromDecToBcd( AMinute ));
			FWire.write(Func::FromDecToBcd( AHour ));
			FWire.write(Func::FromDecToBcd( AWeekDay ));
			FWire.write(Func::FromDecToBcd( ADay ));
			FWire.write(Func::FromDecToBcd( AMonth ));
			FWire.write(Func::FromDecToBcd( AYear % 100 ));

			FWire.endTransmission();
		}

	protected:
		virtual void SystemInit() override
		{
			UpdateHalt();
			UpdateClockFrequency();
			inherited::SystemInit();
		}

	public:
		RTC_DS1307( TwoWire &AWire ) :
			ClockFrequency( rtc1307fDisabledLow ),
			FWire( AWire )
		{
		}
	};
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
