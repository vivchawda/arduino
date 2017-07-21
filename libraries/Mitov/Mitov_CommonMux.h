////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_COMMON_MUX_h
#define _MITOV_COMMON_MUX_h

#include <Mitov.h>

namespace Mitov
{
	template<typename T, typename T_OUT> class ToggleSwitch : public Mitov::BasicMultiInput<T, T_OUT>
	{
		typedef Mitov::BasicMultiInput<T, T_OUT> inherited;

	public:
		OpenWire::ValueSinkPin<T>	TrueInputPin;
		OpenWire::ValueSinkPin<T>	FalseInputPin;

		OpenWire::SinkPin	SelectInputPin;

	public:
		bool	InitialSelectValue = false;

	protected:
		void DoReceiveSelect( void *_Data )
		{
			bool AValue = *(bool *)_Data;
			if( InitialSelectValue == AValue )
				return;

			InitialSelectValue = AValue;
			inherited::CallCalculateSendOutput( false );
		}

	protected:
		virtual T_OUT CalculateOutput() override
		{
			if( InitialSelectValue )
				return (T_OUT)TrueInputPin.Value;

			else
				return (T_OUT)FalseInputPin.Value;
		}

	public:
		ToggleSwitch()
		{
			SelectInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ToggleSwitch::DoReceiveSelect );
			TrueInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ToggleSwitch::DoReceive );
			FalseInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ToggleSwitch::DoReceive );
		}

	};
//---------------------------------------------------------------------------
	class ClockToggleSwitch : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	TrueInputPin;
		OpenWire::SinkPin	FalseInputPin;

		OpenWire::SinkPin	SelectInputPin;

		OpenWire::SourcePin	OutputPin;

	public:
		bool	InitialSelectValue = false;

	protected:
		void DoReceiveSelect( void *_Data )
		{
			InitialSelectValue =  *(bool *)_Data;
		}

		void DoReceiveTrue( void *_Data )
		{
			if( InitialSelectValue )
				OutputPin.Notify( nullptr );
		}

		void DoReceiveFalse( void *_Data )
		{
			if( ! InitialSelectValue )
				OutputPin.Notify( nullptr );
		}

	public:
		ClockToggleSwitch()
		{
			SelectInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ClockToggleSwitch::DoReceiveSelect );
			TrueInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ClockToggleSwitch::DoReceiveTrue );
			FalseInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ClockToggleSwitch::DoReceiveFalse );
		}

	};
//---------------------------------------------------------------------------
	template<typename T, int C_NUM_INPUTS> class CommonMux : public Mitov::CommonMultiInput<T, C_NUM_INPUTS>
	{
		typedef Mitov::CommonMultiInput<T, C_NUM_INPUTS> inherited;

	public:
		uint8_t	InitialChannel = 0;

	public:
		OpenWire::SinkPin	SelectInputPin;

	protected:
		virtual T CalculateOutput() override
		{
			return inherited::InputPins[ InitialChannel ].Value;
		}

	protected:
		void DoReceiveSelect( void *_Data )
		{
			uint32_t AChannel = *(uint32_t *)_Data;
			if( AChannel >= C_NUM_INPUTS )
				AChannel = C_NUM_INPUTS - 1;

			if( InitialChannel == AChannel )
				return;

//			Serial.println( AChannel );

			InitialChannel = AChannel;
			inherited::CallCalculateSendOutput( false );
		}

	public:
		CommonMux()
		{
			SelectInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&CommonMux::DoReceiveSelect );
		}

	};
//---------------------------------------------------------------------------
	template<int C_NUM_INPUTS> class ClockMux : public OpenWire::Component
	{
	public:
		OpenWire::IndexedSinkPin	InputPins[ C_NUM_INPUTS ];
		OpenWire::SourcePin	OutputPin;

		OpenWire::SinkPin	SelectInputPin;

	public:
		uint8_t	InitialChannel = 0;

	protected:
		void DoReceive( int AIndex, void *_Data )
		{
			if( AIndex == InitialChannel )
				OutputPin.Notify( nullptr );
		}

		void DoReceiveSelect( void *_Data )
		{
			uint32_t AChannel = *(uint32_t *)_Data;
			if( AChannel >= C_NUM_INPUTS )
				AChannel = C_NUM_INPUTS - 1;

			InitialChannel = AChannel;
		}

	public:
		ClockMux()
		{
			for( int i = 0; i < C_NUM_INPUTS; ++i )
				InputPins[ i ].SetCallback( i, this, (OpenWire::TOnPinIndexedReceive)&ClockMux::DoReceive );

			SelectInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ClockMux::DoReceiveSelect );
		}

	};
//---------------------------------------------------------------------------
}

#endif
