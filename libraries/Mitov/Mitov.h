////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_h
#define _MITOV_h

#include <OpenWire.h>

#define _VISUINO_MAX_PIN_NO_ 0xFFFF

#ifndef INPUT_PULLDOWN
	#ifndef VISUINO_ESP32
		#define INPUT_PULLDOWN INPUT
	#endif
#endif

#if defined( VISUINO_FEATHER_M0_BASIC ) || defined( VISUINO_FEATHER_M0_RFM9X ) || defined( VISUINO_FEATHER_M0_RFM69 )
	#define VISUINO_M0
	#define VISUINO_FEATHER_M0
#endif

#ifdef VISUINO_ESP32
	inline void analogWrite( uint8_t APin, uint16_t AValue ) {}
#endif

#if defined( VISUINO_FREESOC2 )
  #include <itoa.h>
#endif

#if defined( VISUINO_ARDUINO_DUE ) || defined( VISUINO_NRF52832 )
  #include <avr/dtostrf.h> 
#endif

#if defined( VISUINO_WEMOS_D1_R1 ) || defined( VISUINO_WEMOS_D1_R2 ) || defined( VISUINO_WEMOS_D1_MINI ) || defined( VISUINO_FEATHER_HUZZAH_ESP8266 )
  #define VISUINO_ESP8266
#endif

#if defined( VISUINO_TEENSY_3_0 ) || defined( VISUINO_TEENSY_3_1 ) || defined( VISUINO_TEENSY_LC )
  #define VISUINO_TEENSY
#endif

#if defined( VISUINO_ARDUINO_DUE ) || defined( VISUINO_LINKIT_ONE ) || defined( VISUINO_TEENSY ) || defined( VISUINO_TEENSY_2_0 ) || defined( VISUINO_TEENSY_2_0_PP )
  #define NO_SERIAL_FORMAT
#endif

#if defined( VISUINO_ESP8266 )
  #define V_FMOD(a,b) (a - b * floor(a / b))
  #define sscanf os_sprintf
#else
  #define V_FMOD(a,b) fmod(a,b)
#endif

#if ( defined( VISUINO_FREESOC2 ) || defined( VISUINO_M0 ))
  #if ! defined( __CORE_CM3_PSOC5_H__ )
//    #define PIN_MODE uint32_t
  #endif

namespace Mitov
{
	char *dtostrf (double val, signed char width, unsigned char prec, char *sout) 
	{
		char fmt[20];
		sprintf(fmt, "%%%d.%df", width, prec);
		sprintf(sout, fmt, val);
		return sout;
	}
}
#endif

float posmod( float a, float b )
{
	a = V_FMOD(a,b);
	if( a < 0 )
		a += b;

	return a;
}

#ifndef PWMRANGE
  #define PWMRANGE 255
#endif

#define MAKE_CALLBACK_3_1(T1,T2,A,P) (T2)( void (T1::*) ( P ) )&A
#define MAKE_CALLBACK_3(T1,T2,A,P) (T1 *)this, (T2)( void (T1::*) ( P ) )&A
#define MAKE_CALLBACK_2(T1,T2,A) (T1 *)this, (T2)( void (T1::*) ( void *_Data ) )&A
#define MAKE_CALLBACK(A) MAKE_CALLBACK_2(OpenWire::Component,OpenWire::TOnPinReceive,A)
//#define MAKE_CALLBACK(A) (OpenWire::Component *)this, (OpenWire::TOnPinReceive)( void (OpenWire::Component::*) ( void *_Data ) )&A

#define MITOV_ARRAY_SIZE(A) ( sizeof(A) / sizeof(A[0]) )
#define ARRAY_PARAM(A) A,MITOV_ARRAY_SIZE(A)

namespace Mitov
{
	template<typename T> T Constrain16( uint32_t AValue )
	{
		uint32_t AValue1 = MitovMin( AValue, uint32_t( 0xFFFF ));
		return AValue1;
	}

	template<> int16_t Constrain16<int16_t>( uint32_t AValue )
	{
		int32_t AValue1 = constrain( *(int32_t *)AValue, -0x8000l, 0x7FFFl );
		return AValue1;
	}
//---------------------------------------------------------------------------
enum TAngleUnits { auDegree, auRadians, auNormalized };

//---------------------------------------------------------------------------
	template <typename T> struct TArray
	{
	public:
		uint32_t	Size;
		T *Data;

	public:
		String ToString()
		{
//			Serial.println( "ToString" );
			String AResult = "(";
			for( int i = 0; i < MitovMin( Size, uint32_t( 32 ) ); ++i )
			{
				if( i < Size - 1 )
					AResult += String( Data[ i ] ) + ",";

				else
					AResult += String( Data[ i ] );

			}

			if( Size > 32 )
				AResult += "...";

			return	AResult + ")";
		}

	public:
		TArray( uint32_t ASize, const T *AData ) :
			Size( ASize), 
			Data( (T *)AData )
		{
		}

		TArray( const T &AData ) :
			Size( 1 ), 
			Data( (T *)&AData )
		{
		}
	};
//---------------------------------------------------------------------------
	struct TDataBlock : public TArray<byte>
	{
		typedef TArray inherited;

	public:
		TDataBlock( uint32_t ASize, const void *AData ) :
			inherited( ASize, (byte *)AData )
		{
		}

		TDataBlock() :
			inherited( 0, nullptr )
		{
		}

	};
//---------------------------------------------------------------------------
	template <typename T> struct TValueArray : public TArray<T>
	{
		typedef TArray<T> inherited;

	public:
		TValueArray<T>& operator=(const TArray<T>& other)
		{
			if( inherited::Data )
				delete [] inherited::Data;

			inherited::Size = other.Size;
			inherited::Data = new T[ inherited::Size ];

			for( int i = 0; i < inherited::Size; ++ i )
				inherited::Data[ i ] = other.Data[ i ];

			return *this;
		}

	public:
		TValueArray( T AData ) :
			inherited( 1, new T[ 1 ] )

		{
			*inherited::Data = AData;
		}

		TValueArray( uint32_t ASize, const T *AData ) :
			inherited( ASize, new T[ ASize ] )
		{
			for( int i = 0; i < ASize; ++ i )
				inherited::Data[ i ] = AData[ i ];
		}

		TValueArray( const TArray<T> &AOther ) :
			TValueArray( AOther.Size, AOther.Data )
		{
		}

		TValueArray() :
			inherited( 0, nullptr )
		{
		}

		~TValueArray()
		{
			if( inherited::Data )
				delete [] inherited::Data;
		}
	};
//---------------------------------------------------------------------------
	const bool GBooleanConst[] = { false, true };
//---------------------------------------------------------------------------
	#pragma pack(push, 1)
	struct TColor
	{
	public:
		uint8_t Blue;
		uint8_t Green;
		uint8_t Red;
		uint8_t Alpha = 0;

	public:
		void SetValue( const int32_t AValue )
		{
			*((int32_t *)this) = AValue & 0xFFFFFF;
//			Blue = ( AValue >> 16 ) & 0xFF;
//			Green = ( AValue >> 8 ) & 0xFF;
//			Red = AValue & 0xFF;
//			Alpha = 0;
		}

		void operator = ( const long AValue )
		{
			SetValue( AValue );
		}

		bool operator ==( const TColor other )
		{
			return *((long *)this) == *((long *)&other);
		}

		operator long()
		{
			return *((long *)this);
		}

	public:
		String ToString()
		{
			return	String( "(" ) + 
					String((int)Red ) + "," +
					String((int)Green ) + "," +
					String((int)Blue ) + ")";
		}

		uint16_t To565Color()
		{
			return (((31*(Red+4))/255)<<11) | 
               (((63*(Green+2))/255)<<5) | 
               ((31*(Blue+4))/255);
		}

	public:
		TColor( long AValue = 0 )
		{
			SetValue( AValue );
		}

		TColor( unsigned char ARed, unsigned char AGreen, unsigned char ABlue ) :
			Blue( ABlue ),
			Green( AGreen ),
			Red( ARed )
		{
		}

		TColor( long AValue, bool ) // For Windows BGR support
		{
			Red = ( AValue >> 16 ) & 0xFF;
			Green = ( AValue >> 8 ) & 0xFF;
			Blue = AValue & 0xFF;
		}
	};
	#pragma pack(pop)
//---------------------------------------------------------------------------
	#pragma pack(push, 1)
	struct TRGBWColor
	{
	public:
		uint8_t Blue;
		uint8_t Green;
		uint8_t Red;
		uint8_t White;

	public:
		void SetValue( const uint32_t AValue )
		{
//			*((uint32_t *)this) = AValue;
			White = ( AValue >> 24 ) & 0xFF;;
			Red = ( AValue >> 16 ) & 0xFF;
			Green = ( AValue >> 8 ) & 0xFF;
			Blue = AValue & 0xFF;
		}

		void operator =( const long AValue )
		{
			SetValue( AValue );
		}

		bool operator ==( const TColor other )
		{
			return *((uint32_t *)this) == *((uint32_t *)&other);
		}

		operator long()
		{
			return *((long *)this);
		}

	public:
		String ToString()
		{
			return	String( "(" ) + 
					String((int)Red ) + "," +
					String((int)Green ) + "," +
					String((int)Blue ) + "," +
					String((int)White ) + ")";
		}

		uint16_t To565Color()
		{
			return (((31*(Red+4))/255)<<11) | 
               (((63*(Green+2))/255)<<5) | 
               ((31*(Blue+4))/255);
		}

	public:
		TRGBWColor( uint32_t AValue = 0 )
		{
			SetValue( AValue );
		}

		TRGBWColor( unsigned char ARed, unsigned char AGreen, unsigned char ABlue, unsigned char AWhite ) :
			Blue( ABlue ),
			Green( AGreen ),
			Red( ARed ),
			White( AWhite )
		{
		}

		TRGBWColor( uint32_t AValue, bool ) // For Windows BGR support
		{
			Red = ( AValue >> 16 ) & 0xFF;
			Green = ( AValue >> 8 ) & 0xFF;
			Blue = AValue & 0xFF;
		}
	};
	#pragma pack(pop)
//---------------------------------------------------------------------------
	typedef TRGBWColor TAlphaColor;
//---------------------------------------------------------------------------
	const double HoursPerDay		= 24;
	const double MinsPerHour		= 60;
	const double SecsPerMin			= 60;
	const double MSecsPerSec		= 1000;
	const double MinsPerDay			= HoursPerDay * MinsPerHour;
	const double SecsPerDay			= MinsPerDay * SecsPerMin;
	const double SecsPerHour		= SecsPerMin * MinsPerHour;
	const double MSecsPerDay		= SecsPerDay * MSecsPerSec;
	const int32_t	IMSecsPerDay	= MSecsPerDay;
// Days between 1/1/0001 and 12/31/1899
//	const double DateDelta			= 693594;
//---------------------------------------------------------------------------
	const uint16_t MonthDays[2][12] =
	{
		{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
		{31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
	};
//---------------------------------------------------------------------------
	class TDateTimeCompareItems
	{
	public:
		bool Years : 1;
		bool Months : 1;
		bool Days : 1;
		bool WeekDays : 1;
		bool Hours : 1;
		bool Minutes : 1;
		bool Seconds : 1;
		bool MilliSeconds : 1;

	public:
		bool AllTrue() const
		{ 
			return Years && Months && Days && Hours && Minutes && Seconds && MilliSeconds;
		}

		bool AllDateTrue() const
		{ 
			return Years && Months && Days;
		}

		bool AllDateFalse() const
		{ 
			return ( ! Years ) && ( ! Months ) && ( ! Days );
		}

		bool AllTimeTrue() const
		{ 
			return Hours && Minutes && Seconds && MilliSeconds;
		}

		bool AllTimeFalse() const
		{ 
			return ( ! Hours ) && ( ! Minutes ) && ( ! Seconds ) && ( ! MilliSeconds );
		}

		bool CompareWeekDays() const
		{
			return AllDateFalse() && WeekDays; 
		}

	public:
		TDateTimeCompareItems() :
			Years( true ),
			Months( true ),
			Days( true ),
			WeekDays( true ),
			Hours( true ),
			Minutes( true ),
			Seconds( true ),
			MilliSeconds( true )
		{
		}

	};
//---------------------------------------------------------------------------
	class TDateTime
	{
	public:
		// Do not change the order! Date Must be after Time for pcket communication!
		int32_t Time; // Number of milliseconds since midnight
		int32_t Date; // One plus number of days since 1/1/0001

	protected:
		inline void DivMod( int32_t Dividend, uint16_t Divisor, uint16_t &Result, uint16_t &Remainder ) const
		{
			Result = Dividend / Divisor;
			Remainder = Dividend % Divisor;
		}

	public:
		void DecodeTime( uint16_t &Hour, uint16_t &AMin, uint16_t &Sec, uint16_t &MSec ) const
		{
			uint16_t	MinCount, MSecCount;

			DivMod( Time, SecsPerMin * MSecsPerSec, MinCount, MSecCount );
			DivMod( MinCount, MinsPerHour, Hour, AMin );
			DivMod( MSecCount, MSecsPerSec, Sec, MSec );
		}

		bool IsLeapYear( uint16_t Year ) const
		{
			return (Year % 4 == 0) && ((Year % 100 != 0) || (Year % 400 == 0));
		}

		bool DecodeDateFully( uint16_t &Year, uint16_t &Month, uint16_t &Day, uint16_t &DOW ) const
		{
			const uint32_t TD1 = 365;
			const uint32_t TD4 = TD1 * 4 + 1;
			const uint32_t D100 = TD4 * 25 - 1;
			const uint32_t D400 = D100 * 4 + 1;

			int32_t T = Date;
			if( T <= 0 )
			{
				Year = 0;
				Month = 0;
				Day = 0;
				DOW = 0;
				return( false );
			}

			else
			{
				DOW = T % 7 + 1;
				--T;
				uint16_t Y = 1;
				while( T >= (int32_t)D400 )
				{
					T -= D400;
					Y += 400;
				}

				uint16_t D, I;
				DivMod( T, D100, I, D );
				if( I == 4 )
				{
					-- I;
					D -= D100;
				}

				Y += I * 100;
				DivMod(D, TD4, I, D);

				Y += I * 4;
				DivMod(D, TD1, I, D);

				if( I == 4 )
				{
					--I;
					D += TD1;
				}

				Y += I;
				bool Result = IsLeapYear(Y);

				const uint16_t *DayTable = MonthDays[Result];
				uint16_t M = 0; // The C++ Day table is zero indexed!
				for(;;)
				{
					I = DayTable[M];
					if( D < I )
						break;

					D -= I;
					++ M;
				}

				Year = Y;
				Month = M + 1;
				Day = D + 1;

                return Result;
			}
		}

		bool TryEncodeDate( uint16_t Year, uint16_t Month, uint16_t Day )
		{
			const uint16_t *DayTable = MonthDays[IsLeapYear(Year)];
			if( (Year >= 1) && (Year <= 9999) && (Month >= 1) && (Month <= 12) && (Day >= 1))
				if( Day <= DayTable[Month - 1] )
				{

					for( int i = 1; i < Month; i ++ )
						Day += DayTable[ i - 1 ];

					int I = Year - 1;
					Date = ((uint32_t)I) * 365 + (int)( I / 4 ) - (int)( I / 100 ) + (int)( I / 400 ) + (uint32_t)Day;
					return true;
				}

			return false;
		}

		bool TryEncodeTime( uint16_t Hour, uint16_t AMin, uint16_t Sec, uint16_t MSec )
		{
			if ((Hour < HoursPerDay) && (AMin < MinsPerHour) && (Sec < SecsPerMin) && (MSec < MSecsPerSec))
			{
				Time =  (Hour * (MinsPerHour * SecsPerMin * MSecsPerSec))
					  + (AMin * SecsPerMin * MSecsPerSec)
					  + (Sec * MSecsPerSec)
					  +  MSec;
//				Date = DateDelta; // This is the "zero" day for a TTimeStamp, days between 1/1/0001 and 12/30/1899 including the latter date
				Date = 0; // This is the "zero" day for a TTimeStamp, days between 1/1/0001 and 12/30/1899 including the latter date
				return true;
			}

			return false;
		}

		bool TryEncodeDateTime( uint16_t AYear, uint16_t AMonth, uint16_t ADay, uint16_t AHour, uint16_t AMinute, uint16_t ASecond, uint16_t AMilliSecond )
		{
			bool Result = TryEncodeDate( AYear, AMonth, ADay );
			if( Result )
			{
				TDateTime LTime;
				Result = LTime.TryEncodeTime( AHour, AMinute, ASecond, AMilliSecond );
				if( Result )
					Time = LTime.Time; 

			}

            return Result;
		}

		void DecodeDateTime( uint16_t &AYear, uint16_t &AMonth, uint16_t &ADay, uint16_t &AHour, uint16_t &AMinute, uint16_t &ASecond, uint16_t &AMilliSecond ) const
		{
			uint16_t AWeekDay;
			DecodeDateTime( AYear, AMonth, ADay, AWeekDay, AHour, AMinute, ASecond, AMilliSecond );
		}

		void DecodeDateTime( uint16_t &AYear, uint16_t &AMonth, uint16_t &ADay, uint16_t &AWeekDay, uint16_t &AHour, uint16_t &AMinute, uint16_t &ASecond, uint16_t &AMilliSecond ) const
		{
			DecodeDateFully( AYear, AMonth, ADay, AWeekDay );
			DecodeTime( AHour, AMinute, ASecond, AMilliSecond );
		}

		String ToString() const
		{
			uint16_t AYear, AMonth, ADay, AHour, AMinute, ASecond, AMilliSecond;
			DecodeDateTime( AYear, AMonth, ADay, AHour, AMinute, ASecond, AMilliSecond );
			char ABuffer[ 6 + 4 + 5 * 2 + 3 + 2 + 1 ];
			sprintf( ABuffer, "(%04d.%02d.%02d %02d:%02d:%02d.%03d)", AYear, AMonth, ADay, AHour, AMinute, ASecond, AMilliSecond );
			return ABuffer;
		}

		uint32_t JulianDate()
		{
			uint16_t AYear;
			uint16_t AMonth;
			uint16_t ADay;
			uint16_t DOW;

			DecodeDateFully( AYear, AMonth, ADay, DOW );
//			Serial.println( AYear );
			if( AMonth <= 2 ) 
			{
				AYear--; 
				AMonth += 12;
			}

			int A = AYear / 100; 
			int B = 2 - A + A / 4;
			return uint32_t((365.25*( AYear+4716))+(int)(30.6001*(AMonth+1))+ ADay+B-1524 );
		}

		void AddMilliSeconds( int32_t AValue )
		{
			AllignTime( int64_t( AValue ) + Time );
//			int64_t
//			if( int64_t( AValue ) + >= MSecsPerDay )
//			int32_t AOldTime
		}

		void AddSeconds( int32_t AValue )
		{
			AllignTime( int64_t( AValue * MSecsPerSec ) + Time );
		}

		void AddMinutes( int32_t AValue )
		{
			AllignTime( int64_t( AValue * MSecsPerSec * SecsPerMin ) + Time );
		}

		void AddHours( int32_t AValue )
		{
			AllignTime( int64_t( AValue * MSecsPerSec * SecsPerMin * MinsPerHour ) + Time );
		}

		void AddDays( int32_t AValue )
		{
			Date += AValue;
		}

		void AddMonths( int32_t AValue )
		{
			uint16_t AYear;
			uint16_t AMonth;
			uint16_t ADay;
			uint16_t DOW;
			DecodeDateFully( AYear, AMonth, ADay, DOW );

			IncAMonth( AYear, AMonth, ADay, AValue );

			TryEncodeDate( AYear, AMonth, ADay );
//			Date += AValue;
		}

		void AddYears( int32_t AValue )
		{
			AddMonths( AValue * 12 );
		}

	protected:
		void AllignTime( int64_t AValue )
		{
			Date += AValue / IMSecsPerDay; 
			Time = AValue % IMSecsPerDay;
		}

		void IncAMonth( uint16_t &AYear, uint16_t &AMonth, uint16_t &ADay, int32_t ANumberOfMonths )
		{
			int Sign;

			if( ANumberOfMonths >= 0 )
				Sign = 1;

			else 
				Sign = -1;

			AYear += ANumberOfMonths / 12;
			ANumberOfMonths %= 12;
			int64_t ABigMonth = AMonth;
			ABigMonth += ANumberOfMonths;
			if( uint64_t( ABigMonth - 1 ) > 11 )    // if Month <= 0, word(Month-1) > 11)
			{
				AYear += Sign;
				ABigMonth += -12 * Sign;
			}

			AMonth = ABigMonth;

			const uint16_t *DayTable = MonthDays[IsLeapYear(AYear)];
			if( ADay > DayTable[ AMonth - 1 ] )
				ADay = DayTable[ AMonth - 1 ];

		}


	public:
		bool IsEqual( const TDateTime &AOther, const TDateTimeCompareItems &AItems ) const
		{
			if( AItems.AllTrue() )
				return ( *this == AOther );

			if( AItems.AllDateTrue() && AItems.AllTimeFalse() )
				return ( Date == AOther.Date );

			if( AItems.AllTimeTrue() && AItems.AllDateFalse() )
				return ( Time == AOther.Time );

			uint16_t AYear;
			uint16_t AMonth;
			uint16_t ADay;
			uint16_t AWeekDay;
			uint16_t AHour;
			uint16_t AMinute;
			uint16_t ASecond;
			uint16_t AMilliSecond;

			DecodeDateTime( AYear, AMonth, ADay, AWeekDay, AHour, AMinute, ASecond, AMilliSecond );

			uint16_t AOtherYear;
			uint16_t AOtherMonth;
			uint16_t AOtherDay;
			uint16_t AOtherWeekDay;
			uint16_t AOtherHour;
			uint16_t AOtherMinute;
			uint16_t AOtherSecond;
			uint16_t AOtherMilliSecond;

			AOther.DecodeDateTime( AOtherYear, AOtherMonth, AOtherDay, AOtherWeekDay, AOtherHour, AOtherMinute, AOtherSecond, AOtherMilliSecond );

			if( AItems.CompareWeekDays() )
				if( AWeekDay != AOtherWeekDay ) 
					return false;

			if( AItems.Years )
				if( AYear != AOtherYear ) 
					return false;

			if( AItems.Months )
				if( AMonth != AOtherMonth ) 
					return false;

			if( AItems.Days )
				if( ADay != AOtherDay ) 
					return false;

			if( AItems.Hours )
				if( AHour != AOtherHour ) 
					return false;

			if( AItems.Minutes )
				if( AMinute != AOtherMinute ) 
					return false;

			if( AItems.Seconds )
				if( ASecond != AOtherSecond ) 
					return false;

			if( AItems.MilliSeconds )
				if( AMilliSecond != AOtherMilliSecond ) 
					return false;

			return true;
		}

		bool IsBigger( const TDateTime &AOther, const TDateTimeCompareItems &AItems ) const
		{
			if( AItems.AllTrue() )
				return ( *this > AOther );

			if( AItems.AllDateTrue() && AItems.AllTimeFalse() )
				return ( Date > AOther.Date );

			if( AItems.AllTimeTrue() && AItems.AllDateFalse() )
				return ( Time > AOther.Time );

			uint16_t AYear;
			uint16_t AMonth;
			uint16_t ADay;
			uint16_t AWeekDay;
			uint16_t AHour;
			uint16_t AMinute;
			uint16_t ASecond;
			uint16_t AMilliSecond;

			DecodeDateTime( AYear, AMonth, ADay, AWeekDay, AHour, AMinute, ASecond, AMilliSecond );

			uint16_t AOtherYear;
			uint16_t AOtherMonth;
			uint16_t AOtherDay;
			uint16_t AOtherWeekDay;
			uint16_t AOtherHour;
			uint16_t AOtherMinute;
			uint16_t AOtherSecond;
			uint16_t AOtherMilliSecond;

			AOther.DecodeDateTime( AOtherYear, AOtherMonth, AOtherDay, AOtherWeekDay, AOtherHour, AOtherMinute, AOtherSecond, AOtherMilliSecond );

			if( AItems.CompareWeekDays() )
			{
				if( AWeekDay < AOtherWeekDay ) 
					return false;

				if( AWeekDay > AOtherWeekDay ) 
					return true;
			}

			if( AItems.Years )
			{
				if( AYear < AOtherYear ) 
					return false;

				if( AYear > AOtherYear ) 
					return true;
			}

			if( AItems.Months )
			{
				if( AMonth < AOtherMonth ) 
					return false;

				if( AMonth > AOtherMonth ) 
					return true;
			}

			if( AItems.Days )
			{
				if( ADay < AOtherDay ) 
					return false;

				if( ADay > AOtherDay ) 
					return true;
			}

			if( AItems.Hours )
			{
				if( AHour < AOtherHour ) 
					return false;

				if( AHour > AOtherHour ) 
					return true;
			}

			if( AItems.Minutes )
			{
				if( AMinute < AOtherMinute ) 
					return false;

				if( AMinute > AOtherMinute ) 
					return true;
			}

			if( AItems.Seconds )
			{
				if( ASecond < AOtherSecond ) 
					return false;

				if( ASecond > AOtherSecond ) 
					return true;
			}

			if( AItems.MilliSeconds )
			{
				if( AMilliSecond < AOtherMilliSecond ) 
					return false;

				if( AMilliSecond > AOtherMilliSecond ) 
					return true;
			}

			return true;
		}

		bool IsBiggerOrEqual( const TDateTime &AOther, const TDateTimeCompareItems &AItems ) const
		{
			if( AItems.AllTrue() )
				return ( *this >= AOther );

			return( IsEqual( AOther, AItems ) || IsBigger( AOther, AItems ));
		}

	public:
		bool operator == ( const TDateTime &AOther ) const
		{
			if( Date != AOther.Date )
				return false;

			return ( Time == AOther.Time );
		}

		bool operator != ( const TDateTime &AOther ) const
		{
			if( Date != AOther.Date )
				return true;

			return ( Time != AOther.Time );
		}

		bool operator <= ( const TDateTime &AOther ) const
		{
			if( Date > AOther.Date )
				return false;

			if( Date < AOther.Date )
				return true;

			return ( Time <= AOther.Time );
		}

		bool operator >= ( const TDateTime &AOther ) const
		{
			if( Date > AOther.Date )
				return true;

			if( Date < AOther.Date )
				return false;

			return ( Time >= AOther.Time );
		}

		bool operator < ( const TDateTime &AOther ) const
		{
			if( Date > AOther.Date )
				return false;

			if( Date < AOther.Date )
				return true;

			return ( Time < AOther.Time );
		}

		bool operator > ( const TDateTime &AOther ) const
		{
			if( Date > AOther.Date )
				return true;

			if( Date < AOther.Date )
				return false;

			return ( Time > AOther.Time );
		}

	public:
		TDateTime() :
			Time( 0 ),
			Date( 693594 )
		{
		}

		TDateTime( int32_t ADate, int32_t ATime ) :
			Time( ATime ),
			Date( ADate )
		{
		}

		TDateTime( uint32_t ADummy ) :
			Time( 0 ),
			Date( 693594 )
		{
		}

	};
//---------------------------------------------------------------------------
	class TComplex
	{
	public:
		float Real;
		float Imaginary;

	public:
		operator String()
		{
			return "[" + String( Real ) + "," + String( Imaginary ) + "]";
		}

	public:
		TComplex() :
			Real( 0 ),
			Imaginary( 0 )
		{
		}

		TComplex( float AReal ) :
			Real( AReal ),
			Imaginary( 0 )
		{
		}
	};
//---------------------------------------------------------------------------
	template <typename T> void swap ( T& a, T& b )
	{
		T c(a); a=b; b=c;
	}
//---------------------------------------------------------------------------
	class Bytes
	{
	public:
		uint8_t *_Bytes;
		unsigned int  _BytesSize = 0;

	};
//---------------------------------------------------------------------------
	class ConstBytes
	{
	public:
		uint8_t const * _Bytes;
		unsigned int  _BytesSize = 0;

	};
//---------------------------------------------------------------------------
	class BasicPinRead
	{
	public:
		virtual void SetMode( int AMode ) {}
		virtual bool DigitalRead() = 0;
	};
//---------------------------------------------------------------------------
	class EnabledComponent : public OpenWire::Component
	{
	public:
		bool	Enabled = true;

	};
//---------------------------------------------------------------------------
	class ClockingSupport : public OpenWire::Object
	{
	public:
		OpenWire::ConnectSinkPin	ClockInputPin;

	protected:
		virtual void DoClockReceive( void *_Data ) = 0;

	public:
		ClockingSupport()
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ClockingSupport::DoClockReceive );
		}

	};
//---------------------------------------------------------------------------
	class CommonSink : public OpenWire::Component
	{
	public:
		OpenWire::SinkPin	InputPin;

	protected:
		virtual void DoReceive( void *_Data ) = 0;

	public:
		CommonSink()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&CommonSink::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	class CommonFilter : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

	protected:
		virtual void DoReceive( void *_Data ) = 0;

	public:
		CommonFilter()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&CommonFilter::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	class CommonImplementedEnableFilter : public CommonFilter
	{
		typedef CommonFilter inherited;

    public:
        bool Enabled = true;

	protected:
		virtual void ReceiveValue( void *_Data ) = 0;

		virtual void DoReceive( void *_Data ) override
        {
            if( ! Enabled )
            {
			    OutputPin.Notify( _Data );
                return;
            }

			ReceiveValue( _Data );
        }

	};
//---------------------------------------------------------------------------
	template<typename T_IN, typename T_OUT> class CommonTypedInOutFilter : public CommonImplementedEnableFilter
    {
	protected:
        virtual T_OUT FilterValue( T_IN AValue ) = 0;

	protected:
		virtual void ReceiveValue( void *_Data ) override
        {
			T_IN AInValue = *(T_IN*)_Data;

            T_OUT AOutValue = FilterValue( AInValue );

			OutputPin.Notify( &AOutValue );
        }

    };
//---------------------------------------------------------------------------
	template<typename T> class CommonTypedFilter : public CommonTypedInOutFilter<T, T>
    {
    };
//---------------------------------------------------------------------------
	class CommonGenerator : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	protected:
		unsigned long FLastTime = 0;
		unsigned long FPeriod = 0;

	public:
		bool	Enabled = true;
		float	Frequency = 1.0;

	public:
		void SetFrequency( float AValue )
		{
			if( Frequency == AValue )
				return;

			Frequency = AValue;
			CalculateFields();
		}

	protected:
		virtual void Clock() = 0;

	protected:
		virtual void CalculateFields() override
		{
			if( Frequency == 0 )
				FPeriod = 1000000;

			else
				FPeriod = ( (( 1 / Frequency ) * 1000000 ) + 0.5 );

		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			while( currentMicros - FLastTime >= FPeriod )
			{
				if( Enabled && ( Frequency != 0 ))
					Clock();

				FLastTime += FPeriod;
			}

//			inherited::SystemLoopBegin( currentMicros );
		}

		virtual void SystemStart() override
		{
//			inherited::SystemStart();
			FLastTime = micros();
		}

	};
//---------------------------------------------------------------------------
	class RampToValueElementIntf
	{
	public:
		virtual void UpdateRamp( float ACurrentValue, unsigned long AOldTime, unsigned long ANewTime ) = 0;

	};
//---------------------------------------------------------------------------
	template<typename T> class ValueSource : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::ConnectSinkPin	ClockInputPin;
		OpenWire::SourcePin			OutputPin;

	public:
		T Value;

	public:
		virtual void ExternalSetValue( RampToValueElementIntf * ARampSender, T AValue )
		{
			if( Value == AValue )
				return;

			Value = AValue;
			OutputPin.Notify( &Value );
		}

	protected:
		virtual void SystemStart() override
		{
//			inherited::SystemStart();
			if( ! ClockInputPin.IsConnected() )
				OutputPin.Notify( &Value );

		}

		virtual void DoClockReceive( void *_Data )
		{
			OutputPin.Notify( &Value );
		}

	public:
		ValueSource( T AValue ) :
			Value( AValue )
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ValueSource::DoClockReceive );
		}

	};
//---------------------------------------------------------------------------
	template<typename T> class BindableValueSource : public Mitov::ValueSource<T>
	{
		typedef Mitov::ValueSource<T> inherited;

	protected:
		T OldValue;

	public:
		virtual void ExternalSetValue( RampToValueElementIntf * ARampSender, T AValue ) override
		{
			inherited::ExternalSetValue( ARampSender, AValue );
			OldValue = inherited::Value;
		}

	protected:
		virtual void SystemInit() override
		{
			inherited::SystemInit();
			OldValue = inherited::Value;
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( inherited::Value == OldValue )
				return;

			OldValue = inherited::Value;
			inherited::OutputPin.Notify( &OldValue );
		}

	public:
		BindableValueSource( T AValue ) :
			inherited( AValue ),
			OldValue( AValue )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T> class ValueRampSource : public Mitov::ValueSource<T>
	{
		typedef Mitov::ValueSource<T> inherited;

	protected:
		RampToValueElementIntf	*FActiveElement = nullptr;

	public:
		virtual void ExternalSetValue( RampToValueElementIntf * ARampSender, T AValue ) override
		{
			if( ARampSender != FActiveElement )
				FActiveElement = nullptr;

			inherited::ExternalSetValue( ARampSender, AValue );
		}

	protected:
		unsigned long	FLastTime = 0;

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( FActiveElement )
				FActiveElement->UpdateRamp( inherited::Value, FLastTime, currentMicros );
			

			FLastTime = currentMicros;
//			inherited::SystemLoopBegin( currentMicros );
		}

	public:
		void	RegisterForTimeUpdate( RampToValueElementIntf *AElement, bool ARegister )
		{
			if( ARegister )
				FActiveElement = AElement;

			else if( FActiveElement == AElement )
				FActiveElement = nullptr;

		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T> class BindableValueRampSource : public ValueRampSource<T>
	{
		typedef Mitov::ValueRampSource<T> inherited;

	protected:
		T OldValue;

	public:
		virtual void ExternalSetValue( RampToValueElementIntf * ARampSender, T AValue ) override
		{
			inherited::ExternalSetValue( ARampSender, AValue );
			OldValue = inherited::Value;
		}

	protected:
		virtual void SystemInit() override
		{
			inherited::SystemInit();
			OldValue = inherited::Value;
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( inherited::Value == OldValue )
				return;

			OldValue = inherited::Value;
			inherited::OutputPin.Notify( &OldValue );
		}

	public:
		BindableValueRampSource( T AValue ) :
			inherited( AValue ),
			OldValue( AValue )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T> class BasicValueStateElement : public OpenWire::Object
	{
	public:
		virtual bool TryActive() = 0;

	};
//---------------------------------------------------------------------------
	template<typename T> class ValueStateSource : public Mitov::ValueSource<T>
	{
		typedef Mitov::ValueSource<T> inherited;

	protected:
		T FCurrentValue;

	protected:
		Mitov::SimpleList<BasicValueStateElement<T> *>	FElements;

	public:
		virtual void ExternalSetValue( RampToValueElementIntf * ARampSender, T AValue ) override
		{
			if( FCurrentValue == AValue )
				return;

			FCurrentValue = AValue;
			inherited::OutputPin.Notify( &FCurrentValue );
		}

		void ResetValue()
		{
			for( int i = FElements.size(); i -- ; )
				if( FElements[ i ]->TryActive() )
					return;

			FCurrentValue = inherited::Value;
			inherited::OutputPin.Notify( &FCurrentValue );
		}

	public:
		void RegisterElement( BasicValueStateElement<T> *AValue )
		{
			FElements.push_back( AValue );
		}

	protected:
		virtual void SystemStart() override
		{
			FCurrentValue = inherited::Value;
			inherited::SystemStart();
//			if( ! ClockInputPin.IsConnected() )
//				OutputPin.Notify( &FCurrentValue );

		}

		virtual void DoClockReceive( void *_Data ) override
		{
			inherited::OutputPin.Notify( &FCurrentValue );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T> class BindableValueStateSource : public Mitov::ValueStateSource<T>
	{
		typedef Mitov::ValueStateSource<T> inherited;

	protected:
		T OldValue;

	protected:
		virtual void SystemInit() override
		{
			inherited::SystemInit();
			OldValue = inherited::Value;
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( inherited::Value == OldValue )
				return;

			OldValue = inherited::Value;
			inherited::OutputPin.Notify( &OldValue );
		}

	public:
		BindableValueStateSource( T AValue ) :
			inherited( AValue ),
			OldValue( AValue )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T> class ValueRampStateSource : public Mitov::ValueStateSource<T>
	{
		typedef Mitov::ValueStateSource<T> inherited;

	protected:
		RampToValueElementIntf	*FActiveElement = nullptr;

	public:
		virtual void ExternalSetValue( RampToValueElementIntf * ARampSender, T AValue ) override
		{
			if( ARampSender != FActiveElement )
				FActiveElement = nullptr;

			inherited::ExternalSetValue( ARampSender, AValue );
		}

	protected:
		unsigned long	FLastTime = 0;

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( FActiveElement )
				FActiveElement->UpdateRamp( inherited::FCurrentValue, FLastTime, currentMicros );
			

			FLastTime = currentMicros;
//			inherited::SystemLoopBegin( currentMicros );
		}

	public:
		void	RegisterForTimeUpdate( RampToValueElementIntf *AElement, bool ARegister )
		{
			if( ARegister )
				FActiveElement = AElement;

			else if( FActiveElement == AElement )
				FActiveElement = nullptr;

		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T> class BindableValueRampStateSource : public ValueRampStateSource<T>
	{
		typedef Mitov::ValueRampStateSource<T> inherited;

	protected:
		T OldValue;

	public:
		virtual void ExternalSetValue( RampToValueElementIntf * ARampSender, T AValue ) override
		{
			inherited::ExternalSetValue( ARampSender, AValue );
			OldValue = inherited::Value;
		}

	protected:
		virtual void SystemInit() override
		{
			inherited::SystemInit();
			OldValue = inherited::Value;
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( inherited::Value == OldValue )
				return;

			OldValue = inherited::Value;
			inherited::OutputPin.Notify( &OldValue );
		}

	public:
		BindableValueRampStateSource( T AValue ) :
			inherited( AValue ),
			OldValue( AValue )
		{
		}

	};
//---------------------------------------------------------------------------
	template<typename T, typename T_OUT> class BasicMultiInput : public OpenWire::Component
	{
		typedef OpenWire::Component	inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	protected:
		bool	FModified = false;
		T_OUT	FLastOutput;

	protected:
		void DoReceive( void *_Data )
		{
			FModified = true;
		}

	protected:
		virtual T_OUT CalculateOutput() = 0;

		inline void CallCalculateSendOutput( bool AFromStart ) // Work around compiler bug
		{
			CalculateSendOutput( AFromStart );
		}

		virtual void CalculateSendOutput( bool AFromStart )
		{
//			Serial.println( "CalculateSendOutput+++" );
			T_OUT AValue = CalculateOutput();
//			Serial.println( AValue );
			if( ! AFromStart )
				if( FLastOutput == AValue )
					return;

			OutputPin.SendValue( AValue ); // Use Send Value for String Compatibility!
			FLastOutput = AValue;

			FModified = false;
		}

		virtual void SystemStart() override
		{
			CalculateSendOutput( true );
		}

		virtual void SystemLoopEnd() override
		{
			if( FModified )
				CalculateSendOutput( false );

		}

	};
//---------------------------------------------------------------------------
	template<typename T, typename T_OUT, int C_NUM_INPUTS> class BasicCommonMultiInput : public Mitov::BasicMultiInput<T, T_OUT>
	{
		typedef Mitov::BasicMultiInput<T, T_OUT>	inherited;

	public:
		OpenWire::ValueSinkPin<T> InputPins[ C_NUM_INPUTS ];

	public:
		BasicCommonMultiInput()
		{
			for( int i = 0; i < C_NUM_INPUTS; ++i )
				InputPins[ i ].SetCallback( this, (OpenWire::TOnPinReceive)&BasicCommonMultiInput::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	template<typename T, int C_NUM_INPUTS> class CommonMultiInput : public Mitov::BasicCommonMultiInput<T,T, C_NUM_INPUTS>
	{
		typedef Mitov::BasicCommonMultiInput<T,T, C_NUM_INPUTS>	inherited;

	};
//---------------------------------------------------------------------------
	template<typename T, int C_NUM_INPUTS> class CommonClockedMultiInput : public Mitov::CommonMultiInput<T, C_NUM_INPUTS>, public ClockingSupport
	{
		typedef Mitov::CommonMultiInput<T, C_NUM_INPUTS>	inherited;

	protected:
		virtual void CalculateSendOutput( bool AFromStart )
		{
			if( ClockInputPin.IsConnected() )
				return;

			inherited::CalculateSendOutput( AFromStart );
/*
			T_OUT AValue = CalculateOutput();
//			Serial.println( AValue );
			if( ! AFromStart )
				if( FLastOutput == AValue )
					return;

			OutputPin.SendValue( AValue ); // Use Send Value for String Compatibility!
			FLastOutput = AValue;

			FModified = false;
*/
		}

	protected:
		virtual T CalculateOutput() = 0;

		virtual void DoClockReceive( void *_Data ) override
		{
			T AValue = CalculateOutput();
//			if( FLastOutput == AValue )
//				return;

			inherited::OutputPin.SendValue( AValue ); // Use Send Value for String Compatibility!
			inherited::FLastOutput = AValue;

			inherited::FModified = false;
		}

	};
//---------------------------------------------------------------------------
	typedef void (OpenWire::Object::*TNotifyEvent) ();
//---------------------------------------------------------------------------
	class TScheduleMilliDelay : public OpenWire::Component
	{
	protected:
		uint32_t			FDelay;
		unsigned long		FCurrentTime;
		OpenWire::Object	*FObject;
		TNotifyEvent		FCallback;
		bool				FSelDestroy;

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
//			inherited::SystemLoopBegin( currentMicros );
			if( millis() - FCurrentTime >= FDelay )
			{
				( FObject->*FCallback )();
				FSelDestroy = true;
			}
		}

		virtual void SelfDestroy() override
		{
//			inherited::SelfDestroy();
			if( FSelDestroy )
				delete this;
		}

	public:
		TScheduleMilliDelay( uint32_t ADelay, OpenWire::Object *AObject, TNotifyEvent ACallback ) :
			FDelay( ADelay ),
			FObject( AObject ),
			FCallback( ACallback ),
			FSelDestroy( false )
		{
			FCurrentTime = millis();
		}
	};
//---------------------------------------------------------------------------
	class DigitalInput : public OpenWire::Component, public BasicPinRead
	{
		typedef OpenWire::Component	inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		uint8_t PinNumber;

	protected:
		virtual void SystemInit() override
		{
//			inherited::SystemInit();
			pinMode(PinNumber, INPUT);
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			bool AValue = digitalRead( PinNumber );
			OutputPin.Notify( &AValue );
		}

	public:
		virtual void SetMode( int AMode ) override
		{
#ifdef VISUINO_FREESOC2
			pinMode( PinNumber, (PIN_MODE)AMode );
#else
			pinMode( PinNumber, AMode );
#endif
		}

		virtual bool DigitalRead() override
		{
			return digitalRead( PinNumber );
		}

	public:
		DigitalInput( int APinNumber ) :
			PinNumber( APinNumber )
		{
		}
	};
//---------------------------------------------------------------------------
	class DigitalPullUpInput : public OpenWire::Component, public BasicPinRead
	{
		typedef OpenWire::Component	inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		uint8_t PinNumber;

	protected:
		virtual void SystemInit() override
		{
//			inherited::SystemInit();
			pinMode(PinNumber, INPUT_PULLUP );
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			bool AValue = digitalRead( PinNumber );
			OutputPin.Notify( &AValue );
		}

	public:
		virtual void SetMode( int AMode ) override
		{
#ifdef VISUINO_FREESOC2
			pinMode( PinNumber, (PIN_MODE)AMode );
#else
			pinMode( PinNumber, AMode );
#endif
		}

		virtual bool DigitalRead() override
		{
			return digitalRead( PinNumber );
		}

	public:
		DigitalPullUpInput( int APinNumber ) :
			PinNumber( APinNumber )
		{
		}
	};
//---------------------------------------------------------------------------
	class AnalogAsDigitalInput : public OpenWire::Component, public BasicPinRead
	{
		typedef OpenWire::Component	inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		uint8_t PinNumber;

	public:
		virtual bool DigitalRead() override
		{
			int AValue = analogRead( PinNumber );
			return ( AValue > 409 ); // > 2.0 V High < 0.8 V Low
		}

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			bool AValue = DigitalRead();
			OutputPin.Notify( &AValue );
		}

	public:
		AnalogAsDigitalInput( int APinNumber ) :
			PinNumber( APinNumber )
		{
		}
	};
//---------------------------------------------------------------------------
	class DigitalOutput : public OpenWire::Component
	{
		typedef OpenWire::Component	inherited;

	public:
		OpenWire::SinkPin	InputPin;

	protected:
		void DoReceive( void *_Data )
		{
//			Serial.println( "Test1" );
			digitalWrite( PinNumber, *(bool*)_Data ? HIGH : LOW );
		}

	public:
		uint8_t PinNumber;

	protected:
		virtual void SystemInit() override
		{
//			inherited::SystemInit();
			pinMode(PinNumber, OUTPUT);
		}

	public:
		DigitalOutput( int APinNumber ) :
			PinNumber( APinNumber )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&DigitalOutput::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	class ArduinoBasicAnalogInputChannel : public OpenWire::Component
	{
		typedef OpenWire::Component	inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	protected:
		uint8_t	FPinNumber;
		float	FLastOutput = 0.0f;

	public:
		ArduinoBasicAnalogInputChannel( int APinNumber ) :
			FPinNumber( APinNumber )
		{
		}
	};
//---------------------------------------------------------------------------
	class ArduinoAnalogInputChannel : public ArduinoBasicAnalogInputChannel
	{
		typedef Mitov::ArduinoBasicAnalogInputChannel	inherited;

#ifdef VISUINO_TEENSY
	protected:
		unsigned int FResolution = 0;
		float		 FDivider = 1023.0;

	public:
		void SetResolution( unsigned int AResolution )
		{
			if( AResolution == FResolution )
				return;

			FResolution = AResolution;
			analogReadRes( FResolution );
			CalculateMultiplier();
		}

	protected:
		void CalculateMultiplier()
		{
			FDivider = pow( 2, FResolution ) - 1;
		}

#endif
	protected:
		virtual void SystemStart() override
		{
#ifdef VISUINO_TEENSY
			analogReadRes( FResolution );
			FLastOutput = analogRead( FPinNumber ) / FDivider;
#else
			FLastOutput = analogRead( FPinNumber ) / 1023.0;
#endif
	    	OutputPin.Notify( &FLastOutput );
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
#ifdef VISUINO_TEENSY
			float AValue = analogRead( FPinNumber ) / FDivider;
#else
			float AValue = analogRead( FPinNumber ) / 1023.0;
#endif
			if( AValue == FLastOutput )
				return;

			FLastOutput = AValue;
			OutputPin.Notify( &AValue );
		}

	public:
		ArduinoAnalogInputChannel( int APinNumber ) :
			inherited( APinNumber )
		{
		}

#ifdef VISUINO_TEENSY
		ArduinoAnalogInputChannel( int APinNumber, unsigned int AResolution ) :
			inherited( APinNumber ),
			FResolution( AResolution )
		{
			CalculateMultiplier();
		}

#endif
	};
//---------------------------------------------------------------------------
#if ( defined( VISUINO_TEENSY ) || defined( VISUINO_ESP32 ))
//---------------------------------------------------------------------------
	class ArduinoTouchAnalogInputChannel : public ArduinoBasicAnalogInputChannel
	{
		typedef Mitov::ArduinoBasicAnalogInputChannel	inherited;

	protected:
		virtual void SystemStart() override
		{
			FLastOutput = touchRead( FPinNumber );
	    	OutputPin.Notify( &FLastOutput );
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			float AValue = touchRead( FPinNumber );
			if( AValue == FLastOutput )
				return;

			FLastOutput = AValue;
			OutputPin.Notify( &AValue );
		}

	public:
		ArduinoTouchAnalogInputChannel( int APinNumber ) :
			inherited( APinNumber )
		{
		}
	};
//---------------------------------------------------------------------------
/*
	template <int V_PIN> void SetArduinoAnalogInputResolutionChannel( unsigned int AValue )
	{
		analogReadRes( AValue );
	}
*/
//---------------------------------------------------------------------------
#endif // VISUINO_TEENSY
//---------------------------------------------------------------------------
	class AnalogOutput : public OpenWire::Component
	{
		typedef OpenWire::Component	inherited;

	public:
		OpenWire::SinkPin	InputPin;

	protected:
		void DoReceive( void *_Data )
		{
			float AValue = *(float*)_Data;
			analogWrite( PinNumber, ( AValue * PWMRANGE ) + 0.5 );
		}

	public:
		uint8_t PinNumber;

	protected:
		virtual void SystemInit() override
		{
//			inherited::SystemInit();

			pinMode(PinNumber, OUTPUT);
		}

	public:
		AnalogOutput( int APinNumber ) :
			PinNumber( APinNumber )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&AnalogOutput::DoReceive );
		}
	};
//---------------------------------------------------------------------------
    class ArduinoBasicChannel : public OpenWire::Component
	{
		typedef OpenWire::Component	inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	protected:
		uint8_t  FPinNumber;

	public:
		ArduinoBasicChannel( int APinNumber ) :
			FPinNumber( APinNumber )
		{
		}
	};
//---------------------------------------------------------------------------
/*
    class ArduinoDigitalInputChannel : public OpenWire::Component, public OpenWire::Pin
	{
		typedef OpenWire::Component	inherited;

	protected:
		uint8_t  FPinNumber;

	protected:
		virtual void Receive( void *_Data ) override
		{
			bool AValue = *(bool *)_Data;
            digitalWrite( FPinNumber, AValue ? HIGH : LOW );
		}

	protected:
		virtual void SystemInit() override
		{
//			inherited::SystemInit();
			pinMode( FPinNumber, OUTPUT );
		}

	public:
		ArduinoDigitalInputChannel( int APinNumber ) :
			FPinNumber( APinNumber )
		{
		}
	};
*/
    class ArduinoDigitalInputChannel
	{
		typedef OpenWire::Component	inherited;

	public:
		class DigitalPin : public OpenWire::Pin
		{
		public:
			uint8_t  FPinNumber;

		protected:
			virtual void Receive( void *_Data ) override
			{
				bool AValue = *(bool *)_Data;
				digitalWrite( FPinNumber, AValue ? HIGH : LOW );
			}
		};

	public:
		DigitalPin	DigitalInputPin;

/*
	protected:
		virtual void SystemInit() override
		{
//			inherited::SystemInit();
			pinMode( DigitalInputPin.FPinNumber, OUTPUT );
		}
*/
	public:
		ArduinoDigitalInputChannel( int APinNumber )			
		{
			DigitalInputPin.FPinNumber = APinNumber;
			pinMode( DigitalInputPin.FPinNumber, OUTPUT );
		}
	};
//---------------------------------------------------------------------------
    class ArduinoDigitalOutputChannel : public ArduinoBasicChannel, public BasicPinRead
	{
		typedef Mitov::ArduinoBasicChannel	inherited;

	protected:
        bool FIsPullUp : 1;
		bool FIsPullDown : 1;
		bool FLastOutput : 1;

	protected:
		virtual void SystemInit() override
		{
			inherited::SystemInit();
			pinMode( FPinNumber, FIsPullUp ? INPUT_PULLUP : ( FIsPullDown ? INPUT_PULLDOWN : INPUT ) );
		}

		virtual void SystemStart() override
		{
			FLastOutput = ( digitalRead( FPinNumber ) == HIGH );
	    	OutputPin.SendValue( FLastOutput );
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
        {
    		bool AValue = ( digitalRead( FPinNumber ) == HIGH );
			if( AValue == FLastOutput )
				return;

			FLastOutput = AValue;
	    	OutputPin.Notify( &AValue );
        }

	public:
		virtual void SetMode( int AMode ) override
		{
#ifdef VISUINO_FREESOC2
			pinMode( FPinNumber, (PIN_MODE)AMode );
#else
			pinMode( FPinNumber, AMode );
#endif
		}

		virtual bool DigitalRead() override
		{
			return digitalRead( FPinNumber );
		}

	public:
		ArduinoDigitalOutputChannel( int APinNumber, bool AIsPullUp, bool AIsPullDown ) :
			inherited( APinNumber ),
            FIsPullUp( AIsPullUp ),
			FIsPullDown( AIsPullDown )
		{
		}

	};
//---------------------------------------------------------------------------
    class ArduinoDigitalChannel : public ArduinoBasicChannel
    {
		typedef Mitov::ArduinoBasicChannel	inherited;

	protected:
        bool FIsOutput : 1;
        bool FIsPullUp : 1;
        bool FIsPullDown : 1;
        bool FIsAnalog : 1;
		bool FLastOutput : 1;
		bool FRawInput : 1;
		bool FCombinedInOut : 1;

	public:
		OpenWire::SinkPin	DigitalInputPin;
		OpenWire::SinkPin	AnalogInputPin;

	protected:
#ifdef VISUINO_ESP32
		virtual
#endif // VISUINO_ESP32
        void PinDirectionsInit()
        {
			if( FRawInput )
				return;

            if( FIsOutput )
                pinMode( FPinNumber, OUTPUT );

            else
				pinMode( FPinNumber, FIsPullUp ? INPUT_PULLUP : ( FIsPullDown ? INPUT_PULLDOWN : INPUT ) );
        }

	protected:
		virtual void SystemInit() override
		{
			inherited::SystemInit();

            PinDirectionsInit();
		}

		virtual void SystemStart() override
		{
			if( FRawInput )
				return;

			FLastOutput = ( digitalRead( FPinNumber ) == HIGH );
	    	OutputPin.SendValue( FLastOutput );
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
        {
            if( !FIsOutput )
            {
    			bool AValue = ( digitalRead( FPinNumber ) == HIGH );
				if( AValue == FLastOutput )
					return;

				FLastOutput = AValue;
//	Serial.println( AData.Value );
	    		OutputPin.Notify( &AValue );
            }
        }

	protected:
#ifdef VISUINO_ESP32
		virtual
#endif // VISUINO_ESP32
		void DoAnalogReceive( void *_Data )
        {
            if( FIsOutput && FIsAnalog )
            {
                float AValue = *(float*)_Data;
                analogWrite( FPinNumber, ( AValue * PWMRANGE ) + 0.5 );
            }
        }

/*
		void DoDigitalOutputReceive( void *_Data )
		{
			*((int*)_Data) = FPinNumber;
		}
*/
		void DoDigitalReceive( void *_Data )
		{
			if( FRawInput )
			{
				*((int*)_Data) = FPinNumber;
				return;
			}

            if( ( FCombinedInOut || FIsOutput ) && (! FIsAnalog ))
            {
				bool AValue = *(bool *)_Data;

				if( AValue )
				  if( FCombinedInOut )
					  pinMode( FPinNumber, OUTPUT );

                digitalWrite( FPinNumber, AValue ? HIGH : LOW );

				if( ! AValue )
					if( FCombinedInOut )
						pinMode( FPinNumber, FIsPullUp ? INPUT_PULLUP : ( FIsPullDown ? INPUT_PULLDOWN : INPUT ) );

            }
		}

	public:
        void SetIsOutput( bool AValue )
        {
            if( FIsOutput == AValue )
                return;

            FIsOutput = AValue;
            PinDirectionsInit();
        }

        void SetIsPullUp( bool AValue )
        {
            if( FIsPullUp == AValue )
                return;

            FIsPullUp = AValue;
            PinDirectionsInit();
        }

        void SetIsAnalog( bool AValue )
        {
            if( FIsAnalog == AValue )
                return;

            FIsAnalog = AValue;
            PinDirectionsInit();
        }

	public:
		ArduinoDigitalChannel( int APinNumber, bool AIsOutput, bool AIsPullUp, bool AIsPullDown, bool AIsAnalog, bool ACombinedInOut, bool ARawInput ) :
			inherited( APinNumber ),
            FIsOutput( AIsOutput ),
            FIsPullUp( AIsPullUp ),
			FIsPullDown( AIsPullDown ),
            FIsAnalog( AIsAnalog ),
			FRawInput( ARawInput ),
			FCombinedInOut( ACombinedInOut )
		{
			AnalogInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ArduinoDigitalChannel::DoAnalogReceive );
		}
    };
//---------------------------------------------------------------------------
    class ArduinoAnalogOutputChannel : public OpenWire::Component
    {
		typedef OpenWire::Component	inherited;

	public:
		OpenWire::SinkPin	InputPin;

	protected:
		uint8_t  FPinNumber;

	protected:
		void DoReceive( void *_Data )
		{
            float AValue = *(float*)_Data;
            analogWrite( FPinNumber, ( AValue * PWMRANGE ) + 0.5 );
		}

	public:
		ArduinoAnalogOutputChannel( int APinNumber ) : 
			FPinNumber( APinNumber )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ArduinoAnalogOutputChannel::DoReceive );
		}

	};
//---------------------------------------------------------------------------
#ifdef VISUINO_NRF52832
	#define SERIAL_TYPE Uart

#else // VISUINO_NRF52832
	#ifdef VISUINO_FREESOC2
		#define SERIAL_TYPE UARTClass

	#else // VISUINO_FREESOC2
		#ifdef VISUINO_FEATHER_M0
			#define SERIAL_TYPE Serial_

		#else // VISUINO_FEATHER_M0
			#ifdef VISUINO_FEMTO_USB
				#define SERIAL_TYPE Serial_
				#define Serial SerialUSB

			#else // VISUINO_FEMTO_USB
				#ifdef HAVE_CDCSERIAL
					#define SERIAL_TYPE Serial_
				#else // HAVE_CDCSERIAL
					#ifdef VISUINO_ARDUINO_101
						#define SERIAL_TYPE CDCSerialClass
					#else // VISUINO_ARDUINO_101
						#define SERIAL_TYPE HardwareSerial
					#endif // VISUINO_ARDUINO_101
				#endif // HAVE_CDCSERIAL
			#endif // VISUINO_FEMTO_USB
		#endif // VISUINO_FEATHER_M0
	#endif // VISUINO_FREESOC2
#endif // VISUINO_NRF52832
//---------------------------------------------------------------------------
	enum TArduinoSerialParity { spNone, spEven, slOdd, slMark, slSpace };
//---------------------------------------------------------------------------
	class BasicSerialPort : public OpenWire::Component
	{
		typedef OpenWire::Component	inherited;

	public:
		OpenWire::SourcePin	SendingOutputPin;

	public:
		bool	Enabled : 1;

	protected:
		bool	FSending : 1;

	public:
		uint32_t	AfterSendingDelay = 10000;

	public:
		virtual Stream &GetStream() = 0;

	protected:
		uint32_t	FStartTime;

	public:
		template<typename T> void Print( T AValue )
		{
			if( Enabled )
			{
				FSending = true;
				SendingOutputPin.SendValue( FSending );
				FStartTime = micros();
				GetStream().println( AValue );

			}
		}

		void PrintChar( char AValue )
		{
			if( Enabled )
			{
				FSending = true;
				SendingOutputPin.SendValue( FSending );
				FStartTime = micros();
				GetStream().print( AValue );
			}
		}

		void Write( uint8_t *AData, uint32_t ASize )
		{
			GetStream().write( AData, ASize );
		}

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( FSending )
			{
				if( currentMicros - FStartTime >= AfterSendingDelay )
				{
					FSending = false;
					SendingOutputPin.SendValue( FSending );
				}
			}

			inherited::SystemLoopBegin( currentMicros );
		}

	public:
		BasicSerialPort() :
			Enabled( true ),
			FSending( false )
		{
		}
	};
//---------------------------------------------------------------------------
	template<typename T_SERIAL_TYPE, T_SERIAL_TYPE *T_SERIAL> class BasicTypedSerialPort : public Mitov::BasicSerialPort
	{
		typedef Mitov::BasicSerialPort	inherited;

	public:
		virtual Stream &GetStream() { return *T_SERIAL; }

	protected:
		virtual void SystemInit() override
		{
			if( Enabled )
				StartPort();

			inherited::SystemInit();
		}

	protected:
		virtual void StartPort() = 0;

        void RestartPort()
		{
			if( ! Enabled )
				return;

			T_SERIAL->end();
			StartPort();
		}

	public:
		void SetEnabled( bool AValue )
		{
            if( Enabled == AValue )
                return;

            Enabled = AValue;
			if( Enabled )
				StartPort();

			else
				T_SERIAL->end();
		}

	};
//---------------------------------------------------------------------------
	template<typename T_SERIAL_TYPE, T_SERIAL_TYPE *T_SERIAL> class SpeedSerialPort : public Mitov::BasicTypedSerialPort<T_SERIAL_TYPE, T_SERIAL>
	{
		typedef Mitov::BasicTypedSerialPort<T_SERIAL_TYPE, T_SERIAL>	inherited;

	public:
		unsigned long	Speed = 9600;

	public:
		void SetSpeed( unsigned int AValue )
		{
            if( Speed == AValue )
                return;

            Speed = AValue;
            inherited::RestartPort();
		}

	protected:
		virtual void StartPort()
		{
			T_SERIAL->begin( Speed );
		}

	};
//---------------------------------------------------------------------------
	template<typename T_INSTANCE, T_INSTANCE *T_SERIAL_INSTANCE, typename T> class ArduinoSerialInput : public OpenWire::Component
	{
		typedef OpenWire::Component	inherited;

	public:
		OpenWire::SinkPin	InputPin;

	protected:
		void DoReceive( void *_Data )
		{
			T_SERIAL_INSTANCE->Print( *(T*)_Data );
		}

	public:
		ArduinoSerialInput()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ArduinoSerialInput::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	template<typename T_INSTANCE, T_INSTANCE *T_SERIAL_INSTANCE> class ArduinoSerialInput<T_INSTANCE, T_SERIAL_INSTANCE, bool> : public OpenWire::Component
	{
		typedef OpenWire::Component	inherited;

	public:
		OpenWire::SinkPin	InputPin;

	protected:
		void DoReceive( void *_Data )
		{
			T_SERIAL_INSTANCE->Print( *(bool*)_Data ? "true" : "false" );
		}

	public:
		ArduinoSerialInput()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ArduinoSerialInput::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	template<typename T_INSTANCE, T_INSTANCE *T_SERIAL_INSTANCE> class ArduinoSerialInput<T_INSTANCE, T_SERIAL_INSTANCE, char> : public OpenWire::Component
	{
		typedef OpenWire::Component	inherited;

	public:
		OpenWire::SinkPin	InputPin;

	protected:
		void DoReceive( void *_Data )
		{
			if( *(char*)_Data == '\n' )
				return;

			if( *(char*)_Data == '\r' )
				T_SERIAL_INSTANCE->Print( "" );

			else
				T_SERIAL_INSTANCE->PrintChar( *(char*)_Data );
		}

	public:
		ArduinoSerialInput()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ArduinoSerialInput::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	template<typename T_INSTANCE, T_INSTANCE *T_SERIAL_INSTANCE> class ArduinoSerialBinaryInput : public OpenWire::Component
	{
		typedef OpenWire::Component	inherited;

	public:
		OpenWire::SinkPin	InputPin;

	protected:
		void DoReceive( void *_Data )
		{
			if( T_SERIAL_INSTANCE->Enabled )
			{
				Mitov::TDataBlock ADataBlock = *(Mitov::TDataBlock*)_Data;
				T_SERIAL_INSTANCE->Write((uint8_t *) ADataBlock.Data, ADataBlock.Size );
//				T_SERIAL_INSTANCE->GetStream().write( *(unsigned char*)_Data );
			}
		}

	public:
		ArduinoSerialBinaryInput()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ArduinoSerialBinaryInput::DoReceive );
		}
	};
//---------------------------------------------------------------------------	
	template<typename T_INSTANCE, T_INSTANCE *T_SERIAL_INSTANCE> class ArduinoEnabledSerialBinaryInput : public OpenWire::Component
	{
		typedef OpenWire::Component	inherited;

	public:
		OpenWire::SinkPin	InputPin;

	protected:
		void DoReceive( void *_Data )
		{
			Mitov::TDataBlock ADataBlock = *(Mitov::TDataBlock*)_Data;
			T_SERIAL_INSTANCE->Write((uint8_t *) ADataBlock.Data, ADataBlock.Size );
//				T_SERIAL_INSTANCE->GetStream().write( *(unsigned char*)_Data );
		}

	public:
		ArduinoEnabledSerialBinaryInput()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ArduinoEnabledSerialBinaryInput::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	template<typename T_INSTANCE, T_INSTANCE *T_SERIAL_INSTANCE> class ArduinoSerialStringInput : public Mitov::CommonSink
	{
		typedef Mitov::CommonSink	inherited;

	protected:
		virtual void DoReceive( void *_Data ) override
		{
			T_SERIAL_INSTANCE->Print( (char*)_Data );
		}
	};
//---------------------------------------------------------------------------
	template<typename T_INSTANCE, T_INSTANCE *T_SERIAL_INSTANCE> class ArduinoSerialClockInput : public Mitov::CommonSink
	{
		typedef Mitov::CommonSink	inherited;

	protected:
		virtual void DoReceive( void *_Data ) override
		{
			T_SERIAL_INSTANCE->Print( "(Clock)" );
		}
	};
//---------------------------------------------------------------------------
	template<typename T_INSTANCE, T_INSTANCE *T_SERIAL_INSTANCE, typename T_OBJECT> class ArduinoSerialObjectInput : public Mitov::CommonSink
	{
		typedef Mitov::CommonSink	inherited;

	protected:
		virtual void DoReceive( void *_Data ) override
		{
			T_SERIAL_INSTANCE->Print( ((T_OBJECT *)_Data)->ToString().c_str() );
		}
	};
//---------------------------------------------------------------------------
/*
	template<typename T_INSTANCE, T_INSTANCE *T_SERIAL_INSTANCE> class ArduinoDirectSerialInput : public Mitov::CommonSink
	{
		typedef Mitov::CommonSink	inherited;

	public:
		virtual Stream &GetStream()
		{
			return T_SERIAL_INSTANCE->GetStream();
		}

	protected:
		virtual void DoReceive( void *_Data ) override
		{
//			if( T_SERIAL_INSTANCE->Enabled )
//				T_SERIAL->write( *(unsigned char*)_Data );
		}
	};
*/
//---------------------------------------------------------------------------
	template<typename T_INSTANCE, T_INSTANCE *T_SERIAL_INSTANCE> class ArduinoSerialOutput : public OpenWire::Component
	{
		typedef OpenWire::Component	inherited;

	public:
		OpenWire::SourcePin	OutputPin;

//	protected:
//		byte FData[ 2 ];

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( ! T_SERIAL_INSTANCE->Enabled )
				return;

//			if( Serial.available() )
			{
				int AData = T_SERIAL_INSTANCE->GetStream().read();
				if( AData >= 0 )
				{
//					FData[ 0 ] = AData;
//					OutputPin.Notify( FData );

					unsigned char AByte = AData;
					OutputPin.SendValue( Mitov::TDataBlock( 1, &AByte ));
				}
			}

//			inherited::SystemLoopBegin( currentMicros );
		}

/*
	public:
		ArduinoSerialOutput()
		{
			FData[ 1 ] = 0;
		}
*/
	};
//---------------------------------------------------------------------------
	class GPRSAccessPoint
	{
	public:
		bool	Enabled = true;

		String	AccessPointName;
		String	UserName;
		String	Password;

		int32_t		NumberRetries = 5;
		uint32_t	RetryInterval = 30000;

	public:
		int32_t	FRetryCount = 0;
		unsigned long FLastTime = 0;

	public:
		void ResetRetry()
		{
			FRetryCount = 0;
		}

	public:
		bool CanRetry( unsigned long currentMicros, bool &ACanRetry )
		{
			ACanRetry = ( FRetryCount <= NumberRetries );
//			Serial.print( "ACanRetry" ); Serial.println( ACanRetry );
			if( FRetryCount == 0 )
				return true;

			if( ! ACanRetry )
				return false;

			if(( currentMicros - FLastTime ) < RetryInterval * 1000 )
				return false;

			return true;
		}

	};
//---------------------------------------------------------------------------
	class CommonConverter : public OpenWire::SinkPin
	{
	public:
		OpenWire::SourcePin	OutputPin;

	};
//---------------------------------------------------------------------------
	class Convert_AnalogToComplexArray : public Mitov::CommonConverter
	{
	protected:
		virtual void Receive( void *_Data ) override
		{
			TComplex	AComplex( *(float *)_Data );
			TArray<Mitov::TComplex> AComplexBuffer( 1, &AComplex );
			OutputPin.Notify( &AComplexBuffer );
		}

	};
//---------------------------------------------------------------------------
	class Convert_BinaryBlockToChar : public Mitov::CommonConverter
	{
	protected:
		virtual void Receive( void *_Data ) override
		{
//			Serial.println( "TEST" );
			Mitov::TDataBlock ABlock = *(Mitov::TDataBlock *)_Data;
			char *APtr = (char *)ABlock.Data;
			int ASize = ABlock.Size;
//			Serial.println( ASize );
			while( ASize-- )
			{
//  				Serial.println( *APtr );
				OutputPin.Notify( APtr++ );
//				if( OutputPin.IsConnected() )
//					Serial.println( "TEST2" );
			}
		}

	};
//---------------------------------------------------------------------------
	class Convert_BinaryBlockToText : public Mitov::CommonConverter
	{
	protected:
		virtual void Receive( void *_Data ) override
		{
//			Serial.println( "TEST" );
			Mitov::TDataBlock ABlock = *(Mitov::TDataBlock *)_Data;
			char *APtr = (char *)ABlock.Data;
			int ASize = ABlock.Size;
			String AText;
			AText.reserve( ASize );
//			Serial.println( ASize );
			while( ASize-- )
				AText += *APtr++;

			OutputPin.Notify( (char *)AText.c_str() );
		}

	};	
//---------------------------------------------------------------------------
	class Convert_BinaryToClock : public Mitov::CommonConverter
	{
	protected:
		bool	FLastValue = false;

	protected:
		virtual void Receive( void *_Data ) override
		{
			bool AValue = *(bool *)_Data;
			if( AValue == FLastValue )
				return;

			FLastValue = AValue;
			if( AValue )
				OutputPin.Notify( nullptr );

		}

	};
//---------------------------------------------------------------------------
	template<typename T_FROM, typename T_TO> class TypedConvert : public Mitov::CommonConverter
	{
	protected:
		virtual void Receive( void *_Data ) override
		{
			OutputPin.SendValue( T_TO( *(T_FROM *)_Data ));
		}

	};
//---------------------------------------------------------------------------
	class Convert_DigitalToText : public Mitov::CommonConverter
	{
	protected:
		virtual void Receive( void *_Data ) override
		{
			OutputPin.Notify( (void *)((*(bool*)_Data ) ? "true" : "false" ) );
		}

	};
//---------------------------------------------------------------------------
	class Convert_DateTimeToText : public Mitov::CommonConverter
	{
	protected:
		virtual void Receive( void *_Data ) override
		{
			TDateTime &ADate = *(TDateTime *)_Data;
			OutputPin.SendValue( ADate.ToString());
		}

	};
//---------------------------------------------------------------------------
	class Convert_ColorToText : public Mitov::CommonConverter
	{
	protected:
		virtual void Receive( void *_Data ) override
		{
			TColor &AColor= *(TColor *)_Data;
			OutputPin.SendValue( AColor.ToString());
		}

	};
//---------------------------------------------------------------------------
	class Convert_RGBWColorToText : public Mitov::CommonConverter
	{
	protected:
		virtual void Receive( void *_Data ) override
		{
			TRGBWColor &AColor= *(TRGBWColor *)_Data;
			OutputPin.SendValue( AColor.ToString());
		}

	};
//---------------------------------------------------------------------------
	typedef Convert_RGBWColorToText Convert_AlphaColorToText;
//---------------------------------------------------------------------------
	namespace Func
	{
// Pass 8-bit (each) R,G,B, get back 16-bit packed color
		inline uint16_t Color565(uint8_t r, uint8_t g, uint8_t b) 
		{
			return (uint16_t(r & 0xF8) << 8) | (uint16_t(g & 0xFC) << 3) | (b >> 3);
		}		
//---------------------------------------------------------------------------
		inline uint16_t Color565(uint32_t AColor ) 
		{
			uint8_t	r = AColor >> 16;
			uint8_t	g = AColor >> 8;
			uint8_t	b = AColor;
			return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
		}		
//---------------------------------------------------------------------------
		inline uint32_t ColorFrom565( uint16_t AColor ) 
		{
			return	0b000001110000001100000111 |
					(( uint32_t( AColor ) & 0xf800 ) << 8 ) |
					(( uint32_t( AColor ) & 0x07e0 ) << 5 ) |
					(( uint32_t( AColor ) & 0x001f ) << 3 );

		}		
//---------------------------------------------------------------------------
		void insert_sort(int array[], uint8_t size) 
		{
			uint8_t j;
			int save;
  
			for (int i = 1; i < size; i++) 
			{
				save = array[i];
				for (j = i; j >= 1 && save < array[j - 1]; j--)
					array[j] = array[j - 1];

				array[j] = save; 
			}
		}
//---------------------------------------------------------------------------
		inline uint8_t FromBcdToDec(const uint8_t bcd)
		{
			return (10 * ((bcd & 0xF0) >> 4) + (bcd & 0x0F));
		}
//---------------------------------------------------------------------------
		inline uint8_t FromDecToBcd(const uint8_t dec) 
		{
			const uint8_t tens = dec / 10;
			const uint8_t ones = dec % 10;
			return (tens << 4) | ones;
		}
//---------------------------------------------------------------------------
		inline float ConvertCtoF(float c) 
		{
			return c * 1.8 + 32;
		}
//---------------------------------------------------------------------------
		inline float ConvertFtoC(float f)
		{
			return (f - 32) * 0.55555;
		}
//---------------------------------------------------------------------------
		template<typename T>T MapRange( T x, T in_min, T in_max, T out_min, T out_max)
		{
			return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
		}
//---------------------------------------------------------------------------
		bool ExtractOptionallyQuotedCommaText( String &AText, String &AResult )
		{
			if( AText.length() == 0 )
				return false;

			bool	AStartsWithQuote;
//			int		AStartIndex;

			if( AText[ 0 ] == '"' )
			{
				AStartsWithQuote = true;
//				AStartIndex = 1;
			}

			else
			{
				AStartsWithQuote = false;
//				AStartIndex = 0;
			}

			if( ! AStartsWithQuote )
			{
				int APos = AText.indexOf( "," );
				if( APos < 0 )
				{
					AResult = AText;
					AText = "";
					return true;
				}

				AResult = AText.substring( 0, APos );
				AText.remove( 0, APos + 1 );
				return true;
			}

			AResult = "";
			int ASubStartsWithQuote = 0;
			uint32_t ALength = 0;
			while( ALength < AText.length() )
			{
				if( AText[ ASubStartsWithQuote ] == ',' )
					break;

				if( AText[ ASubStartsWithQuote ] != '"' )
					return false;

				++ASubStartsWithQuote;

	//			Serial.print( "ASubStartsWithQuote: " ); Serial.println( ASubStartsWithQuote );
				int APos = AText.indexOf( "\"", ASubStartsWithQuote );
				if( APos < 0 )
					return false;

	//			Serial.println( APos );
				AResult += AText.substring( ASubStartsWithQuote, APos );

				ASubStartsWithQuote = APos + 1;
				ALength = APos + 2;
			}

			AText.remove( 0, ALength );

			return true;
		}
//---------------------------------------------------------------------------
		bool ContainsTextAtPos( String ALine, int APos, String ASubText )
		{
			if( ALine.length() < APos + ASubText.length() )
				return false;

			for( int i = 0; i < int( ASubText.length()); ++i )
				if( ALine[ APos + i ] != ASubText[ i ] )
					return false;

			return true;
		}
//---------------------------------------------------------------------------
/*
		uint8_t Exponent(uint16_t value)
		{
			// Calculates the base 2 logarithm of a value
			uint8_t result = 0;
			while (((value >> result) & 1) != 1) 
				result++;
			return(result);
		}
*/
//---------------------------------------------------------------------------
	}
//---------------------------------------------------------------------------
	template <typename T> T ConstrainToList( T AValue, const T AList[], int ASize )
	{
		int AIndex = -1;
		for( int i = 0; i < ASize; ++i )
			if( AValue <= AList[ i ] )
			{
				AIndex = i;
				break;
			}

		if( AIndex == -1 )
			return AList[ ASize - 1 ];

		return AList[ AIndex ];
	}
//---------------------------------------------------------------------------
	class InterruptLock 
	{
	public:
		InterruptLock()
		{
#ifndef VISUINO_FREESOC2
			noInterrupts();
#endif
		}

		~InterruptLock() 
		{
#ifndef VISUINO_FREESOC2
			interrupts();
#endif
		}
	};
//---------------------------------------------------------------------------
	class StringEx : public String
	{
		typedef	String	inherited;

	public:
		using inherited::inherited;
		StringEx( const char *AStr, int ALength )
		{
			copy( AStr, ALength );
		}

	};
//---------------------------------------------------------------------------
} // Mitov

#endif

