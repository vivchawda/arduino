////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_SPEED_CHANGE_METER_h
#define _MITOV_SPEED_CHANGE_METER_h

#include <Mitov.h>

namespace Mitov
{
	class SpeedChangeMeter : public Mitov::CommonFilter
	{
		typedef Mitov::CommonFilter inherited;

    public:
        bool Enabled : 1;

	protected:
		bool			FHasTime : 1;
		unsigned long	FLastTime = 0;
		float			FLastValue;

	protected:
		virtual void DoReceive( void *_Data )
		{
            if( ! Enabled )
				return;

			float AValue = *(float *)_Data;
			unsigned long	ATime = micros();
			if( ! FHasTime )
				FHasTime = true;

			else
			{
				float ASpeed = AValue - FLastValue;
				ASpeed /= ( ATime - FLastTime ) / 1000000.0f;
				OutputPin.Notify( &ASpeed );
			}

			FLastValue = AValue;
			FLastTime = ATime;
		}

	public:
		SpeedChangeMeter() :
			Enabled( true ),
			FHasTime( false )
		{
		}
	};
}

#endif
