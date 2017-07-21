////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_COMPLEMENTARYFILTER_h
#define _MITOV_COMPLEMENTARYFILTER_h

#include <Mitov.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	class BasicComplementary : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::ValueSinkPin<float>			InputPin;
		OpenWire::ValueSimpleSinkPin<float>		RateInputPin;
		OpenWire::TypedStartSourcePin<float>	OutputPin;

	public:
		bool	Enabled : 1;

	protected:
		bool			FHasTime : 1;
		unsigned long	FLastTime = 0;

	protected:
		void DoReceive( void *_Data )
		{
			if( ! Enabled )
				OutputPin.SetValue( *(float *)_Data );

		}

	public:
		BasicComplementary() :
			FHasTime( false ),
			Enabled( true )
		{
			InputPin.SetCallback( MAKE_CALLBACK( BasicComplementary::DoReceive ) );
		}

	};
//---------------------------------------------------------------------------
	class BasicFirstComplementary : public BasicComplementary
	{
		typedef BasicComplementary inherited;

	public:
		float	TimeConstant = 0.075f;

	};
//---------------------------------------------------------------------------
	class BasicSecondComplementary : public BasicComplementary
	{
		typedef BasicComplementary inherited;

	public:
		float	Coefficient = 10.0f;

	};
//---------------------------------------------------------------------------
	class ComplementaryAnalog : public BasicFirstComplementary
	{
		typedef BasicFirstComplementary inherited;

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

			float dT = ( ATime - FLastTime ) / 1000000.0f;
			FLastTime = ATime;

			float a = TimeConstant / ( TimeConstant + dT );

			float AAngle = a * ( inherited::OutputPin.Value + RateInputPin.Value * dT) + (1-a) * InputPin.Value;

			inherited::OutputPin.SetValue( AAngle );
		}
	};
//---------------------------------------------------------------------------
	class ComplementaryAngle : public BasicFirstComplementary
	{
		typedef BasicFirstComplementary inherited;

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

			float dT = ( ATime - FLastTime ) / 1000000.0f;
			FLastTime = ATime;

			float a = TimeConstant / ( TimeConstant + dT );

			float AAngle = a * ( inherited::OutputPin.Value + RateInputPin.Value * dT ) + (1-a) * InputPin.Value;

			switch( Units )
			{
				case auDegree:
					AAngle = posmod( AAngle, 360.0f );
					break;

				case auRadians:
					AAngle = posmod( AAngle, 2 * PI );
					break;

				case auNormalized:
					AAngle = posmod( AAngle, 1.0f );
					break;

			}

			inherited::OutputPin.SetValue( AAngle );
		}
	};
//---------------------------------------------------------------------------
	class Complementary2 : public BasicSecondComplementary
	{
		typedef BasicSecondComplementary inherited;

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

			float dT = ( ATime - FLastTime ) / 1000000.0f;
			FLastTime = ATime;

			float x_angle2C = inherited::OutputPin.Value;
			float newAngle = InputPin.Value;

			float x1 = (newAngle -   x_angle2C) * Coefficient * Coefficient;
			float y1 = dT * x1 + y1;
			float x2 = y1 + (newAngle -   x_angle2C) * 2 * Coefficient + RateInputPin.Value;
			x_angle2C = dT * x2 + x_angle2C;

			inherited::OutputPin.SetValue( x_angle2C );
		}
	};
//---------------------------------------------------------------------------
	class ComplementaryAngle2 : public BasicSecondComplementary
	{
		typedef BasicSecondComplementary inherited;

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

			float dT = ( ATime - FLastTime ) / 1000000.0f;
			FLastTime = ATime;

			float x_angle2C = inherited::OutputPin.Value;
			float newAngle = InputPin.Value;

			float x1 = (newAngle -   x_angle2C) * Coefficient * Coefficient;
			float y1 = dT * x1 + y1;
			float x2 = y1 + (newAngle -   x_angle2C) * 2 * Coefficient + RateInputPin.Value;
			x_angle2C = dT * x2 + x_angle2C;

			switch( Units )
			{
				case auDegree:
					x_angle2C = posmod( x_angle2C, 360.0f );
					break;

				case auRadians:
					x_angle2C = posmod( x_angle2C, 2 * PI );
					break;

				case auNormalized:
					x_angle2C = posmod( x_angle2C, 1.0f );
					break;

			}

			inherited::OutputPin.SetValue( x_angle2C );
		}
	};
//---------------------------------------------------------------------------
}

#endif
