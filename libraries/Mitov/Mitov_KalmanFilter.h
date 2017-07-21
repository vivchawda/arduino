////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_KALMANFILTER_h
#define _MITOV_KALMANFILTER_h

#include <Mitov.h>
#include <Mitov_ComplementaryFilter.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	class BasicKalman : public BasicComplementary
	{
		typedef BasicComplementary inherited;

	public:
		float	ValueProcessNoise = 0.01;
		float	RateProcessNoise = 0.0003;
		float	ValueObservedNoise = 0.01;

	protected:
		float x_bias = 0;
		float P_00 = 0, P_01 = 0, P_10 = 0, P_11 = 0;	

	};
//---------------------------------------------------------------------------
	class KalmanAnalog : public BasicKalman
	{
		typedef BasicKalman inherited;

	protected:
		virtual void SystemLoopEnd() 
		{
			inherited::SystemLoopEnd();

			unsigned long ATime = micros();
			if( ! FHasTime )			
			{
				FHasTime = true;
				FLastTime = ATime;
				inherited::OutputPin.SetValue( inherited::InputPin.Value );
				return;
			}

			if( ! Enabled )
				return;

			float dt = ( ATime - FLastTime ) / 1000000.0f;
			FLastTime = ATime;

			float x_angle = inherited::OutputPin.Value;

			x_angle += dt * ( RateInputPin.Value - x_bias );

			P_00 +=  - dt * (P_10 + P_01) + ValueProcessNoise * dt;
			P_01 +=  - dt * P_11;
			P_10 +=  - dt * P_11;
			P_11 +=  + RateProcessNoise * dt;
    
			float y = inherited::InputPin.Value - x_angle;
			float S = P_00 + ValueObservedNoise;
			float K_0 = P_00 / S;
			float K_1 = P_10 / S;
    
			x_angle +=  K_0 * y;
			x_bias  +=  K_1 * y;
			P_00 -= K_0 * P_00;
			P_01 -= K_0 * P_01;
			P_10 -= K_1 * P_00;
			P_11 -= K_1 * P_01;

			inherited::OutputPin.SetValue( x_angle );
		}
	};
//---------------------------------------------------------------------------
	class KalmanAngle : public BasicKalman
	{
		typedef BasicKalman inherited;

	public:
		Mitov::TAngleUnits	Units = auDegree;

	protected:
		virtual void SystemLoopEnd() 
		{
			inherited::SystemLoopEnd();

			unsigned long ATime = micros();
			if( ! FHasTime )			
			{
				FHasTime = true;
				FLastTime = ATime;
				inherited::OutputPin.SetValue( inherited::InputPin.Value );
				return;
			}

			if( ! Enabled )
				return;

			float dt = ( ATime - FLastTime ) / 1000000.0f;
			FLastTime = ATime;

			float x_angle = inherited::OutputPin.Value;

			x_angle += dt * ( RateInputPin.Value - x_bias );

			P_00 +=  - dt * (P_10 + P_01) + ValueProcessNoise * dt;
			P_01 +=  - dt * P_11;
			P_10 +=  - dt * P_11;
			P_11 +=  + RateProcessNoise * dt;
    
			float y = inherited::InputPin.Value - x_angle;
			float S = P_00 + ValueObservedNoise;
			float K_0 = P_00 / S;
			float K_1 = P_10 / S;
    
			x_angle +=  K_0 * y;
			x_bias  +=  K_1 * y;
			P_00 -= K_0 * P_00;
			P_01 -= K_0 * P_01;
			P_10 -= K_1 * P_00;
			P_11 -= K_1 * P_01;

			switch( Units )
			{
				case auDegree:
					x_angle = posmod( x_angle, 360.0f );
					break;

				case auRadians:
					x_angle = posmod( x_angle, 2 * PI );
					break;

				case auNormalized:
					x_angle = posmod( x_angle, 1.0f );
					break;

			}

			inherited::OutputPin.SetValue( x_angle );
		}
	};
//---------------------------------------------------------------------------
}

#endif
