////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_SWITCH_h
#define _MITOV_SWITCH_h

#include <Mitov.h>

namespace Mitov
{
	template<typename T> class CommonSwitch : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::ValueSinkPin<T>	InputPin;
		OpenWire::TypedStartSourcePin<T>	OutputPin;

		OpenWire::SinkPin	EnableInputPin;

	public:
		bool	InitialEnabled : 1;
		bool	IgnoreSame : 1;

	protected:
		void DoReceiveEnable( void *_Data )
		{
			bool AValue = *(bool *)_Data;
//			if( InitialEnabled == AValue )
//				return;

			InitialEnabled = AValue;
//			DoReceive( nullptr );
		}

		void DoReceive( void *_Data )
		{
			if( InitialEnabled )
				OutputPin.SetValue( InputPin.Value, ! IgnoreSame );
		}

	public:
		CommonSwitch() :
			InitialEnabled( false ),
			IgnoreSame( true )
		{
			EnableInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&CommonSwitch::DoReceiveEnable );
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&CommonSwitch::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	class TextSwitch : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::ValueSinkPin<String>	InputPin;
		OpenWire::SourcePin				OutputPin;

		OpenWire::SinkPin	EnableInputPin;

	public:
		bool	InitialEnabled = false;

	protected:
		void DoReceiveEnable( void *_Data )
		{
			bool AValue = *(bool *)_Data;
			if( InitialEnabled == AValue )
				return;

			InitialEnabled = AValue;
			DoReceive( nullptr );
		}

		void DoReceive( void *_Data )
		{
			if( InitialEnabled )
				OutputPin.SendValue( InputPin.Value );
		}

	public:
		TextSwitch()
		{
			EnableInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&TextSwitch::DoReceiveEnable );
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&TextSwitch::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	class ClockSwitch : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

		OpenWire::SinkPin	EnableInputPin;

	public:
		bool	InitialEnabled = false;

	protected:
		void DoReceiveEnable( void *_Data )
		{
			InitialEnabled = *(bool *)_Data;
		}

		void DoReceive( void *_Data )
		{
			if( InitialEnabled )
				OutputPin.Notify( nullptr );
		}

	public:
		ClockSwitch()
		{
			EnableInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ClockSwitch::DoReceiveEnable );
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ClockSwitch::DoReceive );
		}

	};
//---------------------------------------------------------------------------
}

#endif
