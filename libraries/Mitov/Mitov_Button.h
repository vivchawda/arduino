////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_BUTTON_h
#define _MITOV_BUTTON_h

#include <Mitov.h>

namespace Mitov
{
	class Button : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

	protected:
		unsigned long	FLastTime = 0;
		bool	FLastValue : 1;
		bool	FValue : 1;

	public:
		uint32_t	DebounceInterval = 50;

	protected:
		void DoReceive( void *_Data )
		{
			bool AValue = *( bool *)_Data;
			if( AValue != FLastValue )
				FLastTime = millis();

			FLastValue = AValue;
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( FValue != FLastValue )
				if( millis() - FLastTime > DebounceInterval )
				{
					FValue = FLastValue;
					OutputPin.SendValue( FValue );
				}

//			inherited::SystemLoopBegin( currentMicros );
		}

		virtual void SystemStart() override
		{
			FValue = FLastValue;
			OutputPin.SendValue( FValue );
//			inherited::SystemStart();
		}

	public:
		Button() :
			FLastValue( false ),
			FValue( false )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&Button::DoReceive );
		}

	};
}

#endif
