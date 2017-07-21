////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_COUNTER_h
#define _MITOV_COUNTER_h

#include <Mitov.h>

namespace Mitov
{
#define Min Min
#define Max Max
	class CounterLimit
	{
	public:
		int32_t Value;
		bool RollOver = true;

	public:
		CounterLimit( int32_t AValue ) :
			Value( AValue )
		{
		}
	};
//---------------------------------------------------------------------------
	class BasicCounter : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	ResetInputPin;
		OpenWire::SourcePin	OutputPin;

    public:
        bool Enabled = true;

	public:
		int32_t	InitialValue = 0;

	public:
		CounterLimit	Min = -2147483648;
		CounterLimit	Max = 2147483647;

	public:
		inline void SetValue( int32_t AValue )
		{
			FCount = AValue;
			OutputPin.Notify( &FCount );
		}

	protected:
		int32_t	FCount = 0;

	protected:
		virtual void SystemInit() override
		{
//			Serial.println( "Counter::SystemInit" );
			FCount = InitialValue;

//			inherited::SystemInit();
			OutputPin.Notify( &FCount );
		}

		virtual void SystemStart() override
		{
//			inherited::SystemStart();
			if( FCount != InitialValue )
				OutputPin.Notify( &FCount );

		}

	protected:
		inline void	CountUp()
		{
			if( Max.RollOver || ( FCount < Max.Value ))
			{
				++FCount;

				if( FCount > Max.Value )
					FCount = Min.Value;

				OutputPin.Notify( &FCount );
			}
		}

		inline void	CountDown()
		{
			if( Min.RollOver || ( FCount > Min.Value ))
			{
				--FCount;
				if( FCount < Min.Value )
					FCount = Max.Value;

				OutputPin.Notify( &FCount );
			}
		}

	protected:
		void DoReceiveReset( void *_Data )
		{
			FCount = InitialValue;
			OutputPin.Notify( &FCount );
		}

	public:
		BasicCounter()			
		{
			ResetInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&BasicCounter::DoReceiveReset );
		}

	};
//---------------------------------------------------------------------------
	class ArduinoCounterSetValueElement : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	InputPin;

	protected:
		BasicCounter &FOwner;

	public:
		int32_t	Value = 0;

	protected:
		void DoReceived( void *_Data )
		{
			FOwner.SetValue( Value );
		}

	public:
		ArduinoCounterSetValueElement( BasicCounter &AOwner ) :
			FOwner( AOwner )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ArduinoCounterSetValueElement::DoReceived );
		}

	};
//---------------------------------------------------------------------------
	class Counter : public Mitov::BasicCounter
	{
		typedef Mitov::BasicCounter inherited;

	public:
		OpenWire::SinkPin	InputPin;

	public:
		bool	Reversed = false;

	public:
		void DoReceive( void *_Data )
		{
            if( ! Enabled )
				return;

			if( Reversed )
				CountDown();

			else
				CountUp();

		}

	public:
		Counter()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&Counter::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	class UpDownCounter : public Mitov::BasicCounter
	{
		typedef Mitov::BasicCounter inherited;

	public:
		OpenWire::SinkPin	UpInputPin;
		OpenWire::SinkPin	DownInputPin;

	protected:
		void DoReceiveUp( void *_Data )
		{
            if( Enabled )
				CountUp();
		}

		void DoReceiveDown( void *_Data )
		{
            if( Enabled )
				CountDown();
		}

	public:
		UpDownCounter()
		{
			UpInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&UpDownCounter::DoReceiveUp );
			DownInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&UpDownCounter::DoReceiveDown );
		}

	};
//---------------------------------------------------------------------------
#undef Min
#undef Max
}

#endif
