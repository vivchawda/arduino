////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_TIMING_h
#define _MITOV_TIMING_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	class ClockGenerator : public Mitov::CommonGenerator
	{
	protected:
		virtual void Clock() override
		{
			OutputPin.Notify( nullptr );
		}

	};
//---------------------------------------------------------------------------
	class Timer : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	StartInputPin;
		OpenWire::SinkPin	ResetInputPin;
		OpenWire::SourcePin	OutputPin;

	public:
		uint32_t	Interval = 1000000;
		bool		AutoRepeat : 1;
		bool		CanRestart : 1;
		bool		Enabled : 1;

	public:
		void	UpdateEnabled()
		{
			if( Enabled )
			{
				if( AutoRepeat )
					DoReceiveStart( nullptr );
			}

			else
			{
				if( FValue )
				{
					FValue = false;
					OutputPin.SendValue( FValue );
				}
			}
		}

	protected:
		bool		FValue : 1;

		uint32_t	FStartTime;

	protected:
		virtual void SystemStart() override
		{
//			inherited::SystemStart();
			OutputPin.SendValue( FValue );
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( Enabled )
			{
				if( ! AutoRepeat )
					if( ! FValue )
					{
						inherited::SystemLoopBegin( currentMicros );
						return;
					}

				if( FValue )
					if( ! Enabled )
					{
						FValue = false;
						OutputPin.SendValue( FValue );
//						inherited::SystemLoopBegin( currentMicros );
						return;
					}

				currentMicros = micros(); // Use micros(); to make sure the DoReceiveStart is not from the same clock event!
				if( currentMicros - FStartTime >= Interval )
				{
					FValue = !FValue;
					OutputPin.SendValue( FValue );
					if( AutoRepeat )
						FStartTime += Interval;
				}
			}

//			inherited::SystemLoopBegin( currentMicros );
		}

	protected:
		virtual void DoReceiveStart( void *_Data )
		{
			if( ! Enabled )
				return;

			if( !CanRestart )
				if( FValue )
					return;

			FStartTime = micros();
			if( FValue )
				return;

			FValue = true;
			OutputPin.SendValue( FValue );
		}

		void DoReceiveClear( void *_Data )
		{
			if( FValue )
			{
				FValue = false;
				OutputPin.SendValue( FValue );

				if( AutoRepeat )
					FStartTime = micros();
			}
		}

	public:
		Timer() :
			AutoRepeat( false ),
			CanRestart( true ),
			Enabled( true ),
			FValue( false )
		{
			StartInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&Timer::DoReceiveStart );
			ResetInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&Timer::DoReceiveClear );
		}
	};
//---------------------------------------------------------------------------
	class Repeat : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool	Enabled = true;

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( Enabled )
				OutputPin.Notify( nullptr );

//			inherited::SystemLoopBegin( currentMicros );
		}

	};
//---------------------------------------------------------------------------
	class Start : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	protected:
		virtual void SystemStart() override
		{
			OutputPin.Notify( nullptr );
//			inherited::SystemStart();
		}

	};
//---------------------------------------------------------------------------
	class MultiSource : public OpenWire::Component
	{
	public:
		Mitov::SimpleList<OpenWire::SourcePin> OutputPins;

	public:
		OpenWire::SinkPin	InputPin;

	protected:
		void DoReceive( void *_Data )
		{
			for( int i = 0; i < OutputPins.size(); ++ i )
				OutputPins[ i ].Notify( _Data );
		}

	public:
		MultiSource()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&MultiSource::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	class RepeatSource : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

	public:
		bool	Enabled = true;
		uint32_t	Count = 1;

	protected:
		void DoReceive( void *_Data )
		{
			OutputPin.Notify( _Data );
			if( ! Enabled )
				return;

			for( uint32_t i = 0; i < Count; ++i )
				OutputPin.Notify( _Data );
		}

	public:
		RepeatSource()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&RepeatSource::DoReceive );
		}
	};
//---------------------------------------------------------------------------
	class Delay : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	StartInputPin;
		OpenWire::SinkPin	ResetInputPin;
		OpenWire::SourcePin	OutputPin;

	public:
		unsigned long	Interval = 1000000;
		bool			CanRestart : 1;
		bool			Enabled : 1;


	protected:
		bool			FValue : 1;
		unsigned long	FStartTime;

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( FValue )
			{
				if( ! Enabled )
					FValue = false;

				else
				{
					unsigned long ACurrentMicros = micros(); // Use micros(); to make sure the DoReceiveStart is not from the same clock event!
					if( ACurrentMicros - FStartTime >= Interval )
					{
						FValue = false;
						OutputPin.Notify( nullptr );
					}
				}
			}

//			inherited::SystemLoopBegin( currentMicros );
		}

	protected:
		virtual void DoReceiveStart( void *_Data )
		{
			if( ! Enabled )
				return;

			if( !CanRestart )
				if( FValue )
					return;

			FStartTime = micros();
			if( FValue )
				return;

			FValue = true;
		}

		void DoReceiveClear( void *_Data )
		{
			FValue = false;
		}

	public:
		Delay() :
			CanRestart( true ),
			Enabled( true ),
			FValue( false )
		{
			StartInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&Delay::DoReceiveStart );
			ResetInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&Delay::DoReceiveClear );
		}
	};
//---------------------------------------------------------------------------
	class OnOffDelay : public Mitov::CommonFilter
	{
		typedef Mitov::CommonFilter inherited;

	public:
		unsigned long	OnInterval = 1000000;
		unsigned long	OffInterval = 1000000;
        bool			Enabled : 1;
		bool			InitialValue : 1;

	protected:
		bool			FInputValue : 1;
		unsigned long	FStartTime;

	protected:
		virtual void DoReceive( void *_Data ) override
		{
			bool AValue = *(bool *)_Data;
			if( FInputValue == AValue )
				return;

			FInputValue = AValue;
			FStartTime = micros();
		}

	protected:
		virtual void SystemStart() override
		{
			FInputValue = InitialValue;
			inherited::OutputPin.SendValue( FInputValue );
//			inherited::SystemStart();
		}

		virtual void SystemLoopBegin( unsigned long ACurrentMicros ) override
		{
			if( FInputValue == InitialValue )
				return;

			unsigned long AInterval = ( FInputValue ) ? OnInterval : OffInterval;
			if( micros() - FStartTime < AInterval ) // Use micros() to avoid problems when started from the same loop!
				return;

			InitialValue = FInputValue; 
			inherited::OutputPin.SendValue( FInputValue );
		}

	public:
		OnOffDelay() : 
			Enabled( true ),
			InitialValue( false )
//			FInputValue( false )

		{
		}
	};
//---------------------------------------------------------------------------
	class DetectEdge : public Mitov::CommonFilter
	{
		typedef Mitov::CommonFilter inherited;

    public:
        bool	Enabled : 1;
		bool	Rising : 1;

	protected:
		bool	FLastValue : 1;

	protected:
		virtual void DoReceive( void *_Data ) override
		{
			bool AValue = *(bool *)_Data;
			if( AValue == FLastValue )
				return;

			FLastValue = AValue;
			if( Rising == AValue )
				OutputPin.Notify( NULL );

		}

	public:
		DetectEdge() :
			Enabled( true ),
			Rising( true ),
			FLastValue( false )
		{
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
