////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_DYNAMIC_RGB_BAR_h
#define _MITOV_DYNAMIC_RGB_BAR_h

#include <Mitov.h>

namespace Mitov
{
	class MitovDynamicRGBBar : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SourcePin	*PixelOutputPins;
		OpenWire::SourcePin	ColorOutputPins[ 3 ];
		OpenWire::ValueSimpleSinkPin<Mitov::TColor> *InputPins;

	public:
		void UpdateInverted()
		{
			for( int i = 0; i < FCountPixels; ++i )
				PixelOutputPins[ i ].SendValue<bool>( Inverted );

			PixelOutputPins[ FPixel ].SendValue<bool>( ! Inverted );
		}

	public:
		unsigned long	RefreshInterval = 1000;
		bool			Inverted = false;

	protected:
		unsigned long	FLastTime = 0;
		uint8_t			FCountPixels;
		uint8_t			FPixel = 0;

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( ( currentMicros - FLastTime ) < RefreshInterval )
				return;

			PixelOutputPins[ FPixel ].SendValue<bool>( Inverted );

			++FPixel;

			if( FPixel >= FCountPixels )
				FPixel = 0;

			Mitov::TColor AColor = InputPins[ FPixel ].Value;

			ColorOutputPins[ 0 ].SendValue<float>( float( AColor.Red ) / 255 );
			ColorOutputPins[ 1 ].SendValue<float>( float( AColor.Green ) / 255 );
			ColorOutputPins[ 2 ].SendValue<float>( float( AColor.Blue ) / 255 );

			PixelOutputPins[ FPixel ].SendValue<bool>( ! Inverted );
			FLastTime = currentMicros;
//			UpdateDisplay();
		}

		virtual void SystemInit() override
		{
//			inherited::SystemInit();
			for( int i = 0; i < FCountPixels; ++i )
				PixelOutputPins[ i ].SendValue<bool>( Inverted );

			FLastTime = micros();
		}

	public:
		MitovDynamicRGBBar( int ACountPixels, Mitov::TColor AInitialColor ) :
			FCountPixels( ACountPixels ),
			FPixel( FCountPixels - 1 )
		{			
			PixelOutputPins = new OpenWire::SourcePin[ ACountPixels ];
			InputPins = new OpenWire::ValueSimpleSinkPin<Mitov::TColor>[ ACountPixels ];
			for( int i = 0; i < ACountPixels; ++i )
				InputPins[ i ].Value = AInitialColor;
		}

	};
}

#endif
