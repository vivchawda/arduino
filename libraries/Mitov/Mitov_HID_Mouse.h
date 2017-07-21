////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_HID_MOUSE_h
#define _MITOV_HID_MOUSE_h

#include <Mitov.h>

#ifndef VISUINO_TEENSY
  #include <Mouse.h>
#endif

namespace Mitov
{
	class MitovHIDMouse;
//---------------------------------------------------------------------------
	class TArduinoMousePosition : OpenWire::Object
	{
	public:
		OpenWire::SinkPin	XInputPin;
		OpenWire::SinkPin	YInputPin;
		OpenWire::SinkPin	ZeroInputPin;

	protected:
		MitovHIDMouse	*FOwner;

	protected:
		int32_t	FCurrentX = 0;
		int32_t	FCurrentY = 0;

	protected:
		void DoReceiveX( void *_Data );

		void DoReceiveY( void *_Data );

		void DoReceiveZero( void *_Data )
		{
			FCurrentX = 0;
			FCurrentY = 0;
		}

	public:
		TArduinoMousePosition( MitovHIDMouse *AOwner ) :
			FOwner( AOwner )
		{
			XInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&TArduinoMousePosition::DoReceiveX );
			YInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&TArduinoMousePosition::DoReceiveY );
			ZeroInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&TArduinoMousePosition::DoReceiveZero );
		}
	};
//---------------------------------------------------------------------------
	class TArduinoMouseWheelPosition : OpenWire::Object
	{
	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SinkPin	ZeroInputPin;

	protected:
		MitovHIDMouse	*FOwner;

	protected:
		int32_t	FCurrent = 0;

	protected:
		void DoReceive( void *_Data );

		void DoReceiveZero( void *_Data )
		{
			FCurrent = 0;
		}

	public:
		TArduinoMouseWheelPosition( MitovHIDMouse *AOwner ) :
			FOwner( AOwner )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&TArduinoMouseWheelPosition::DoReceive );
			ZeroInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&TArduinoMouseWheelPosition::DoReceiveZero );
		}
	};
//---------------------------------------------------------------------------
	class MitovHIDMouse : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	ButtonsInputPins[ 3 ];

	public:
		TArduinoMousePosition		Position;
		TArduinoMouseWheelPosition	Wheel;
		bool	Enabled : 1;

	protected:
		bool	FButton1 : 1;
		bool	FButton2 : 1;
		bool	FButton3 : 1;

	public:
		void UpdateEnabled()
		{
			if( Enabled )
				Mouse.begin();

			else
				Mouse.end();

		}

	protected:
		void DoButtonReceive1( void *_Data )
		{
			if( ! Enabled )
				return;

			bool AValue = *(bool *)_Data;
			if( FButton1 == AValue )
				return;

			FButton1 = AValue;
			if( AValue )
				Mouse.press( MOUSE_LEFT );

			else
				Mouse.release( MOUSE_LEFT );
		}

		void DoButtonReceive2( void *_Data )
		{
			if( ! Enabled )
				return;

			bool AValue = *(bool *)_Data;
			if( FButton2 == AValue )
				return;

			FButton2 = AValue;
			if( AValue )
				Mouse.press( MOUSE_MIDDLE );

			else
				Mouse.release( MOUSE_MIDDLE );
		}

		void DoButtonReceive3( void *_Data )
		{
			if( ! Enabled )
				return;

			bool AValue = *(bool *)_Data;
			if( FButton3 == AValue )
				return;

			FButton3 = AValue;
			if( AValue )
				Mouse.press( MOUSE_RIGHT );

			else
				Mouse.release( MOUSE_RIGHT );
		}

	protected:
		virtual void SystemInit() override
		{
//			inherited::SystemInit();
			if( Enabled )
				Mouse.begin();
		}

	public:
		MitovHIDMouse() :
			Position( this ),
			Wheel( this ),
			Enabled( true ),
			FButton1( false ),
			FButton2( false ),
			FButton3( false )
		{
			ButtonsInputPins[ 0 ].SetCallback( this, (OpenWire::TOnPinReceive)&MitovHIDMouse::DoButtonReceive1 );
			ButtonsInputPins[ 1 ].SetCallback( this, (OpenWire::TOnPinReceive)&MitovHIDMouse::DoButtonReceive2 );
			ButtonsInputPins[ 2 ].SetCallback( this, (OpenWire::TOnPinReceive)&MitovHIDMouse::DoButtonReceive3 );
		}

	};
//---------------------------------------------------------------------------
	class MitovHIDMouseElementMove : public OpenWire::Object
	{
	protected:
		MitovHIDMouse	&FOwner;

	public:
		OpenWire::SinkPin	ClockInputPin;

	public:
		int16_t	X = 0;
		int16_t	Y = 0;
		int16_t	Wheel = 0;

	protected:
		bool	FPressed = false;;

	protected:
		void DoReceiveClock( void *_Data )
		{
			if( FOwner.Enabled )
				Mouse.move( X, Y, Wheel );
		}

	public:
		MitovHIDMouseElementMove( MitovHIDMouse &AOwner ) :
			FOwner( AOwner )
		{
			ClockInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&MitovHIDMouseElementMove::DoReceiveClock );
		}

	};
//---------------------------------------------------------------------------
	void TArduinoMousePosition::DoReceiveX( void *_Data )
	{
		int32_t	AValue = *(int32_t *)_Data;
		if( FCurrentX == AValue )
			return;

		if( FOwner->Enabled )
		{
			Mouse.move( AValue - FCurrentX, 0, 0 );
			FCurrentX = AValue;
		}
	}
//---------------------------------------------------------------------------
	void TArduinoMousePosition::DoReceiveY( void *_Data )
	{
		int32_t	AValue = *(int32_t *)_Data;
		if( FCurrentY == AValue )
			return;

		if( FOwner->Enabled )
		{
			Mouse.move( 0, AValue - FCurrentY, 0 );
			FCurrentY = AValue;
		}
	}
//---------------------------------------------------------------------------
	void TArduinoMouseWheelPosition::DoReceive( void *_Data )
	{
		int32_t	AValue = *(int32_t *)_Data;
		if( FCurrent == AValue )
			return;

		if( FOwner->Enabled )
		{
			Mouse.move( 0, 0, AValue - FCurrent );
			FCurrent = AValue;
		}
	}
//---------------------------------------------------------------------------
}

#endif
