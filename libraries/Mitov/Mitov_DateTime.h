////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_DATE_DIME_h
#define _MITOV_DATE_DIME_h

#include <Mitov.h>

namespace Mitov
{
//---------------------------------------------------------------------------
    class EncodeDateTime : public Mitov::BasicCommonMultiInput<long, TDateTime, 7>
	{
		typedef Mitov::BasicCommonMultiInput<long, TDateTime, 7> inherited;

    public:
        TDateTime	InitialValue;
		bool	OnlyModified = false;

	protected:
		virtual TDateTime CalculateOutput() override
		{
			TDateTime ADate;
			ADate.TryEncodeDateTime( InputPins[ 0 ].Value, InputPins[ 1 ].Value, InputPins[ 2 ].Value, InputPins[ 3 ].Value, InputPins[ 4 ].Value, InputPins[ 5 ].Value, InputPins[ 6 ].Value );
			return ADate;
		}

	};
//---------------------------------------------------------------------------
    class EncodeDateTimeClocked : public Mitov::BasicCommonMultiInput<long, TDateTime, 7>, public ClockingSupport
	{
		typedef Mitov::BasicCommonMultiInput<long, TDateTime, 7> inherited;

    public:
        TDateTime	InitialValue;
		bool	OnlyModified = false;

	protected:
		virtual void CalculateSendOutput( bool AFromStart )
		{
			if( ClockInputPin.IsConnected() )
				return;

			inherited::CalculateSendOutput( AFromStart );
		}

	protected:
		virtual TDateTime CalculateOutput() override
		{
			TDateTime ADate;
			ADate.TryEncodeDateTime( InputPins[ 0 ].Value, InputPins[ 1 ].Value, InputPins[ 2 ].Value, InputPins[ 3 ].Value, InputPins[ 4 ].Value, InputPins[ 5 ].Value, InputPins[ 6 ].Value );
			return ADate;
		}

		virtual void DoClockReceive( void *_Data ) override
		{
			TDateTime AValue = CalculateOutput();
//			if( FLastOutput == AValue )
//				return;

			inherited::OutputPin.SendValue( AValue ); // Use Send Value for String Compatibility!
			inherited::FLastOutput = AValue;

			inherited::FModified = false;
		}

	};
//---------------------------------------------------------------------------
	class DecodeDateTime : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPins[ 8 ];

	protected:
		void DoReceive( void *_Data )
		{
			TDateTime &ADate = *(TDateTime *)_Data;
			uint16_t AItem16[ 8 ];
			ADate.DecodeDateTime( AItem16[ 0 ], AItem16[ 1 ], AItem16[ 2 ], AItem16[ 7 ], AItem16[ 3 ], AItem16[ 4 ], AItem16[ 5 ], AItem16[ 6 ] );
			for( int i = 0; i < 8; ++ i )
			{
				long int AValue = AItem16[ i ];
				OutputPins[ i ].Notify( &AValue );
			}
		}

	public:
		DecodeDateTime()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&DecodeDateTime::DoReceive );
		}
	};	
//---------------------------------------------------------------------------
	class CompileDateTime : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::ConnectSinkPin	ClockInputPin;
		OpenWire::SourcePin			OutputPin;

	protected:
		TDateTime	FValue;

	protected:
		virtual void SystemStart() override
		{
//			inherited::SystemStart();
			if( ! ClockInputPin.IsConnected() )
				OutputPin.Notify( &FValue );

		}

		void DoClockReceive( void *_Data )
		{
			OutputPin.Notify( &FValue );
		}

	public:
		CompileDateTime()
		{
			const char *monthName[12] = 
			{
			  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
			  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
			};

			char AMonth[12];
			int ADay, AYear;
			uint8_t AMonthIndex;
			sscanf( __DATE__, "%s %d %d", AMonth, &ADay, &AYear);

			int AHour, AMin, ASec;
			sscanf( __TIME__, "%d:%d:%d", &AHour, &AMin, &ASec);

			for (AMonthIndex = 0; AMonthIndex < 12; AMonthIndex++) 
				if (strcmp(AMonth, monthName[AMonthIndex]) == 0)
					break;

			FValue.TryEncodeDateTime( AYear, AMonthIndex + 1, ADay, AHour, AMin, ASec, 0 );

			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&CompileDateTime::DoClockReceive );
		}

	};
//---------------------------------------------------------------------------
	class AddDateTime : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

	public:
		bool	Enabled = true;

		int32_t	Years = 0;
		int32_t	Months = 0;
		int32_t	Days = 0;
		int32_t	Hours = 0;
		int32_t	Minutes = 0;
		int32_t	Seconds = 0;
		int32_t	MilliSeconds = 0;

	protected:
		void DoReceive( void *_Data )
		{
			if( ! Enabled )
			{
				OutputPin.Notify( _Data );
				return;
			}

			TDateTime ADate = *(TDateTime *)_Data;
			if( Years )
				ADate.AddYears( Years );

			if( Months )
				ADate.AddMonths( Months );

			if( Days )
				ADate.AddDays( Days );

			if( Hours )
				ADate.AddHours( Hours );

			if( Minutes )
				ADate.AddMinutes( Minutes );

			if( Seconds )
				ADate.AddSeconds( Seconds );

			if( MilliSeconds )
				ADate.AddMilliSeconds( MilliSeconds );

			OutputPin.Notify( &ADate );
		}

	public:
		AddDateTime()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&AddDateTime::DoReceive );
		}
	};
//---------------------------------------------------------------------------
}

#endif
