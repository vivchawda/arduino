////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_DISPLAY_SSD1306_SPI_h
#define _MITOV_DISPLAY_SSD1306_SPI_h

#include <Mitov_Display_SSD1306.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	template<ArduinoDisplayOLEDType T_PARAM> class DisplaySSD1306SPI : public DisplaySSD1306<T_PARAM>
	{
		typedef DisplaySSD1306<T_PARAM> inherited;

	public:
		OpenWire::SourcePin	ChipSelectOutputPin;
		OpenWire::SourcePin	DataCommandOutputPin;

	protected:
		BasicSPI	&FSPI;

	protected:
		virtual void SendCommand( uint8_t ACommand ) override
		{
			ChipSelectOutputPin.SendValue<bool>( true );
			DataCommandOutputPin.SendValue<bool>( false );
			ChipSelectOutputPin.SendValue<bool>( false );

			FSPI.transfer( ACommand );

			ChipSelectOutputPin.SendValue<bool>( true );
		}

		virtual void SendBuffer() override
		{
			ChipSelectOutputPin.SendValue<bool>( true );
			DataCommandOutputPin.SendValue<bool>( true );
			ChipSelectOutputPin.SendValue<bool>( false );

			for (uint16_t i = 0; i < ( inherited::FWidth * inherited::FHeight / 8 ); ++i )
				FSPI.transfer( inherited::buffer[ i ] );

			ChipSelectOutputPin.SendValue<bool>( true );
		}

	public:
		DisplaySSD1306SPI( BasicSPI &ASPI, int16_t AWidth, int16_t AHeight, const unsigned char * AFont ) :
			inherited( AWidth, AHeight, AFont ),
			FSPI( ASPI )
		{
		}
	};
//---------------------------------------------------------------------------
	template<> void DisplaySSD1306SPI<odtSH1106>::SendBuffer()
	{
		byte height=64;
		byte width=132; 
		height >>= 3;
		width >>= 3;
		// I2C
		byte m_row = 0;
		byte m_col = 2;
		int p = 0;
		for ( byte i = 0; i < height; i++)
		{
			// send a bunch of data in one xmission
			SendCommand(0xB0 + i + m_row);//set page address
			SendCommand(m_col & 0xf);//set lower column address
			SendCommand(0x10 | (m_col >> 4));//set higher column address

			for( byte j = 0; j < 8; j++)
			{
				ChipSelectOutputPin.SendValue<bool>( true );
				DataCommandOutputPin.SendValue<bool>( true );
				ChipSelectOutputPin.SendValue<bool>( false );

				for ( byte k = 0; k < width; k++, p++)
					FSPI.transfer( inherited::buffer[ p ] );

				ChipSelectOutputPin.SendValue<bool>( true );
			}
		}
	}
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
