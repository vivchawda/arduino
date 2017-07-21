////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_HID_KEYBOARD_h
#define _MITOV_HID_KEYBOARD_h

#include <Mitov.h>

#ifndef VISUINO_TEENSY
  #include <Keyboard.h>
#endif

namespace Mitov
{
	class MitovHIDKeyboard : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		bool	Enabled = true;

	public:
		void UpdateEnabled()
		{
			if( Enabled )
				Keyboard.begin();

			else
				Keyboard.end();

		}

	public:
		template<typename T> void Print( T AValue )
		{
			if( Enabled )
			{
				Keyboard.println( AValue );
/*
				FSending = true;
				SendingOutputPin.SendValue( FSending );
				FStartTime = micros();
				GetStream().println( AValue );
*/
			}
		}

		void PrintChar( char AValue )
		{
			if( Enabled )
			{
				Keyboard.write( AValue );
/*
				FSending = true;
				SendingOutputPin.SendValue( FSending );
				FStartTime = micros();
				GetStream().print( AValue );
*/
			}
		}

		void Write( uint8_t *AData, uint32_t ASize )
		{
			for( uint32_t i = 0; i < ASize; ++ i )
				Keyboard.write( AData[ i ] );

//			GetStream().write( AData, ASize );
		}

	protected:
		virtual void SystemInit() override
		{
//			inherited::SystemInit();
			if( Enabled )
				Keyboard.begin();
		}

	};
//---------------------------------------------------------------------------
	class MitovHIDKeyboardElementKey : public OpenWire::Object
	{
	protected:
		MitovHIDKeyboard	&FOwner;

	public:
		OpenWire::SinkPin	InputPin;

	public:
		char	Character = 0;

	protected:
		bool	FPressed = false;;

	protected:
		void DoReceive( void *_Data )
		{
			if( FOwner.Enabled )
			{
				bool AValue = *(bool *)_Data;
				if( AValue != FPressed )
				{
					FPressed = AValue;
					if( AValue )
						Keyboard.press( Character );

					else
						Keyboard.release( Character );

				}
			}
		}

	public:
		MitovHIDKeyboardElementKey( MitovHIDKeyboard &AOwner ) :
			FOwner( AOwner )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&MitovHIDKeyboardElementKey::DoReceive );
		}

	};
//---------------------------------------------------------------------------
}

#endif
