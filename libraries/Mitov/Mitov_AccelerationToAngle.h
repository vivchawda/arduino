////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_ACCELERATIONTOANGLE_h
#define _MITOV_ACCELERATIONTOANGLE_h

#include <Mitov.h>

namespace Mitov
{
	class AccelerationToAngle : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::ValueSinkPin<float>	InputPins[ 3 ];
		OpenWire::TypedSourcePin<float>	OutputPins[ 3 ];

		OpenWire::TypedSourcePin<float>	ForceOutputPin;

	public:
		Mitov::TAngleUnits	Units : 2;

	protected:
		bool	FChangeOnly : 1;
		bool	FModified : 1;

	protected:
		float ComputeAngle( float a, float b )
		{
			// Hold the module so that Z is pointing 'up' and you can measure the heading with x&y
			// Calculate heading when the magnetometer is level, then correct for signs of axis.
			float AHeading = atan2( a, b ); // Y , X

//			Serial.println( AHeading );

			// Once you have your heading, you must then add your 'Declination Angle', which is the 'Error' of the magnetic field in your location.
			// Find yours here: http://www.magnetic-declination.com/
			// If you cannot find your Declination, comment out these two lines, your compass will be slightly off.

			// Correct for when signs are reversed.
			AHeading = posmod( AHeading, 2*PI );
/*
			if(AHeading < 0)
				AHeading += 2*PI;
    
			// Check for wrap due to addition of declination.
			if(AHeading > 2*PI)
				AHeading -= 2*PI;
*/
			switch( Units )
			{
				case auDegree:
					AHeading *= 180.0f / PI;
					break;

				case auNormalized:
					AHeading /= 2 * PI;
					break;

			}

			return AHeading;
		}

	protected:
		virtual void SystemLoopEnd() 
		{
			inherited::SystemLoopEnd();
			if( ! FModified )
				return;

			float AValue;

			AValue = ComputeAngle( InputPins[ 2 ].Value, InputPins[ 1 ].Value );
			OutputPins[ 0 ].SetValue( AValue, FChangeOnly );

			AValue = ComputeAngle( InputPins[ 2 ].Value, InputPins[ 0 ].Value );
			OutputPins[ 1 ].SetValue( AValue, FChangeOnly );

			AValue = ComputeAngle( InputPins[ 1 ].Value, InputPins[ 0 ].Value );
			OutputPins[ 2 ].SetValue( AValue, FChangeOnly );

			AValue = sqrt( InputPins[ 0 ].Value * InputPins[ 0 ].Value + InputPins[ 1 ].Value * InputPins[ 1 ].Value + InputPins[ 2 ].Value * InputPins[ 2 ].Value );
			ForceOutputPin.SetValue( AValue, FChangeOnly );

			FChangeOnly = true;
		}

		void DoInputChange( void * )
		{
			FModified = true;
		}

	public:
		AccelerationToAngle() :
			Units( auDegree ),
			FChangeOnly( false ),
			FModified( true )
		{
			for( int i = 0; i < 3; ++i )
				InputPins[ i ].SetCallback( MAKE_CALLBACK( AccelerationToAngle::DoInputChange ));
		}
	};
}

#endif
