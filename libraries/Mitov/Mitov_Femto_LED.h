////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_FEMTO_LED_h
#define _MITOV_FEMTO_LED_h

#include <Mitov.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	class ArduinoFemtoLED : public OpenWire::Object
	{
		typedef OpenWire::Object inherited;

	public:
		OpenWire::SinkPin	InputPin;

	protected:
		void DoReceive( void *_Data )
		{
			TColor &AColor = *(TColor *)_Data;
			analogWrite( 4, AColor.Red );
			analogWrite( 3, AColor.Green );
			analogWrite( 10, AColor.Blue );
		}

	public:
		ArduinoFemtoLED()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&ArduinoFemtoLED::DoReceive );
		}
	};
//---------------------------------------------------------------------------
}

#endif
