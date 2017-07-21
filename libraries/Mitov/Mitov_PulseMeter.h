////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_PULSE_METER_h
#define _MITOV_PULSE_METER_h

#include <Mitov.h>

namespace Mitov
{
	class PulseMeter : public Mitov::CommonFilter
	{
		typedef Mitov::CommonFilter inherited;

    public:
        bool Enabled : 1;

	protected:
		bool			FOldValue : 1;
		unsigned long	FStartTime = 0;

	protected:
		virtual void DoReceive( void *_Data )
		{
            if( ! Enabled )
				return;

			bool AValue = *(bool *)_Data;
			if( FOldValue == AValue )
				return;

			unsigned long ANow = micros();
			FOldValue = AValue;
			if( AValue )
			{
				FStartTime = ANow;
				return;
			}

			uint32_t APeriod = ANow - FStartTime;

			OutputPin.Notify( &APeriod );
		}

	public:
		PulseMeter() :
			Enabled( true ),
			FOldValue( false )
		{
		}
	};
}

#endif
