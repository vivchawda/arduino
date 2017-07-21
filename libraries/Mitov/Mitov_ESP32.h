////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ESP32_h
#define _MITOV_ESP32_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	class ESP32_DAC_DigitalChannel : public ArduinoDigitalChannel
	{
		typedef ArduinoDigitalChannel	inherited;
		
	protected:
		virtual void PinDirectionsInit() override
        {
			if( FRawInput )
				return;

            if( FIsOutput )
			{
				if( FIsAnalog )
					pinMode( FPinNumber, ANALOG );

				else
					pinMode( FPinNumber, OUTPUT );
			}

            else
				pinMode( FPinNumber, FIsPullUp ? INPUT_PULLUP : ( FIsPullDown ? INPUT_PULLDOWN : INPUT ) );
        }

		virtual void DoAnalogReceive( void *_Data ) override
        {
            if( FIsOutput && FIsAnalog )
            {
                float AValue = *(float*)_Data;
                dacWrite( FPinNumber, ( AValue * 255 ) + 0.5 );
            }
        }

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
