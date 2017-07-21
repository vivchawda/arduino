////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_DISPLAY_ST7735_h
#define _MITOV_DISPLAY_ST7735_h

#include <Mitov_Basic_SPI.h>
#include <Mitov_Graphics_Color.h>

#ifdef _MITOV_DISPLAY_ST7735R_BLACKTAB_
  #define _MITOV_DISPLAY_ST7735R_REDTAB_
#endif

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
namespace ST7735
{
	const uint8_t	NOP		= 0x00;
	const uint8_t	SWRESET	= 0x01;
	const uint8_t	RDDID	= 0x04;
	const uint8_t	RDDST	= 0x09;

	const uint8_t	SLPIN	= 0x10;
	const uint8_t	SLPOUT	= 0x11;
	const uint8_t	PTLON	= 0x12;
	const uint8_t	NORON	= 0x13;

	const uint8_t	INVOFF	= 0x20;
	const uint8_t	INVON	= 0x21;
	const uint8_t	DISPOFF	= 0x28;
	const uint8_t	DISPON	= 0x29;
	const uint8_t	CASET	= 0x2A;
	const uint8_t	RASET	= 0x2B;
	const uint8_t	RAMWR	= 0x2C;
	const uint8_t	RAMRD	= 0x2E;

	const uint8_t	PTLAR	= 0x30;
	const uint8_t	COLMOD	= 0x3A;
	const uint8_t	MADCTL	= 0x36;

	const uint8_t	FRMCTR1	= 0xB1;
	const uint8_t	FRMCTR2	= 0xB2;
	const uint8_t	FRMCTR3	= 0xB3;
	const uint8_t	INVCTR	= 0xB4;
	const uint8_t	DISSET5	= 0xB6;

	const uint8_t	PWCTR1	= 0xC0;
	const uint8_t	PWCTR2	= 0xC1;
	const uint8_t	PWCTR3	= 0xC2;
	const uint8_t	PWCTR4	= 0xC3;
	const uint8_t	PWCTR5  = 0xC4;
	const uint8_t	VMCTR1  = 0xC5;

	const uint8_t	RDID1   = 0xDA;
	const uint8_t	RDID2   = 0xDB;
	const uint8_t	RDID3   = 0xDC;
	const uint8_t	RDID4   = 0xDD;

	const uint8_t	PWCTR6  = 0xFC;

	const uint8_t	GMCTRP1 = 0xE0;
	const uint8_t	GMCTRN1 = 0xE1;

	const uint8_t	DELAY	= 0x80;

	const uint8_t	MADCTL_MY  = 0x80;
	const uint8_t	MADCTL_MX  = 0x40;
	const uint8_t	MADCTL_MV  = 0x20;
	const uint8_t	MADCTL_ML  = 0x10;
	const uint8_t	MADCTL_RGB = 0x00;
	const uint8_t	MADCTL_BGR = 0x08;
	const uint8_t	MADCTL_MH  = 0x04;
}

static const uint8_t PROGMEM
  Rcmd1[] = {                 // Init for 7735R, part 1 (red or green tab)
    15,                       // 15 commands in list:
    ST7735::SWRESET,   ST7735::DELAY,  //  1: Software reset, 0 args, w/delay
      150,                    //     150 ms delay
    ST7735::SLPOUT ,   ST7735::DELAY,  //  2: Out of sleep mode, 0 args, w/delay
      255,                    //     500 ms delay
    ST7735::FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735::FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735::FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
      0x01, 0x2C, 0x2D,       //     Dot inversion mode
      0x01, 0x2C, 0x2D,       //     Line inversion mode
    ST7735::INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
      0x07,                   //     No inversion
    ST7735::PWCTR1 , 3      ,  //  7: Power control, 3 args, no delay:
      0xA2,
      0x02,                   //     -4.6V
      0x84,                   //     AUTO mode
    ST7735::PWCTR2 , 1      ,  //  8: Power control, 1 arg, no delay:
      0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
    ST7735::PWCTR3 , 2      ,  //  9: Power control, 2 args, no delay:
      0x0A,                   //     Opamp current small
      0x00,                   //     Boost frequency
    ST7735::PWCTR4 , 2      ,  // 10: Power control, 2 args, no delay:
      0x8A,                   //     BCLK/2, Opamp current small & Medium low
      0x2A,  
    ST7735::PWCTR5 , 2      ,  // 11: Power control, 2 args, no delay:
      0x8A, 0xEE,
    ST7735::VMCTR1 , 1      ,  // 12: Power control, 1 arg, no delay:
      0x0E,
    ST7735::INVOFF , 0      ,  // 13: Don't invert display, no args, no delay
    ST7735::MADCTL , 1      ,  // 14: Memory access control (directions), 1 arg:
      0xC8,                   //     row addr/col addr, bottom to top refresh
    ST7735::COLMOD , 1      ,  // 15: set color mode, 1 arg, no delay:
      0x05 };                 //     16-bit color

#ifdef _MITOV_DISPLAY_ST7735R_GREENTAB_
static const uint8_t PROGMEM
  Rcmd2green[] = {            // Init for 7735R, part 2 (green tab only)
    2,                        //  2 commands in list:
    ST7735::CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 0
      0x00, 0x7F+0x02,        //     XEND = 127
    ST7735::RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x01,             //     XSTART = 0
      0x00, 0x9F+0x01 };      //     XEND = 159

#endif
//---------------------------------------------------------------------------
#ifdef _MITOV_DISPLAY_ST7735R_REDTAB_
static const uint8_t PROGMEM
  Rcmd2red[] = {              // Init for 7735R, part 2 (red tab only)
    2,                        //  2 commands in list:
    ST7735::CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F,             //     XEND = 127
    ST7735::RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x9F };           //     XEND = 159
#endif
//---------------------------------------------------------------------------
#ifdef _MITOV_DISPLAY_ST7735R_GREENTAB_144_
static const uint8_t PROGMEM
  Rcmd2green144[] = {              // Init for 7735R, part 2 (green 1.44 tab)
    2,                        //  2 commands in list:
    ST7735::CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F,             //     XEND = 127
    ST7735::RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F };           //     XEND = 127
#endif
//---------------------------------------------------------------------------
#ifndef _MITOV_DISPLAY_ST7735B_
static const uint8_t PROGMEM
  Rcmd3[] = {                 // Init for 7735R, part 3 (red or green tab)
    4,                        //  4 commands in list:
    ST7735::GMCTRP1, 16      , //  1: Magical unicorn dust, 16 args, no delay:
      0x02, 0x1c, 0x07, 0x12,
      0x37, 0x32, 0x29, 0x2d,
      0x29, 0x25, 0x2B, 0x39,
      0x00, 0x01, 0x03, 0x10,
    ST7735::GMCTRN1, 16      , //  2: Sparkles and rainbows, 16 args, no delay:
      0x03, 0x1d, 0x07, 0x06,
      0x2E, 0x2C, 0x29, 0x2D,
      0x2E, 0x2E, 0x37, 0x3F,
      0x00, 0x00, 0x02, 0x10,
    ST7735::NORON  ,    ST7735::DELAY, //  3: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    ST7735::DISPON ,    ST7735::DELAY, //  4: Main screen turn on, no args w/delay
      100 };                  //     100 ms delay
#endif
//---------------------------------------------------------------------------
#ifdef _MITOV_DISPLAY_ST7735B_
static const uint8_t PROGMEM
  Bcmd[] = {                  // Initialization commands for 7735B screens
    18,                       // 18 commands in list:
    ST7735::SWRESET,   ST7735::DELAY,  //  1: Software reset, no args, w/delay
      50,                     //     50 ms delay
    ST7735::SLPOUT ,   ST7735::DELAY,  //  2: Out of sleep mode, no args, w/delay
      255,                    //     255 = 500 ms delay
    ST7735::COLMOD , 1+ST7735::DELAY,  //  3: Set color mode, 1 arg + delay:
      0x05,                   //     16-bit color
      10,                     //     10 ms delay
    ST7735::FRMCTR1, 3+ST7735::DELAY,  //  4: Frame rate control, 3 args + delay:
      0x00,                   //     fastest refresh
      0x06,                   //     6 lines front porch
      0x03,                   //     3 lines back porch
      10,                     //     10 ms delay
    ST7735::MADCTL , 1      ,  //  5: Memory access ctrl (directions), 1 arg:
      0x08,                   //     Row addr/col addr, bottom to top refresh
    ST7735::DISSET5, 2      ,  //  6: Display settings #5, 2 args, no delay:
      0x15,                   //     1 clk cycle nonoverlap, 2 cycle gate
                              //     rise, 3 cycle osc equalize
      0x02,                   //     Fix on VTL
    ST7735::INVCTR , 1      ,  //  7: Display inversion control, 1 arg:
      0x0,                    //     Line inversion
    ST7735::PWCTR1 , 2+ST7735::DELAY,  //  8: Power control, 2 args + delay:
      0x02,                   //     GVDD = 4.7V
      0x70,                   //     1.0uA
      10,                     //     10 ms delay
    ST7735::PWCTR2 , 1      ,  //  9: Power control, 1 arg, no delay:
      0x05,                   //     VGH = 14.7V, VGL = -7.35V
    ST7735::PWCTR3 , 2      ,  // 10: Power control, 2 args, no delay:
      0x01,                   //     Opamp current small
      0x02,                   //     Boost frequency
    ST7735::VMCTR1 , 2+ST7735::DELAY,  // 11: Power control, 2 args + delay:
      0x3C,                   //     VCOMH = 4V
      0x38,                   //     VCOML = -1.1V
      10,                     //     10 ms delay
    ST7735::PWCTR6 , 2      ,  // 12: Power control, 2 args, no delay:
      0x11, 0x15,
    ST7735::GMCTRP1,16      ,  // 13: Magical unicorn dust, 16 args, no delay:
      0x09, 0x16, 0x09, 0x20, //     (seriously though, not sure what
      0x21, 0x1B, 0x13, 0x19, //      these config values represent)
      0x17, 0x15, 0x1E, 0x2B,
      0x04, 0x05, 0x02, 0x0E,
    ST7735::GMCTRN1,16+ST7735::DELAY,  // 14: Sparkles and rainbows, 16 args + delay:
      0x0B, 0x14, 0x08, 0x1E, //     (ditto)
      0x22, 0x1D, 0x18, 0x1E,
      0x1B, 0x1A, 0x24, 0x2B,
      0x06, 0x06, 0x02, 0x0F,
      10,                     //     10 ms delay
    ST7735::CASET  , 4      ,  // 15: Column addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 2
      0x00, 0x81,             //     XEND = 129
    ST7735::RASET  , 4      ,  // 16: Row addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 1
      0x00, 0x81,             //     XEND = 160
    ST7735::NORON  ,   ST7735::DELAY,  // 17: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    ST7735::DISPON ,   ST7735::DELAY,  // 18: Main screen turn on, no args, w/delay
      255 };                  //     255 = 500 ms delay

#endif
//---------------------------------------------------------------------------
	class DisplayST7735Basic : public OpenWire::Component, public GraphicsImpl //, public GraphicsImpl
	{
		typedef OpenWire::Component inherited;

	public:
		TGraphicsColorTextSettings	Text;

	protected:
#if defined (SPI_HAS_TRANSACTION)
		SPISettings mySPISettings;
#elif defined (__AVR__)
		static uint8_t SPCRbackup;
		static uint8_t mySPCR;
#endif
	protected:
		uint8_t  colstart, rowstart; // some displays need this changed

	public:
		bool	Inverted = false;

	public:
		void SetInverted( bool AValue )
		{
			if( Inverted == AValue )
				return;

			Inverted = AValue;
			UpdateInverted();
		}

	public:
		void SetOrientation( GraphicsOrientation AValue )
		{
			if( Orientation == AValue )
				return;

			Orientation = AValue;
			UpdateOrientation();
		}

	protected:
		BasicSPI	&FSPI;

	protected:
#ifdef	_MITOV_ST7735_RESET_PIN_
		virtual void SetResetOutput( bool AValue ) = 0;
		virtual bool ResetOutputPinIsConnected() { return true; }
#endif
		virtual void SetRegisterSelectOutput( bool AValue ) = 0;
		virtual void SetChipSelectOutput( bool AValue ) = 0;

	protected:
		virtual void UpdateOrientation() = 0;

		inline void UpdateInverted()
		{
			writecommand( ( Inverted ) ? ST7735::INVON : ST7735::INVOFF );
		}

		inline void spiwrite(uint8_t c) 
		{
		  //Serial.println(c, HEX);

#if defined (SPI_HAS_TRANSACTION)
		  FSPI.transfer(c);
#elif defined (__AVR__)
		  SPCRbackup = SPCR;
		  SPCR = mySPCR;
		  FSPI.transfer(c);
		  SPCR = SPCRbackup;
//      SPDR = c;
//      while(!(SPSR & _BV(SPIF)));
#elif defined (__arm__)
		  FSPI.setClockDivider(21); //4MHz
		  FSPI.setDataMode(SPI_MODE0);
		  FSPI.transfer(c);
#endif
		}

/*
		inline uint8_t spiread()
		{
		  //Serial.println(c, HEX);

#if defined (SPI_HAS_TRANSACTION)
			uint8_t AResult = FSPI.transfer(0);
#elif defined (__AVR__)
			SPCRbackup = SPCR;
			SPCR = mySPCR;
			uint8_t AResult = FSPI.transfer(0);
			SPCR = SPCRbackup;
//      SPDR = c;
//      while(!(SPSR & _BV(SPIF)));
#elif defined (__arm__)
			FSPI.setClockDivider(21); //4MHz
			FSPI.setDataMode(SPI_MODE0);
			uint8_t AResult = FSPI.transfer(0);
#endif
			return AResult;
		}
*/
		void writecommand(uint8_t c) 
		{
#if defined (SPI_HAS_TRANSACTION)
			FSPI.beginTransaction(mySPISettings);
#endif
			SetRegisterSelectOutput( false );
			SetChipSelectOutput( false );

			spiwrite(c);

			SetChipSelectOutput( true );

#if defined (SPI_HAS_TRANSACTION)
			FSPI.endTransaction();
#endif
		}

		void writedata(uint8_t c) 
		{
#if defined (SPI_HAS_TRANSACTION)
			FSPI.beginTransaction(mySPISettings);
#endif
			SetRegisterSelectOutput( true );
			SetChipSelectOutput( false );
    
			spiwrite(c);

			SetChipSelectOutput( true );
#if defined (SPI_HAS_TRANSACTION)
			FSPI.endTransaction();
#endif
		}

		void commonInit(const uint8_t *cmdList) 
		{
#if defined (SPI_HAS_TRANSACTION)
//			FSPI.begin();
			mySPISettings = SPISettings(8000000, MSBFIRST, SPI_MODE0);
#elif defined (__AVR__)
			SPCRbackup = SPCR;
//			FSPI.begin();
			FSPI.setClockDivider(SPI_CLOCK_DIV4);
			FSPI.setDataMode(SPI_MODE0);
			mySPCR = SPCR; // save our preferred state
			//Serial.print("mySPCR = 0x"); Serial.println(SPCR, HEX);
			SPCR = SPCRbackup;  // then restore
#elif defined (__SAM3X8E__)
//			FSPI.begin();
			FSPI.setClockDivider(21); //4MHz
			FSPI.setDataMode(SPI_MODE0);
#endif

#ifdef	_MITOV_ST7735_RESET_PIN_
			if( ResetOutputPinIsConnected()) 
			{
				SetResetOutput( true );
				delay(500);
				SetResetOutput( false );
				delay(500);
				SetResetOutput( true );
				delay(500);
			}
#endif

			commandList(cmdList);
		}

		// Companion code to the above tables.  Reads and issues
		// a series of LCD commands stored in PROGMEM byte array.
		void commandList(const uint8_t *addr) 
		{
			uint8_t  numCommands, numArgs;
			uint16_t ms;

			numCommands = pgm_read_byte(addr++);   // Number of commands to follow
			while(numCommands--) 
			{                 // For each command...
				writecommand(pgm_read_byte(addr++)); //   Read, issue command
				numArgs  = pgm_read_byte(addr++);    //   Number of args to follow
				ms       = numArgs & ST7735::DELAY;          //   If hibit set, delay follows args
				numArgs &= ~ST7735::DELAY;                   //   Mask out delay bit
				while(numArgs--)                     //   For each argument...
					writedata(pgm_read_byte(addr++));  //     Read, issue argument

				if(ms) 
				{
					ms = pgm_read_byte(addr++); // Read post-command delay time (ms)
					if(ms == 255) 
						ms = 500;     // If 255, delay for 500 ms

					delay(ms);
				}
			}
		}

		void setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1 )
		{
			writecommand(ST7735::CASET); // Column addr set
			writedata(0x00);
			writedata(x0+colstart);     // XSTART 
			writedata(0x00);
			writedata(x1+colstart);     // XEND

			writecommand(ST7735::RASET); // Row addr set
			writedata(0x00);
			writedata(y0+rowstart);     // YSTART
			writedata(0x00);
			writedata(y1+rowstart);     // YEND

			writecommand( ST7735::RAMWR ); // write to RAM
		}

	public:

#ifdef _GRAPHICS_DRAW_ALPHA_COLOR_BITMAP_

		virtual void drawAlphaColorBitmap( int16_t x, int16_t y, const uint32_t *bitmap, int16_t w, int16_t h ) override
		{
			uint16_t AHeight = height();
			uint16_t AWidth = width();

			if( ( x < 0 ) || ( y < 0 ) || ( x + w > AWidth ) || ( x + w > AHeight ))
			{
				GraphicsImpl::drawAlphaColorBitmap( x, y, bitmap, w, h );
				return;
			}

			// check rotation, move pixel around if necessary

			setAddrWindow(x,y,x + w - 1, y + h - 1);
#if defined (SPI_HAS_TRANSACTION)
			SPI.beginTransaction(mySPISettings);
#endif
			SetRegisterSelectOutput( true );
			SetChipSelectOutput( false );

			for( int16_t j=0; j<h; j++)
				for( int16_t i=0; i<w; ++i )
				{
					uint16_t AColor = Func::Color565( pgm_read_dword( bitmap++ ));
					spiwrite( AColor >> 8 );
					spiwrite( AColor );
				}

			SetChipSelectOutput( true );
#if defined (SPI_HAS_TRANSACTION)
			SPI.endTransaction();
#endif
		}

#endif // _GRAPHICS_DRAW_ALPHA_COLOR_BITMAP_

#ifdef _GRAPHICS_DRAW_COLOR_BITMAP_
		virtual void drawColorBitmap( int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h ) override
		{
			uint16_t AHeight = height();
			uint16_t AWidth = width();

			if( ( x < 0 ) || ( y < 0 ) || ( x + w > AWidth ) || ( x + w > AHeight ))
			{
				GraphicsImpl::drawColorBitmap( x, y, bitmap, w, h );
				return;
			}

			setAddrWindow(x,y,x + w - 1, y + h - 1);
#if defined (SPI_HAS_TRANSACTION)
			SPI.beginTransaction(mySPISettings);
#endif
			SetRegisterSelectOutput( true );
			SetChipSelectOutput( false );

			for( int16_t j=0; j<h; j++)
				for( int16_t i=0; i<w; ++i )
				{
					uint32_t ARed = pgm_read_byte( bitmap ++ );
					uint32_t AGreen = pgm_read_byte( bitmap ++ );
					uint32_t ABlue = pgm_read_byte( bitmap ++ );
					uint16_t AColor = Func::Color565( ( AGreen << 8 ) | ( ARed << 16 ) | ABlue );

					spiwrite( AColor >> 8 );
					spiwrite( AColor );
				}

			SetChipSelectOutput( true );
#if defined (SPI_HAS_TRANSACTION)
			SPI.endTransaction();
#endif
		}

#endif // _GRAPHICS_DRAW_COLOR_BITMAP_

#ifdef _GRAPHICS_DRAW_GRAYSCALE_BITMAP_

		virtual void drawGrayscaleBitmap( int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint8_t AAlpha ) override
		{
			uint16_t AHeight = height();
			uint16_t AWidth = width();

			if( ( x < 0 ) || ( y < 0 ) || ( x + w > AWidth ) || ( x + w > AHeight ))
			{
				GraphicsImpl::drawGrayscaleBitmap( x, y, bitmap, w, h, AAlpha );
				return;
			}

			setAddrWindow(x,y,x + w - 1, y + h - 1);
#if defined (SPI_HAS_TRANSACTION)
			SPI.beginTransaction(mySPISettings);
#endif
			SetRegisterSelectOutput( true );
			SetChipSelectOutput( false );

			for( int16_t j=0; j<h; j++)
				for( int16_t i=0; i<w; ++i )
				{
					uint32_t AGray = pgm_read_byte( bitmap ++ );
					uint16_t AColor = Func::Color565( ( AGray << 8 ) | ( AGray << 16 ) | AGray );

					spiwrite( AColor >> 8 );
					spiwrite( AColor );
				}

			SetChipSelectOutput( true );
#if defined (SPI_HAS_TRANSACTION)
			SPI.endTransaction();
#endif
		}

#endif // _GRAPHICS_DRAW_GRAYSCALE_BITMAP_

#ifdef _GRAPHICS_DRAW_GRAYSCALE_ALPHA_BITMAP_

		virtual void drawAlphaGrayscaleBitmap( int16_t x, int16_t y, const uint16_t *_bitmap, int16_t w, int16_t h ) override
		{
			uint16_t AHeight = height();
			uint16_t AWidth = width();

			if( ( x < 0 ) || ( y < 0 ) || ( x + w > AWidth ) || ( x + w > AHeight ))
			{
				GraphicsImpl::drawAlphaGrayscaleBitmap( x, y, _bitmap, w, h );
				return;
			}

			const uint8_t *bitmap = (const uint8_t *)_bitmap;

			setAddrWindow(x,y,x + w - 1, y + h - 1);
#if defined (SPI_HAS_TRANSACTION)
			SPI.beginTransaction(mySPISettings);
#endif
			SetRegisterSelectOutput( true );
			SetChipSelectOutput( false );

			for( int16_t j=0; j<h; j++)
				for( int16_t i=0; i<w; ++i )
				{
					uint32_t AGray = pgm_read_byte( bitmap );
					uint16_t AColor = Func::Color565( ( AGray << 8 ) | ( AGray << 16 ) | AGray );
					bitmap += 2;

					spiwrite( AColor >> 8 );
					spiwrite( AColor );
				}

			SetChipSelectOutput( true );
#if defined (SPI_HAS_TRANSACTION)
			SPI.endTransaction();
#endif
		}

#endif // _GRAPHICS_DRAW_GRAYSCALE_ALPHA_BITMAP_

#ifdef _GRAPHICS_DRAW_BLACK_WHITE_BITMAP_

		virtual void drawBWBitmap( int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, TUniColor color, TUniColor bg ) override
		{
			uint16_t AHeight = height();
			uint16_t AWidth = width();

			if( ( x < 0 ) || ( y < 0 ) || ( x + w > AWidth ) || ( x + w > AHeight ))
			{
				GraphicsImpl::drawBWBitmap( x, y, bitmap, w, h, color, bg );
				return;
			}

			int16_t byteWidth = (w + 7) / 8;

//			color = Func::Color565( color );
//			bg = Func::Color565( bg );
  
			setAddrWindow(x,y,x + w - 1, y + h - 1);
#if defined (SPI_HAS_TRANSACTION)
			SPI.beginTransaction(mySPISettings);
#endif
			SetRegisterSelectOutput( true );
			SetChipSelectOutput( false );

			for( int16_t j=0; j<h; j++)
				for( int16_t i=0; i<w; ++i )
				{
					uint16_t AColor;
					if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7)))
						AColor = 0xFFFF;

					else 
      					AColor = 0;

					spiwrite( AColor >> 8 );
					spiwrite( AColor );
				}

			SetChipSelectOutput( true );
#if defined (SPI_HAS_TRANSACTION)
			SPI.endTransaction();
#endif
		}

#endif // _GRAPHICS_DRAW_BLACK_WHITE_BITMAP_

#ifdef _GRAPHICS_DRAW_REPEAT_ALPHA_COLOR_BITMAP_

		virtual void drawRepeatAlphaColorBitmap( int16_t x, int16_t y, const uint8_t *bitmap, int16_t AListCount, int16_t w, int16_t h ) override
		{
			uint16_t AHeight = height();
			uint16_t AWidth = width();

			if( ( x < 0 ) || ( y < 0 ) || ( x + w > AWidth ) || ( x + w > AHeight ))
			{
				GraphicsImpl::drawRepeatAlphaColorBitmap( x, y, bitmap, AListCount, w, h );
				return;
			}

			setAddrWindow(x,y, x + w - 1, y + h - 1);

#if defined (SPI_HAS_TRANSACTION)
			SPI.beginTransaction(mySPISettings);
#endif
			SetRegisterSelectOutput( true );
			SetChipSelectOutput( false );

			for( int16_t i = 0; i < AListCount; ++i )
			{
				uint8_t ACount = pgm_read_byte( bitmap ++ );
				uint16_t AColor = Func::Color565( pgm_read_dword( bitmap ));
				uint8_t AHi = AColor >> 8;
				uint8_t ALo = AColor;
				bitmap += 4;
				for( int16_t j = 0; j <= ACount; ++ j )
				{
					spiwrite( AHi );
					spiwrite( ALo );
				}
			}

			SetChipSelectOutput( true );
#if defined (SPI_HAS_TRANSACTION)
			SPI.endTransaction();
#endif
		}

#endif // _GRAPHICS_DRAW_REPEAT_ALPHA_COLOR_BITMAP_

#ifdef _GRAPHICS_DRAW_REPEAT_COLOR_BITMAP_

		virtual void drawRepeatColorBitmap( int16_t x, int16_t y, const uint8_t *bitmap, int16_t AListCount, int16_t w, int16_t h ) override
		{
			uint16_t AHeight = height();
			uint16_t AWidth = width();

			if( ( x < 0 ) || ( y < 0 ) || ( x + w > AWidth ) || ( x + w > AHeight ))
			{
				GraphicsImpl::drawRepeatColorBitmap( x, y, bitmap, AListCount, w, h );
				return;
			}

			setAddrWindow(x,y, x + w - 1, y + h - 1);

#if defined (SPI_HAS_TRANSACTION)
			SPI.beginTransaction(mySPISettings);
#endif
			SetRegisterSelectOutput( true );
			SetChipSelectOutput( false );

			for( int16_t i = 0; i < AListCount; ++i )
			{
				uint8_t ACount = pgm_read_byte( bitmap ++ );

				uint32_t ARed = pgm_read_byte( bitmap ++ );
				uint32_t AGreen = pgm_read_byte( bitmap ++ );
				uint32_t ABlue = pgm_read_byte( bitmap ++ );
				uint16_t AColor = Func::Color565( ( ARed << 16 ) | ( AGreen << 8 ) | ABlue );

				uint8_t AHi = AColor >> 8;
				uint8_t ALo = AColor;
				for( int16_t j = 0; j <= ACount; ++ j )
				{
					spiwrite( AHi );
					spiwrite( ALo );
				}
			}

			SetChipSelectOutput( true );
#if defined (SPI_HAS_TRANSACTION)
			SPI.endTransaction();
#endif
		}

#endif // _GRAPHICS_DRAW_REPEAT_COLOR_BITMAP_

#ifdef _GRAPHICS_DRAW_GRAYSCALE_REPEAT_BITMAP_

		virtual void drawRepeatGrayscaleBitmap( int16_t x, int16_t y, const uint8_t *bitmap, int16_t AListCount, int16_t w, int16_t h, uint8_t AAlpha )  override
		{
			uint16_t AHeight = height();
			uint16_t AWidth = width();

			if( ( x < 0 ) || ( y < 0 ) || ( x + w > AWidth ) || ( x + w > AHeight ))
			{
				GraphicsImpl::drawRepeatGrayscaleBitmap( x, y, bitmap, AListCount, w, h, AAlpha );
				return;
			}

			setAddrWindow(x,y, x + w - 1, y + h - 1);

#if defined (SPI_HAS_TRANSACTION)
			SPI.beginTransaction(mySPISettings);
#endif
			SetRegisterSelectOutput( true );
			SetChipSelectOutput( false );

			for( int16_t i = 0; i < AListCount; ++i )
			{
				uint8_t ACount = pgm_read_byte( bitmap ++ );

				uint32_t AGray = pgm_read_byte( bitmap ++ );
				uint16_t AColor = Func::Color565( ( AGray << 8 ) | ( AGray << 16 ) | AGray );

				uint8_t AHi = AColor >> 8;
				uint8_t ALo = AColor;
				for( int16_t j = 0; j <= ACount; ++ j )
				{
					spiwrite( AHi );
					spiwrite( ALo );
				}
			}

			SetChipSelectOutput( true );
#if defined (SPI_HAS_TRANSACTION)
			SPI.endTransaction();
#endif
		}

#endif // _GRAPHICS_DRAW_GRAYSCALE_REPEAT_BITMAP_

#ifdef _GRAPHICS_DRAW_GRAYSCALE_REPEAT_ALPHA_BITMAP_

		virtual void drawAlphaGrayscaleRepeatBitmap( int16_t x, int16_t y, const uint8_t *bitmap, int16_t AListCount, int16_t w, int16_t h )  override
		{
			uint16_t AHeight = height();
			uint16_t AWidth = width();

			if( ( x < 0 ) || ( y < 0 ) || ( x + w > AWidth ) || ( x + w > AHeight ))
			{
				GraphicsImpl::drawAlphaGrayscaleRepeatBitmap( x, y, bitmap, AListCount, w, h );
				return;
			}

			setAddrWindow(x,y, x + w - 1, y + h - 1);

#if defined (SPI_HAS_TRANSACTION)
			SPI.beginTransaction(mySPISettings);
#endif
			SetRegisterSelectOutput( true );
			SetChipSelectOutput( false );

			for( int16_t i = 0; i < AListCount; ++i )
			{
				uint8_t ACount = pgm_read_byte( bitmap );

				bitmap += 2;

				uint32_t AGray = pgm_read_byte( bitmap ++ );
				uint16_t AColor = Func::Color565( ( AGray << 8 ) | ( AGray << 16 ) | AGray );

				uint8_t AHi = AColor >> 8;
				uint8_t ALo = AColor;
				for( int16_t j = 0; j <= ACount; ++ j )
				{
					spiwrite( AHi );
					spiwrite( ALo );
				}
			}

			SetChipSelectOutput( true );
#if defined (SPI_HAS_TRANSACTION)
			SPI.endTransaction();
#endif
		}

#endif // _GRAPHICS_DRAW_GRAYSCALE_REPEAT_ALPHA_BITMAP_

		virtual void drawGrayscalePixel(int16_t x, int16_t y, TUniColor color) override
		{
			uint16_t AGray = ( color & 0xFF );
			drawPixel( x, y, color << 16 | ( AGray << 8 ) | AGray );
		}

		virtual void drawPixel(int16_t x, int16_t y, TUniColor color) override
		{
			uint16_t AHeight = height();
			uint16_t AWidth = width();

			if((x < 0) ||(x >= AWidth) || (y < 0) || (y >= AHeight)) 
				return;

			color = Func::Color565( color );

			setAddrWindow(x,y,x+1,y+1);

#if defined (SPI_HAS_TRANSACTION)
			SPI.beginTransaction(mySPISettings);
#endif
			SetRegisterSelectOutput( true );
			SetChipSelectOutput( false );

			spiwrite(color >> 8);
			spiwrite(color);

			SetChipSelectOutput( true );
#if defined (SPI_HAS_TRANSACTION)
			SPI.endTransaction();
#endif
		}

		virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, TUniColor color) override
		{
			// Rudimentary clipping
			uint16_t AHeight = height();
			uint16_t AWidth = width();
			if((x >= AWidth ) || (y >= AHeight )) 
				return;

			if((x+w-1) >= AWidth )
				w = AWidth - x;

			color = Func::Color565( color );
			setAddrWindow(x, y, x+w-1, y);

			uint8_t hi = color >> 8, lo = color;

#if defined (SPI_HAS_TRANSACTION)
			SPI.beginTransaction(mySPISettings);
#endif
			SetRegisterSelectOutput( true );
			SetChipSelectOutput( false );
			while (w--) 
			{
				spiwrite(hi);
				spiwrite(lo);
			}

			SetChipSelectOutput( true );
#if defined (SPI_HAS_TRANSACTION)
			SPI.endTransaction();
#endif
		}

		virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, TUniColor color) override
		{
			// Rudimentary clipping
			uint16_t AHeight = height();
			uint16_t AWidth = width();
			if((x >= AWidth) || (y >= AHeight)) 
				return;

			if((y+h-1) >= AHeight) 
				h = AHeight-y;

			color = Func::Color565( color );
			setAddrWindow(x, y, x, y+h-1);

			uint8_t hi = color >> 8, lo = color;
    
		#if defined (SPI_HAS_TRANSACTION)
			SPI.beginTransaction(mySPISettings);
		#endif
			SetRegisterSelectOutput( true );
			SetChipSelectOutput( false );
			while (h--) 
			{
				spiwrite(hi);
				spiwrite(lo);
			}

			SetChipSelectOutput( true );
		#if defined (SPI_HAS_TRANSACTION)
			SPI.endTransaction();
		#endif
		}

		virtual TUniColor GetPixelColor( int16_t x, int16_t y ) override
		{
			return 0x0000; // Not supported
		}

		virtual void Scroll( TGraphicsPos X, TGraphicsPos Y, TUniColor color ) override
		{
			// Not supported
		}

/*
		virtual TUniColor GetPixelColor( int16_t x, int16_t y ) override
		{
			if((x < 0) || (x >= width() ) || (y < 0) || (y >= height()))
				return 0x0000;

			setAddrWindow(x,y,x+1,y+1, ST7735::RAMRD);
#if defined (SPI_HAS_TRANSACTION)
			SPI.beginTransaction(mySPISettings);
#endif
			SetRegisterSelectOutput( true );
			SetChipSelectOutput( false );

			uint16_t AColor = uint16_t( spiread()) << 8;
			AColor |= spiread();
//			spiwrite(color >> 8);
//			spiwrite(color);

			SetChipSelectOutput( true );
#if defined (SPI_HAS_TRANSACTION)
			SPI.endTransaction();
#endif
			return AColor;
		}
*/
		virtual void fillRect( int16_t x, int16_t y, int16_t w, int16_t h, TUniColor color) override
		{
			// rudimentary clipping (drawChar w/big text requires this)
			int16_t AWidth = width();
			int16_t AHeight = height();

			if((x >= AWidth) || (y >= AHeight))
				return;

			if((x + w - 1) >= AWidth)
				w = AWidth - x;

			if((y + h - 1) >= AHeight)
				h = AHeight - y;

			setAddrWindow(x, y, x+w-1, y+h-1);

			color = Func::Color565( color );

			uint8_t hi = color >> 8, lo = color;
    
#if defined (SPI_HAS_TRANSACTION)
			SPI.beginTransaction(mySPISettings);
#endif
			SetRegisterSelectOutput( true );
			SetChipSelectOutput( false );
			for(y=h; y>0; y--)
				for(x=w; x>0; x--)
				{
					spiwrite(hi);
					spiwrite(lo);
				}

			SetChipSelectOutput( true );
#if defined (SPI_HAS_TRANSACTION)
			SPI.endTransaction();
#endif
		}

		virtual void ClearScreen( TUniColor color ) override
		{
			fillRect( 0, 0, width(), height(), 0xFF000000 | color );
		}

	protected:
		virtual void SystemInit() override
		{
			UpdateOrientation();
			UpdateInverted();

			ClearScreen( BackgroundColor );

			setTextSize( Text.Size );
			setTextColor( Text.Color, Text.BackgroundColor );
			setTextWrap( Text.Wrap );

			for( int i = 0; i < FElements.size(); ++ i )
				FElements[ i ]->Render( false );

		}

	public:
		DisplayST7735Basic( BasicSPI &ASPI, const unsigned char * AFont ) :
			FSPI( ASPI ),
			GraphicsImpl( AFont )
		{
		}

	};
//---------------------------------------------------------------------------
	class DisplayST7735 : public DisplayST7735Basic
	{
		typedef DisplayST7735Basic inherited;

	public:
#ifdef	_MITOV_ST7735_RESET_PIN_
		OpenWire::SourcePin	ResetOutputPin;
#endif
		OpenWire::SourcePin	RegisterSelectOutputPin;
		OpenWire::SourcePin	ChipSelectOutputPin;

	public:
#ifdef	_MITOV_ST7735_RESET_PIN_
		virtual void SetResetOutput( bool AValue ) override
		{
			ResetOutputPin.SendValue<bool>( AValue );
		}

		virtual bool ResetOutputPinIsConnected() override
		{ 
			return ResetOutputPin.IsConnected(); 
		}

#endif
		virtual void SetRegisterSelectOutput( bool AValue ) override
		{
			RegisterSelectOutputPin.SendValue<bool>( AValue );
		}

		virtual void SetChipSelectOutput( bool AValue ) override
		{
			ChipSelectOutputPin.SendValue<bool>( AValue );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
#ifdef _MITOV_DISPLAY_ST7735B_
	class DisplayST7735B : public DisplayST7735
	{
		typedef DisplayST7735 inherited;

	protected:
		static const uint16_t WIDTH = 128;
		static const uint16_t HEIGHT = 160;

	protected:
		virtual void UpdateOrientation() override
		{
			writecommand(ST7735::MADCTL);

			switch( Orientation )
			{
				case goUp:		writedata(ST7735::MADCTL_MX | ST7735::MADCTL_MY | ST7735::MADCTL_BGR); break;
				case goRight: 	writedata(ST7735::MADCTL_MY | ST7735::MADCTL_MV | ST7735::MADCTL_BGR); break;
				case goDown: 	writedata(ST7735::MADCTL_BGR); break;
				case goLeft:	writedata(ST7735::MADCTL_MX | ST7735::MADCTL_MV | ST7735::MADCTL_BGR); break;
			}
		}

	public:
		virtual int16_t width(void) const override
		{
			if( Orientation == goUp || Orientation == goDown )
				return WIDTH;

			return HEIGHT;
		}

		virtual int16_t height(void) const override
		{
			if( Orientation == goUp || Orientation == goDown )
				return HEIGHT;

			return WIDTH;
		}

	protected:
		virtual void SystemInit() override
		{
			commonInit(Bcmd);

			inherited::SystemInit();
		}

	public:
		DisplayST7735B( BasicSPI &ASPI, const unsigned char * AFont ) :
			inherited( ASPI, AFont )
		{
			colstart = 0;
			rowstart = 0;
		}

	};
#endif
//---------------------------------------------------------------------------
#ifdef _MITOV_DISPLAY_ST7735R_GREENTAB_
	class DisplayST7735Green : public DisplayST7735
	{
		typedef DisplayST7735 inherited;

	protected:
		static const uint16_t WIDTH = 128;
		static const uint16_t HEIGHT = 160;

	protected:
		virtual void UpdateOrientation() override
		{
			writecommand(ST7735::MADCTL);

			switch( Orientation )
			{
				case goUp:		writedata(ST7735::MADCTL_MX | ST7735::MADCTL_MY | ST7735::MADCTL_BGR); break;
				case goRight: 	writedata(ST7735::MADCTL_MY | ST7735::MADCTL_MV | ST7735::MADCTL_BGR); break;
				case goDown: 	writedata(ST7735::MADCTL_BGR); break;
				case goLeft:	writedata(ST7735::MADCTL_MX | ST7735::MADCTL_MV | ST7735::MADCTL_BGR); break;
			}
		}

	public:
		virtual int16_t width(void) const override
		{
			if( Orientation == goUp || Orientation == goDown )
				return WIDTH;

			return HEIGHT;
		}

		virtual int16_t height(void) const override
		{
			if( Orientation == goUp || Orientation == goDown )
				return HEIGHT;

			return WIDTH;
		}

	protected:
		virtual void SystemInit() override
		{
			commonInit(Rcmd1);
			commandList(Rcmd2green);
			commandList(Rcmd3);

			inherited::SystemInit();
		}

	public:
		DisplayST7735Green( BasicSPI &ASPI, const unsigned char * AFont ) :
			inherited( ASPI, AFont )
		{
			colstart = 2;
			rowstart = 1;
		}

	};
#endif
//---------------------------------------------------------------------------
#ifdef _MITOV_DISPLAY_ST7735R_GREENTAB_144_
	class DisplayST7735Green144 : public DisplayST7735
	{
		typedef DisplayST7735 inherited;

	protected:
		virtual void UpdateOrientation() override
		{
			writecommand(ST7735::MADCTL);

			switch( Orientation )
			{
				case goUp:		writedata(ST7735::MADCTL_MX | ST7735::MADCTL_MY | ST7735::MADCTL_BGR); break;
				case goRight: 	writedata(ST7735::MADCTL_MY | ST7735::MADCTL_MV | ST7735::MADCTL_BGR); break;
				case goDown: 	writedata(ST7735::MADCTL_BGR); break;
				case goLeft:	writedata(ST7735::MADCTL_MX | ST7735::MADCTL_MV | ST7735::MADCTL_BGR); break;
			}
		}

	public:
		virtual int16_t width(void) const override
		{
			return 128;
		}

		virtual int16_t height(void) const override
		{
			return 128;
		}

	protected:
		virtual void SystemInit() override
		{
			commonInit(Rcmd1);
			commandList(Rcmd2green144);
			commandList(Rcmd3);

			inherited::SystemInit();
		}

	public:
		DisplayST7735Green144( BasicSPI &ASPI, const unsigned char * AFont ) :
			inherited( ASPI, AFont )
		{
			colstart = 2;
			rowstart = 3;
		}

	};
#endif
//---------------------------------------------------------------------------
#ifdef _MITOV_DISPLAY_ST7735R_REDTAB_
	class DisplayST7735Red : public DisplayST7735
	{
		typedef DisplayST7735 inherited;

	protected:
		static const uint16_t WIDTH = 128;
		static const uint16_t HEIGHT = 160;

	protected:
		virtual void UpdateOrientation() override
		{
			writecommand(ST7735::MADCTL);

			switch( Orientation )
			{
				case goUp:		writedata(ST7735::MADCTL_MX | ST7735::MADCTL_MY | ST7735::MADCTL_BGR); break;
				case goRight: 	writedata(ST7735::MADCTL_MY | ST7735::MADCTL_MV | ST7735::MADCTL_BGR); break;
				case goDown: 	writedata(ST7735::MADCTL_BGR); break;
				case goLeft:	writedata(ST7735::MADCTL_MX | ST7735::MADCTL_MV | ST7735::MADCTL_BGR); break;
			}
		}

	public:
		virtual int16_t width(void) const override
		{
			if( Orientation == goUp || Orientation == goDown )
				return WIDTH;

			return HEIGHT;
		}

		virtual int16_t height(void) const override
		{
			if( Orientation == goUp || Orientation == goDown )
				return HEIGHT;

			return WIDTH;
		}

	protected:
		virtual void SystemInit() override
		{
			commonInit(Rcmd1);
			commandList(Rcmd2red);
			commandList(Rcmd3);

			inherited::SystemInit();
		}

	public:
		DisplayST7735Red( BasicSPI &ASPI, const unsigned char * AFont ) :
			inherited( ASPI, AFont )
		{
			colstart = 0;
			rowstart = 0;
		}

	};
#endif
//---------------------------------------------------------------------------
#ifdef _MITOV_DISPLAY_ST7735R_BLACKTAB_
	class DisplayST7735Black : public DisplayST7735
	{
		typedef DisplayST7735 inherited;

	protected:
		static const uint16_t WIDTH = 128;
		static const uint16_t HEIGHT = 160;

	public:
		virtual void UpdateOrientation() override
		{
			writecommand(ST7735::MADCTL);

			switch( Orientation )
			{
				case goUp:		writedata(ST7735::MADCTL_MX | ST7735::MADCTL_MY | ST7735::MADCTL_RGB); break;
				case goRight: 	writedata(ST7735::MADCTL_MY | ST7735::MADCTL_MV | ST7735::MADCTL_RGB); break;
				case goDown: 	writedata(ST7735::MADCTL_RGB); break;
				case goLeft:	writedata(ST7735::MADCTL_MX | ST7735::MADCTL_MV | ST7735::MADCTL_RGB); break;
			}
		}

	public:
		virtual int16_t width(void) const override
		{
			if( Orientation == goUp || Orientation == goDown )
				return WIDTH;

			return HEIGHT;
		}

		virtual int16_t height(void) const override
		{
			if( Orientation == goUp || Orientation == goDown )
				return HEIGHT;

			return WIDTH;
		}

	protected:
		virtual void SystemInit() override
		{
			commonInit(Rcmd1);
			commandList(Rcmd2red);
			commandList(Rcmd3);
			writecommand(ST7735::MADCTL);
			writedata(0xC0);

/*

writecommand(ST7735::MADCTL);
writedata(MADCTL_MY | MADCTL_MV | MADCTL_RGB);
*/
			inherited::SystemInit();

		}

	public:
		DisplayST7735Black( BasicSPI &ASPI, const unsigned char * AFont ) :
			inherited( ASPI, AFont )
		{
			colstart = 0;
			rowstart = 0;
		}

	};
#endif
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
