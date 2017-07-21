////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_GRAPHICS_h
#define _MITOV_GRAPHICS_h

#include <Mitov.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	class Graphics;
	class GraphicsElementBasic;
	class GraphicsTextElementBasic;
//---------------------------------------------------------------------------
#ifdef __32BIT_GRAPHICS_COORDINATES__
	typedef int32_t		TGraphicsPos;
	typedef uint32_t	TGraphicsSize;
#else
	typedef int16_t		TGraphicsPos;
	typedef uint16_t	TGraphicsSize;
#endif
//---------------------------------------------------------------------------
	enum TArduinoMonochromeColor { tmcBlack, tmcWhite, tmcInvert, tmcNone };
//---------------------------------------------------------------------------
	typedef uint64_t TUniColor;
//---------------------------------------------------------------------------
	struct GraphicsPoint
	{
		TGraphicsPos	X;
		TGraphicsPos	Y;
	};
//---------------------------------------------------------------------------
	class GraphicsIntf
	{
	protected:
		Mitov::SimpleList<GraphicsElementBasic *>	FElements;

	public:
		virtual void GetPosition( TGraphicsPos &AX, TGraphicsPos &AY ) { AX = 0; AY = 0; }
		virtual void RegisterRender( GraphicsElementBasic *AItem ) 
		{
			FElements.push_back( AItem );
		}

		virtual Graphics *GetGraphics() = 0;
	};
//---------------------------------------------------------------------------
	class GraphicsElementBasic : public OpenWire::Object
	{
	protected:
		GraphicsIntf	&FOwner;

	public:
		virtual void Render( bool AForce ) = 0;

	public:
		GraphicsElementBasic( GraphicsIntf &AOwner ) :
			FOwner( AOwner )
		{
			FOwner.RegisterRender( this );
		}
	};
//---------------------------------------------------------------------------
	class GraphicsTextIntf
	{
	protected:
		Mitov::SimpleList<GraphicsTextElementBasic *>	FElements;

	public:
		virtual void RegisterRender( GraphicsTextElementBasic *AItem ) 
		{
			FElements.push_back( AItem );
		}
	};
//---------------------------------------------------------------------------
	class GraphicsTextElementBasic
	{
	public:
		virtual void Enter( Graphics *AGraphics ) {}
		virtual void Leave( Graphics *AGraphics ) {}

	public:
		GraphicsTextElementBasic( GraphicsTextIntf &AOwner )
		{
			AOwner.RegisterRender( this );
		}
	};
//---------------------------------------------------------------------------
	class GraphicsElementClocked : public GraphicsElementBasic, public Mitov::ClockingSupport
	{
		typedef GraphicsElementBasic inherited;

	public:
		virtual void DoClockReceive( void *_Data ) override
		{
			Render( true );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class TArduinoGraphicsCornerSize
	{
	public:
		int16_t		X = 10;
		int16_t		Y = 10;

	};
//---------------------------------------------------------------------------
	enum TArduinoTextHorizontalAlign { thaLeft, thaCenter, thaRight };
//---------------------------------------------------------------------------
	enum TArduinoTextVerticalAlign { tvaTop, tvaCenter, tvaBottom };
//---------------------------------------------------------------------------
	enum GraphicsOrientation { goUp, goRight, goDown, goLeft };
//---------------------------------------------------------------------------
	class Graphics : public Print
	{
		typedef Print inherited;

	protected:
		int16_t	cursor_x = 0;
		int16_t	cursor_y = 0;

	protected:
		const unsigned char * FFont;

	protected:
		TUniColor	textcolor, textbgcolor;

		uint8_t	textsize = 1;

		bool wrap : 1;   // If set, 'wrap' text at right edge of display
		bool _cp437 : 1; // If set, use correct CP437 charset (default is off)

	public:
		void PrintChar( char AValue )
		{
			write( AValue );
		}

		void PrintChar( byte AValue )
		{
			write( AValue );
		}

	public:
		template<typename T> void Print( T AValue )
		{
			println( AValue );
		}

	public:
		virtual void Scroll( TGraphicsPos X, TGraphicsPos Y, TUniColor color )
		{
			if( X == 0 && Y == 0 )
				return;

			int AFromYIndex, AToYIndex;
			int AHeight = height() - abs( Y );
			if( Y < 0 )
			{
				AFromYIndex = -Y;
				AToYIndex = 0;
			}

			else
			{
				AFromYIndex = 0;
				AToYIndex = Y;
			}

			int AFromXIndex, AToXIndex;
			int AWidth = width() - abs( X );
			if( X < 0 )
			{
				AFromXIndex = -X;
				AToXIndex = 0;
			}

			else
			{
				AFromXIndex = 0;
				AToXIndex = X;
			}

			if( Y < 0 )
			{
				for( int ay = 0; ay < AHeight; ++ay )
				{
					if( X < 0 )
					{
						for( int ax = 0; ax < AWidth; ++ax )
							drawPixel( ax + AToXIndex, ay + AToYIndex, GetPixelColor( ax + AFromXIndex, ay + AFromYIndex ));

						drawFastHLine( AWidth, ay + AToYIndex, width() - AWidth, color );
//						for( int ax = AWidth; ax < width(); ++ax )
//							drawPixel( ax, ay + AToYIndex, color );
					}

					else
					{
						for( int ax = AWidth; ax--; )
							drawPixel( ax + AToXIndex, ay + AToYIndex, GetPixelColor( ax + AFromXIndex, ay + AFromYIndex ));

						drawFastHLine( 0, ay + AToYIndex, X, color );
//						for( int ax = 0; ax < X; ++ax )
//							drawPixel( ax, ay + AToYIndex, color );
					}
				}

				fillRect( 0, AHeight, width(), height(), color );
//				for( int ay = AHeight; ay < height(); ++ay )
//					for( int ax = 0; ax < width(); ++ax )
//						drawPixel( ax, ay, color );
			}

			else
			{
				for( int ay = AHeight; ay--; )
				{
					if( X < 0 )
					{
						for( int ax = 0; ax < AWidth; ++ax )
							drawPixel( ax + AToXIndex, ay + AToYIndex, GetPixelColor( ax + AFromXIndex, ay + AFromYIndex ));

						drawFastHLine( AWidth, ay + AToYIndex, width() - AWidth, color );
//						for( int ax = AWidth; ax < width(); ++ax )
//							drawPixel( ax, ay + AToYIndex, color );
					}

					else
					{
						for( int ax = AWidth; ax--; )
							drawPixel( ax + AToXIndex, ay + AToYIndex, GetPixelColor( ax + AFromXIndex, ay + AFromYIndex ));

						drawFastHLine( 0, ay + AToYIndex, X, color );
//						for( int ax = 0; ax < X; ++ax )
//							drawPixel( ax, ay + AToYIndex, color );
					}
				}

				fillRect( 0, 0, width(), Y, color );
//				for( int ay = 0; ay < Y; ++ay )
//					for( int ax = 0; ax < width(); ++ax )
//						drawPixel( ax, ay, color );
			}
		}

		virtual void drawGrayscalePixel(int16_t x, int16_t y, TUniColor color)
		{
			drawPixel( x, y, color );
		}

		virtual void drawColorPixel(int16_t x, int16_t y, TUniColor color)
		{
			drawPixel( x, y, color );
		}

	public:
		// This MUST be defined by the subclass:
		virtual void drawPixel(int16_t x, int16_t y, TUniColor color) = 0;
		virtual TUniColor	GetBackgroundColor() = 0;

	public:
		void setFont( const unsigned char *AFont )
		{
			FFont = AFont;
		}

		const unsigned char *getFont()
		{
			return FFont;
		}

		void getTextColor( TUniColor &AColor, TUniColor &ABackgroundColor )
		{
			AColor = textcolor;
			ABackgroundColor = textbgcolor; 
		}

		void drawEllipse( int16_t xc, int16_t yc, int16_t rx, int16_t ry, TUniColor color, bool ACorrectX, bool ACorrectY ) 
		{
			if( ACorrectY )
			{
				++yc;
				++ry;
			}

			uint32_t hh = ry * ry;
			uint32_t ww = rx * rx;
			uint32_t hhww = hh * ww;
			TGraphicsPos x0 = rx;
			TGraphicsPos dx = 0;
			TGraphicsPos ALastX = rx;

			if( ! ACorrectY )
			{
				drawPixel( xc - rx, yc, color );
				if( ACorrectX )
					drawPixel( xc + rx + 1, yc, color );

				else
					drawPixel( xc + rx, yc, color );

			}

			for (int y = 1; y <= ry; y++)
			{
				uint32_t x1 = x0 - (dx - 1);  // try slopes of dx - 1 or more

				for ( ; x1 > 0; x1--)
					if (x1*x1*hh + y*y*ww <= hhww)
						break;

				dx = x0 - x1;  // current approximation of the slope
				x0 = x1;

				int ay1 = ( ACorrectY ) ? y - 1 : y;
				int aX0R = ( ACorrectX ) ? x0 - 1 : x0;

				if( ALastX - x0 )
				{
					drawFastHLine( xc - ALastX, yc - y, ALastX - x0, color );
					drawFastHLine( xc - ALastX, yc + ay1, ALastX - x0, color );

					drawFastHLine( xc + x0, yc - y, ALastX - x0, color );
					drawFastHLine( xc + x0, yc + ay1, ALastX - x0, color );
				}

				else
				{
					drawPixel( xc - x0, yc - y, color );
					drawPixel( xc - x0, yc + ay1, color );

					drawPixel( xc + aX0R, yc - y, color );
					drawPixel( xc + aX0R, yc + ay1, color );
				}

				ALastX = x0;
			}
		}

		void drawEllipseWH( int16_t x, int16_t y, int16_t w, int16_t h, TUniColor color ) 
		{
			drawEllipse( x + w / 2, y + h / 2, w / 2, h / 2, color, !( w & 1 ), !( h & 1 ) );
		}

		void drawEllipseHelper( int16_t xc, int16_t yc, int16_t rx, int16_t ry, uint8_t ACorners, TUniColor color ) //, bool ACorrectX = false, bool ACorrectY = false ) 
		{
//			if( ACorrectY )
//				++ry;

			uint32_t hh = ry * ry;
			uint32_t ww = rx * rx;
			uint32_t hhww = hh * ww;
			TGraphicsPos x0 = rx;
			TGraphicsPos dx = 0;
			TGraphicsPos ALastX = rx;

/*
			if( ! ACorrectY )
			{
				drawPixel( xc - rx, yc, color );
				if( ACorrectX )
					drawPixel( xc + rx + 1, yc, color );

				else
					drawPixel( xc + rx, yc, color );

			}
*/
			for (int y = 1; y <= ry; y++)
			{
				uint32_t x1 = x0 - (dx - 1);  // try slopes of dx - 1 or more

				for ( ; x1 > 0; x1--)
					if (x1*x1*hh + y*y*ww <= hhww)
						break;

				dx = x0 - x1;  // current approximation of the slope
				x0 = x1;

//				int ay1 = ( ACorrectY ) ? y - 1 : y;
//				int aX0R = ( ACorrectX ) ? x0 - 1 : x0;

				if( ALastX - x0 )
				{
					if( ACorners & 1 )
						drawFastHLine( xc - ALastX, yc - y, ALastX - x0, color );

					if( ACorners & 8 )
						drawFastHLine( xc - ALastX, yc + y, ALastX - x0, color );

					if( ACorners & 2 )
						drawFastHLine( xc + x0, yc - y, ALastX - x0, color );

					if( ACorners & 4 )
						drawFastHLine( xc + x0, yc + y, ALastX - x0, color );
				}

				else
				{
					if( ACorners & 1 )
						drawPixel( xc - x0, yc - y, color );

					if( ACorners & 8 )
						drawPixel( xc - x0, yc + y, color );

					if( ACorners & 2 )
						drawPixel( xc + x0, yc - y, color );

					if( ACorners & 4 )
						drawPixel( xc + x0, yc + y, color );
				}

				ALastX = x0;
			}

/*
			//Region 1
			int16_t p = ry*ry-rx*rx*ry+rx*rx/4;
			int16_t x=0;
			int16_t y=ry;

//			drawPixel( 1, 0, tmcWhite );

			while(2.0*ry*ry*x <= 2.0*rx*rx*y)
			{
				if(p < 0)
				{
					x++;
					p = p+2*ry*ry*x+ry*ry;
				}

				else
				{
					x++;y--;
					p = p+2*ry*ry*x-2*rx*rx*y-ry*ry;
				}

				if( ACorners & 4 )
				{
					int16_t ax = ( ACorrectX ) ? xc+x - 1 : xc+x;
					int16_t ay = ( ACorrectY ) ? yc+y - 1 : yc+y;
					drawPixel( ax, ay,color);
				}

				if( ACorners & 2 )
					if( y != 0 || ACorners != 0xF )
					{
						if( ACorrectX )
							drawPixel(xc+x - 1,yc-y,color);

						else
							drawPixel(xc+x,yc-y,color);
					}

				if( ACorners & 8 )
					if( ACorrectY )
						drawPixel(xc-x,yc+y - 1 ,color);

					else
						drawPixel(xc-x,yc+y,color);

				if( ACorners & 1 )
					if( y != 0 || ACorners != 0xF )
						drawPixel(xc-x,yc-y,color);

//				return;
			}

			//Region 2
			p = ry*ry*(x+0.5)*(x+0.5)+rx*rx*(y-1)*(y-1)-rx*rx*ry*ry;
			while(y > 0)
			{
				if(p <= 0)
				{
					x++;
					y--;
					p = p+2*ry*ry*x-2*rx*rx*y+rx*rx;
				}

				else
				{
					y--;
					p = p-2*rx*rx*y+rx*rx;
				}

				if( ACorners & 4 )
				{
					if( ! ACorrectY || ( y > 0 ) )
					{
						int16_t ax = ( ACorrectX ) ? xc+x - 1 : xc+x;
						int16_t ay = ( ACorrectY ) ? yc+y - 1 : yc+y;
						drawPixel( ax,ay,color);
					}
				}

				if( ACorners & 2 )
					if( y != 0 || ACorners != 0xF )
					{
						if( ACorrectX )
							drawPixel(xc+x - 1,yc-y,color);

						else
							drawPixel(xc+x,yc-y,color);
					}

				if( ACorners & 8 )
				{
					if( ACorrectY )
					{
						if( y > 0 )
							drawPixel(xc-x,yc+y - 1,color);
					}

					else
						drawPixel(xc-x,yc+y,color);
				}

				if( ACorners & 1 )
					if( y != 0 || ACorners != 0xF )
						drawPixel(xc-x,yc-y,color);
			}
*/
		}

		void fillEllipse( int16_t ax, int16_t ay, int16_t rx, int16_t ry, TUniColor color, bool ACorrectX, bool ACorrectY ) 
		{
//			Serial.print( "fillEllipse: " ); Serial.print( ax ); Serial.print( "x" ); Serial.println( ay );
			if( ACorrectY )
			{
				++ay;
				++ry;
			}

			uint32_t hh = ry * ry;
			uint32_t ww = rx * rx;
			uint32_t hhww = hh * ww;
			TGraphicsPos x0 = rx;
			TGraphicsPos dx = 0;

			if( ! ACorrectY )
			{
				if( ACorrectX )
					drawFastHLine( ax - rx, ay, rx * 2 + 1, color );

				else
					drawFastHLine( ax - rx, ay, rx * 2, color );

			}

			for (int y = 1; y <= ry; y++)
			{
				uint32_t x1 = x0 - (dx - 1);  // try slopes of dx - 1 or more

				for ( ; x1 > 0; x1--)
					if (x1*x1*hh + y*y*ww <= hhww)
						break;

				dx = x0 - x1;  // current approximation of the slope
				x0 = x1;

				int ay1 = ( ACorrectY ) ? y - 1 : y;
				int aWidth = ( ACorrectX ) ? x0 * 2 + 1 : x0 * 2;

				drawFastHLine( ax - x0, ay - y, aWidth, color );
				drawFastHLine( ax - x0, ay + ay1, aWidth, color );

			}
		}

		void fillEllipseWH( int16_t x, int16_t y, int16_t w, int16_t h, TUniColor color ) 
		{
			fillEllipse( x + w / 2, y + h / 2, w / 2, h / 2, color, !( w & 1 ), !( h & 1 ) );
//			fillEllipseHelper( x + w / 2, y + h / 2, w / 2, h / 2, 0xF, color, !( w & 1 ), !( h & 1 ) );
		}

		void fillEllipseHelper( int16_t xc, int16_t yc, int16_t rx, int16_t ry, uint8_t ACorners, TUniColor color ) 
		{
//			if( ACorrectY )
//				++ry;

			uint32_t hh = ry * ry;
			uint32_t ww = rx * rx;
			uint32_t hhww = hh * ww;
			TGraphicsPos x0 = rx;
			TGraphicsPos dx = 0;

/*
			if( ! ACorrectY )
			{
				if( ACorrectX )
					drawFastHLine( ax - rx, ay, rx * 2 + 1, color );

				else
					drawFastHLine( ax - rx, ay, rx * 2, color );

			}
*/
			for (int y = 1; y <= ry; y++)
			{
				uint32_t x1 = x0 - (dx - 1);  // try slopes of dx - 1 or more

				for ( ; x1 > 0; x1--)
					if (x1*x1*hh + y*y*ww <= hhww)
						break;

				dx = x0 - x1;  // current approximation of the slope
				x0 = x1;

//				int ay1 = ( ACorrectY ) ? y - 1 : y;
//				int aWidth = ( ACorrectX ) ? x0 * 2 + 1 : x0 * 2;

				if( ACorners & 1 )
					drawFastHLine( xc - x0, yc - y, x0, color );

				if( ACorners & 8 )
					drawFastHLine( xc - x0, yc + y, x0, color );

				if( ACorners & 2 )
					drawFastHLine( xc, yc - y, x0, color );

				if( ACorners & 4 )
					drawFastHLine( xc, yc + y, x0, color );

//				drawFastHLine( ax - x0, ay - y, aWidth, color );
//				drawFastHLine( ax - x0, ay + ay1, aWidth, color );

			}

/*
			//Region 1
			int16_t p = ry*ry-rx*rx*ry+rx*rx/4;
			int16_t x=0;
			int16_t y=ry;

			int16_t ALastY = -10;

			while(2.0*ry*ry*x <= 2.0*rx*rx*y)
			{
				if(p < 0)
				{
					x++;
					p = p+2*ry*ry*x+ry*ry;
				}

				else
				{
					x++;y--;
					p = p+2*ry*ry*x-2*rx*rx*y-ry*ry;
				}

				if( y != ALastY )
				{
					ALastY = y;
					if( ACorners == 0xF )
					{
						int16_t ax = ( ACorrectX ) ? 2 * x : 2 * x + 1;
						if( ACorrectY )
						{
							if( y > 0 )
								drawFastHLine( xc-x, yc+y - 1, ax, color );
						}

						else
							drawFastHLine( xc-x, yc+y, ax, color );

						if( y != 0 )
							drawFastHLine( xc-x, yc-y, ax, color );
					}

					else
					{
						if( ACorners & 8 )
						{
							if( ACorrectY )
							{
								if( y > 0 )
									drawFastHLine( xc-x, yc+y - 1, x, color );
							}

							else
								drawFastHLine( xc-x, yc+y, x, color );
						}

						if( ACorners & 4 )
						{
							if( ACorrectY )
							{
								if( y > 0 )
									drawFastHLine( xc, yc+y - 1, x, color );
							}

							else
								drawFastHLine( xc, yc+y, x, color );
						}

						if( ACorners & 1 )
							drawFastHLine( xc-x, yc-y, x, color );

						if( ACorners & 2 )
							drawFastHLine( xc, yc-y, x, color );
					}
				}
			}


			//Region 2
			p = ry*ry*(x+0.5)*(x+0.5)+rx*rx*(y-1)*(y-1)-rx*rx*ry*ry;
			while(y > 0)
			{
				if(p <= 0)
				{
					x++;
					y--;
					p = p+2*ry*ry*x-2*rx*rx*y+rx*rx;
				}

				else
				{
					y--;
					p = p-2*rx*rx*y+rx*rx;
				}

				if( y != ALastY )
				{
					ALastY = y;
					if( ACorners == 0xF )
					{
						int16_t ax = ( ACorrectX ) ? 2 * x : 2 * x + 1;
						if( ACorrectY )
						{
							if( y > 0 )
								drawFastHLine( xc-x,yc+y - 1, ax, color );
						}

						else
							drawFastHLine( xc-x,yc+y, ax, color );

						if( y != 0 )
							drawFastHLine( xc-x,yc-y, ax, color );
					}

					else
					{
						if( ACorners & 8 )
							if( ACorrectY )
							{
								if( y > 0 )
									drawFastHLine( xc-x,yc+y - 1, x, color );
							}

							else
								drawFastHLine( xc-x,yc+y, x, color );

						if( ACorners & 4 )
							if( ACorrectY )
							{
								if( y > 0 )
									drawFastHLine( xc,yc+y - 1, x, color );
							}

							else
								drawFastHLine( xc,yc+y, x, color );

						if( ACorners & 1 )
							drawFastHLine( xc-x,yc-y, x, color );

						if( ACorners & 2 )
							drawFastHLine( xc,yc-y, x, color );
					}
				}
			}
*/
		}

		virtual void ClearScreen( TUniColor color )
		{
			fillRect( 0, 0, width(), height(), color );
		}

		virtual TUniColor GetPixelColor( int16_t x, int16_t y ) = 0;

		bool getTextWrap()
		{
			return wrap;
		}

		virtual void drawRect( int16_t x, int16_t y, int16_t w, int16_t h, TUniColor color ) 
		{
			drawFastHLine( x, y, w, color);
			drawFastHLine( x, y+h-1, w, color);
			drawFastVLine( x, y, h, color);
			drawFastVLine( x+w-1, y, h, color);
		}

		virtual void fillRect( int16_t x, int16_t y, int16_t w, int16_t h, TUniColor color) 
		{
			// Update in subclasses if desired!
			for (int16_t i=x; i<x+w; i++)
				drawFastVLine( i, y, h, color );

		}

		virtual void fillScreen( TUniColor color ) 
		{
			fillRect( 0, 0, width(), height(), color);
		}

#ifdef _GRAPHICS_DRAW_GRAYSCALE_BITMAP_

		virtual void drawGrayscaleBitmap( int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint8_t AAlpha ) 
		{
			TUniColor AAlpha16 = uint16_t( AAlpha ) << 8;
  
			for( int16_t j=0; j<h; j++)
				for( int16_t i=0; i<w; ++i )
					drawGrayscalePixel( x+i, y+j, AAlpha16 | pgm_read_byte( bitmap ++ ));

		}

#endif // _GRAPHICS_DRAW_GRAYSCALE_BITMAP_

#ifdef _GRAPHICS_DRAW_GRAYSCALE_REPEAT_BITMAP_

		virtual void drawRepeatGrayscaleBitmap( int16_t x, int16_t y, const uint8_t *bitmap, int16_t AListCount, int16_t w, int16_t h, uint8_t AAlpha ) 
		{
			TUniColor AAlpha16 = uint16_t( AAlpha ) << 8;
  
			int16_t x1 = 0;
			for( int16_t i = 0; i < AListCount; ++i )
			{
				uint8_t ACount = pgm_read_byte( bitmap ++ );
				uint32_t AGrayscale = pgm_read_byte( bitmap ++ );

				uint32_t AColor = AAlpha16 | AGrayscale;

				for( int16_t j = 0; j <= ACount; ++ j )
				{
					drawGrayscalePixel( x + x1++, y, AColor );
					if( x1 >= w )
					{
						x1 = 0;
						++ y;
					}					
				}
			}
		}

#endif // _GRAPHICS_DRAW_GRAYSCALE_REPEAT_BITMAP_

#ifdef _GRAPHICS_DRAW_GRAYSCALE_REPEAT_ALPHA_BITMAP_

		virtual void drawAlphaGrayscaleRepeatBitmap( int16_t x, int16_t y, const uint8_t *bitmap, int16_t AListCount, int16_t w, int16_t h ) 
		{
			int16_t x1 = 0;
			for( int16_t i = 0; i < AListCount; ++i )
			{
				uint8_t ACount = pgm_read_byte( bitmap ++ );
				uint16_t AGrayscale = pgm_read_byte( bitmap ++ );
				uint16_t AColor = ( AGrayscale << 8 ) | pgm_read_byte( bitmap ++ );

				for( int16_t j = 0; j <= ACount; ++ j )
				{
					drawGrayscalePixel( x + x1++, y, AColor );
					if( x1 >= w )
					{
						x1 = 0;
						++ y;
					}					
				}
			}
		}

#endif // _GRAPHICS_DRAW_GRAYSCALE_REPEAT_ALPHA_BITMAP_

#ifdef _GRAPHICS_DRAW_ALPHA_COLOR_BITMAP_

		virtual void drawAlphaColorBitmap( int16_t x, int16_t y, const uint32_t *bitmap, int16_t w, int16_t h ) 
		{
			for( int16_t j=0; j<h; j++)
				for( int16_t i=0; i<w; ++i )
					drawPixel( x+i, y+j, pgm_read_dword( bitmap ++ ));

		}

#endif // _GRAPHICS_DRAW_ALPHA_COLOR_BITMAP_

#ifdef _GRAPHICS_DRAW_ALPHA_RGBW_COLOR_BITMAP_

		virtual void drawAlphaRGBWColorBitmap( int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h ) 
		{
			for( int16_t j=0; j<h; j++)
				for( int16_t i=0; i<w; ++i )
				{
					uint32_t ARed = pgm_read_byte( bitmap ++ );
					uint16_t AGreen = pgm_read_byte( bitmap ++ );
					uint8_t ABlue = pgm_read_byte( bitmap ++ );
					uint32_t AWhite = pgm_read_byte( bitmap ++ );
					uint64_t AAlpha = pgm_read_byte( bitmap ++ );
					uint64_t AColor = ( AAlpha << 32 ) | (AWhite << 24) | ( ARed << 16 ) | ( AGreen << 8 ) | ABlue;

					drawPixel( x+i, y+j, AColor );
				}

		}

#endif // _GRAPHICS_DRAW_ALPHA_COLOR_BITMAP_

#ifdef _GRAPHICS_DRAW_COLOR_BITMAP_

		virtual void drawColorBitmap( int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h ) 
		{
			for( int16_t j=0; j<h; j++)
				for( int16_t i=0; i<w; ++i )
				{
					uint32_t ARed = pgm_read_byte( bitmap ++ );
					uint32_t AGreen = pgm_read_byte( bitmap ++ );
					uint32_t ABlue = pgm_read_byte( bitmap ++ );
					uint32_t AColor = 0xFF000000 | ( ARed << 16 ) | ( AGreen << 8 ) | ABlue;

					drawColorPixel( x+i, y+j, AColor );
				}

		}

#endif // _GRAPHICS_DRAW_COLOR_BITMAP_

#ifdef _GRAPHICS_DRAW_RGBW_COLOR_BITMAP_

		virtual void drawRGBWColorBitmap( int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h ) 
		{
			for( int16_t j=0; j<h; j++)
				for( int16_t i=0; i<w; ++i )
				{
					uint32_t ARed = pgm_read_byte( bitmap ++ );
					uint16_t AGreen = pgm_read_byte( bitmap ++ );
					uint8_t ABlue = pgm_read_byte( bitmap ++ );
					uint64_t AWhite = pgm_read_byte( bitmap ++ );
					uint64_t AColor = 0xFF00000000 | ( AWhite << 32 ) | ( ARed << 16 ) | ( AGreen << 8 ) | ABlue;

					drawPixel( x+i, y+j, AColor );
				}

		}

#endif // _GRAPHICS_DRAW_RGBW_COLOR_BITMAP_

#ifdef _GRAPHICS_DRAW_REPEAT_COLOR_BITMAP_

		virtual void drawRepeatColorBitmap( int16_t x, int16_t y, const uint8_t *bitmap, int16_t AListCount, int16_t w, int16_t h ) 
		{
			int16_t x1 = 0;
			for( int16_t i = 0; i < AListCount; ++i )
			{
				uint8_t ACount = pgm_read_byte( bitmap ++ );

				uint32_t ARed = pgm_read_byte( bitmap ++ );
				uint32_t AGreen = pgm_read_byte( bitmap ++ );
				uint32_t ABlue = pgm_read_byte( bitmap ++ );
				uint32_t AColor = 0xFF000000 | ( ARed << 16 ) | ( AGreen << 8 ) | ABlue;

				for( int16_t j = 0; j <= ACount; ++ j )
				{
					drawColorPixel( x + x1++, y, AColor );
					if( x1 >= w )
					{
						x1 = 0;
						++ y;
					}					
				}
			}
		}

#endif // _GRAPHICS_DRAW_REPEAT_COLOR_BITMAP_

#ifdef _GRAPHICS_DRAW_REPEAT_RGBW_COLOR_BITMAP_

		virtual void drawRepeatRGBWColorBitmap( int16_t x, int16_t y, const uint8_t *bitmap, int16_t AListCount, int16_t w, int16_t h ) 
		{
			int16_t x1 = 0;
			for( int16_t i = 0; i < AListCount; ++i )
			{
				uint8_t ACount = pgm_read_byte( bitmap ++ );

				uint32_t ARed = pgm_read_byte( bitmap ++ );
				uint16_t AGreen = pgm_read_byte( bitmap ++ );
				uint8_t ABlue = pgm_read_byte( bitmap ++ );
				uint64_t AWhite = pgm_read_byte( bitmap ++ );
				uint64_t AColor = 0xFF00000000 | ( AWhite << 24 ) | ( ARed << 16 ) | ( AGreen << 8 ) | ABlue;

				for( int16_t j = 0; j <= ACount; ++ j )
				{
					drawPixel( x + x1++, y, AColor );
					if( x1 >= w )
					{
						x1 = 0;
						++ y;
					}					
				}
			}
		}

#endif // _GRAPHICS_DRAW_REPEAT_RGBW_COLOR_BITMAP_

#ifdef _GRAPHICS_DRAW_REPEAT_ALPHA_COLOR_BITMAP_

		virtual void drawRepeatAlphaColorBitmap( int16_t x, int16_t y, const uint8_t *bitmap, int16_t AListCount, int16_t w, int16_t h ) 
		{
			int16_t x1 = 0;
			for( int16_t i = 0; i < AListCount; ++i )
			{
				uint8_t ACount = pgm_read_byte( bitmap ++ );
				uint32_t AColor = pgm_read_dword( bitmap );
				bitmap += 4;
				for( int16_t j = 0; j <= ACount; ++ j )
				{
					drawColorPixel( x + x1++, y, AColor );
					if( x1 >= w )
					{
						x1 = 0;
						++ y;
					}					
				}
			}
		}

#endif // _GRAPHICS_DRAW_REPEAT_ALPHA_COLOR_BITMAP_

#ifdef _GRAPHICS_DRAW_REPEAT_ALPHA_RGBW_COLOR_BITMAP_

		virtual void drawRepeatAlphaRGBWColorBitmap( int16_t x, int16_t y, const uint8_t *bitmap, int16_t AListCount, int16_t w, int16_t h ) 
		{
			int16_t x1 = 0;
			for( int16_t i = 0; i < AListCount; ++i )
			{
				uint8_t ACount = pgm_read_byte( bitmap ++ );

				uint32_t ARed = pgm_read_byte( bitmap ++ );
				uint16_t AGreen = pgm_read_byte( bitmap ++ );
				uint8_t ABlue = pgm_read_byte( bitmap ++ );
				uint32_t AWhite = pgm_read_byte( bitmap ++ );
				uint64_t AAlpha = pgm_read_byte( bitmap ++ );

				uint64_t AColor = ( AAlpha << 32 ) | (AWhite << 24) | ( ARed << 16 ) | ( AGreen << 8 ) | ABlue;
				for( int16_t j = 0; j <= ACount; ++ j )
				{
					drawPixel( x + x1++, y, AColor );
					if( x1 >= w )
					{
						x1 = 0;
						++ y;
					}					
				}
			}
		}

#endif // _GRAPHICS_DRAW_REPEAT_ALPHA_RGBW_COLOR_BITMAP_

#ifdef _GRAPHICS_DRAW_GRAYSCALE_ALPHA_BITMAP_

		virtual void drawAlphaGrayscaleBitmap( int16_t x, int16_t y, const uint16_t *bitmap, int16_t w, int16_t h ) 
		{
			for( int16_t j=0; j<h; j++)
				for( int16_t i=0; i<w; ++i )
					drawGrayscalePixel( x+i, y+j, pgm_read_word( bitmap ++ ));

		}

#endif // _GRAPHICS_DRAW_GRAYSCALE_ALPHA_BITMAP_

#ifdef _GRAPHICS_DRAW_BLACK_WHITE_BITMAP_

		// Draw a 1-bit color bitmap at the specified x, y position from the
		// provided bitmap buffer (must be PROGMEM memory) using color as the
		// foreground color and bg as the background color.
		virtual void drawBWBitmap( int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, TUniColor color, TUniColor bg ) 
		{
			int16_t byteWidth = (w + 7) / 8;
  
			for( int16_t j=0; j<h; ++j ) 
				for( int16_t i=0; i<w; ++i ) 
				{
					if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7)))
						drawGrayscalePixel(x+i, y+j, color);

					else 
      					drawGrayscalePixel(x+i, y+j, bg);
				}

		}

#endif // _GRAPHICS_DRAW_BLACK_WHITE_BITMAP_

		void drawTransparencyBitmap( int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h )
		{
			int16_t byteWidth = (w + 3) / 4;
  
			for( int16_t j=0; j<h; ++j ) 
				for( int16_t i=0; i<w; ++i ) 
				{
					uint8_t AByte = ( pgm_read_byte(bitmap + j * byteWidth + i / 4) >> ((3 - (i & 0b11)) * 2 )) & 0b11;

					switch( AByte )
					{
						case 1 : drawPixel( x+i, y+j, tmcInvert ); break;
						case 2 : drawPixel( x+i, y+j, tmcBlack ); break;
						case 3 : drawPixel( x+i, y+j, tmcWhite ); break;
					}
				}

		}

		void drawRoundRect( int16_t x, int16_t y, int16_t w, int16_t h, int16_t rx, int16_t ry, TUniColor color )
		{
			if( rx == 0 || ry == 0 )
			{
				drawRect( x, y, w, h, color );
				return;
			}

			drawFastHLine( x+rx , y    , w-2 * rx - 1, color ); // Top
			drawFastHLine( x+rx , y+h-1, w-2 * rx - 1, color ); // Bottom
			drawFastVLine( x    , y+ry , h-2 * ry, color ); // Left
			drawFastVLine( x+w-1, y+ry , h-2 * ry, color ); // Right

			drawEllipseHelper( x+rx,		y+ry,	  rx, ry, 1, color);
			drawEllipseHelper( x+w-rx-1,	y+ry,	  rx, ry, 2, color);
			drawEllipseHelper( x+w-rx-1,	y+h-ry-1, rx, ry, 4, color);
			drawEllipseHelper( x+rx,		y+h-ry-1, rx, ry, 8, color);
		}

		void fillRoundRect( int16_t x, int16_t y, int16_t w, int16_t h, int16_t rx, int16_t ry, TUniColor color )
		{
			if( rx == 0 || ry == 0 )
			{
				fillRect( x, y, w, h, color );
				return;
			}

			fillRect( x+rx, y, w-rx*2 - 1, ry, color );
			fillRect( x+rx, y+h-ry, w-rx*2 - 1, ry, color );
			fillRect( x, y+ry, w, h-ry * 2, color );
			fillEllipseHelper( x+rx,		y+ry,	  rx, ry, 1, color);
			fillEllipseHelper( x+w-rx-1,	y+ry,	  rx, ry, 2, color);
			fillEllipseHelper( x+w-rx-1,	y+h-ry-1, rx, ry, 4, color);
			fillEllipseHelper( x+rx,		y+h-ry-1, rx, ry, 8, color);
		}

		// Bresenham's algorithm - thx wikpedia
		virtual void drawLine( int16_t x0, int16_t y0, int16_t x1, int16_t y1, TUniColor color) 
		{
			int16_t steep = abs(y1 - y0) > abs(x1 - x0);
			if (steep) 
			{
				swap(x0, y0);
				swap(x1, y1);
			}

			if (x0 > x1) 
			{
				swap(x0, x1);
				swap(y0, y1);
			}

			int16_t dx, dy;
			dx = x1 - x0;
			dy = abs(y1 - y0);

			int16_t err = dx / 2;
			int16_t ystep;

			if (y0 < y1) 
				ystep = 1;

			else
				ystep = -1;

			for (; x0<=x1; x0++) 
			{
				if (steep)
					drawPixel(y0, x0, color);

				else
					drawPixel(x0, y0, color);
			
				err -= dy;
				if (err < 0) 
				{
					y0 += ystep;
					err += dx;
				}
			}
		}

		virtual void drawFastVLine( int16_t x, int16_t y, int16_t h, TUniColor color) 
		{
			// Update in subclasses if desired!
			drawLine( x, y, x, y+h-1, color);
		}

		virtual void drawFastHLine( int16_t x, int16_t y, int16_t w, TUniColor color) 
		{
			// Update in subclasses if desired!
			drawLine( x, y, x+w-1, y, color );
		}

		virtual size_t write( uint8_t c )
		{
//			Serial.println( c );
			if(( c != '\n' ) && ( c != '\r' ))
				if( cursor_y >= height() )
				{
//					Serial.println( "Scroll" );
					int ACorrection = ( cursor_y - height() ) + textsize * 8;
					Scroll( 0, -ACorrection, GetBackgroundColor() );
					cursor_y -= ACorrection;
				}

			if (c == '\n') 
			{
				cursor_y += textsize*8;
				cursor_x  = 0;
			} 
			else if (c == '\r') 
			{
				cursor_x  = 0;
			} 
			else 
			{
				drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
				cursor_x += textsize*6;
//				Serial.println( cursor_x );
				if (wrap && (cursor_x > ( width() - textsize*6))) 
				{
					cursor_y += textsize*8;
					cursor_x = 0;
				}
			}

			return 1;
		}

		// Draw a character
		void drawChar( int16_t x, int16_t y, unsigned char c, TUniColor color, TUniColor bg, uint8_t size )
		{
		  if((x >= width() )            || // Clip right
			 (y >= height() )           || // Clip bottom
			 ((x + 6 * size - 1) < 0) || // Clip left
			 ((y + 8 * size - 1) < 0))   // Clip top
			return;

		  if(!_cp437 && (c >= 176)) 
			  c++; // Handle 'classic' charset behavior

		  for (int8_t i=0; i<6; i++ ) 
		  {
			uint8_t line;
			if (i == 5) 
			  line = 0x0;

			else 
			  line = pgm_read_byte( FFont + 3 + (c*5)+i); // For now ifnore size information

			for (int8_t j = 0; j<8; j++) 
			{
			  if (line & 0x1) 
			  {
				if (size == 1) // default size
				  drawPixel(x+i, y+j, color);

				else  // big size
				  fillRect(x+(i*size), y+(j*size), size, size, color);
				 
			  }

			  else if (bg != color) 
			  {
				if (size == 1) // default size
				  drawPixel(x+i, y+j, bg);

				else  // big size
				  fillRect(x+i*size, y+j*size, size, size, bg);
				
			  }

			  line >>= 1;
			}
		  }
		}

		void setCursor(int16_t x, int16_t y) 
		{
			cursor_x = x;
			cursor_y = y;
		}

		void getCursor( int16_t &x, int16_t &y ) const 
		{
			x = cursor_x;
			y = cursor_y;
		}

		void setTextSize(uint8_t s) 
		{
			textsize = (s > 0) ? s : 1;
		}

		uint8_t getTextSize()
		{
			return textsize;
		}

		void setTextColor( TUniColor c, TUniColor b) 
		{
			textcolor   = c;
			textbgcolor = b; 
		}

		void setTextWrap(boolean w) 
		{
			wrap = w;
		}

		void cp437(boolean x) 
		{
			_cp437 = x;
		}

	public:
		// Return the size of the display (per current rotation)
		virtual int16_t width(void) const = 0;
		virtual int16_t height(void) const = 0;

	public:
		Graphics( const unsigned char * AFont ) :
			FFont( AFont ),
			wrap( true ),
			_cp437( false )
		{
			textcolor = textbgcolor = 0xFFFF;
		}
	};
//---------------------------------------------------------------------------
	class GraphicsImpl : public Graphics, public GraphicsIntf
	{
		typedef Graphics inherited;

	public:
		GraphicsOrientation	Orientation : 2;

	public:
		TUniColor	BackgroundColor = 0;

	public:
		virtual TUniColor	GetBackgroundColor() override
		{
			return BackgroundColor;
		}

		virtual Graphics *GetGraphics() override { return this; }

	public:
		GraphicsImpl( const unsigned char * AFont ) :
			inherited( AFont )
		{
			Orientation  = goUp;
		}
	};
//---------------------------------------------------------------------------
	class GraphicsElementSetCursor : public GraphicsElementClocked
	{
		typedef GraphicsElementClocked inherited;

	public:
		uint32_t X = 0;
		uint32_t Y = 0;

	public:
		virtual void Render( bool AForce ) override
		{
			if( ! AForce )
				if( ClockInputPin.IsConnected() )
					return;

			FOwner.GetGraphics()->setCursor( X, Y );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class GraphicsElementDrawScene : public GraphicsElementClocked, public GraphicsIntf
	{
		typedef GraphicsElementClocked inherited;

	public:
		TGraphicsPos X = 0;
		TGraphicsPos Y = 0;

	public:
		virtual void GetPosition( TGraphicsPos &AX, TGraphicsPos &AY ) override
		{ 
			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );

			AX = AParentX + X;
			AY = AParentY + Y;
		}

		virtual Graphics *GetGraphics() override
		{
			return FOwner.GetGraphics();
		}

	public:
		virtual void Render( bool AForce ) override
		{
			for( int i = 0; i < FElements.size(); ++ i )
				FElements[ i ]->Render( AForce );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
