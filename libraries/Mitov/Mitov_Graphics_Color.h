////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_GRAPHICS_COLOR_h
#define _MITOV_GRAPHICS_COLOR_h

#include <Mitov.h>
#include <Mitov_Graphics.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	class GraphicsColorElementFillScreen : public GraphicsElementClocked
	{
		typedef GraphicsElementClocked inherited;

	public:
		uint32_t	Color = 0xFF000000;

	public:
		virtual void Render( bool AForce ) override
		{
			if( ! AForce )
				if( ClockInputPin.IsConnected() )
					return;

			FOwner.GetGraphics()->ClearScreen( Color );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class GraphicsColorElementDrawPixel : public GraphicsElementClocked
	{
		typedef GraphicsElementClocked inherited;

	public:
		uint32_t	Color = 0xFFFFFFFF;
		TGraphicsPos		X = 0;
		TGraphicsPos		Y = 0;

	public:
		virtual void Render( bool AForce ) override
		{
			if( !( Color & 0xFF000000 ))
				return;

			if( ! AForce )
				if( ClockInputPin.IsConnected() )
					return;

			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );

			FOwner.GetGraphics()->drawPixel( AParentX + X, AParentY + Y, Color );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class GraphicsColorElementDrawRectangle : public GraphicsElementClocked
	{
		typedef GraphicsElementClocked inherited;

	public:
		uint32_t	Color = 0xFFFFFFFF;
		uint32_t	FillColor = 0;
		TGraphicsPos		X = 0;
		TGraphicsPos		Y = 0;
		TGraphicsSize	Width = 40;
		TGraphicsSize	Height = 20;

	public:
		virtual void Render( bool AForce ) override
		{
			if( !( ( FillColor & 0xFF000000 ) || ( Color & 0xFF000000 )))
				return;

			if( ! AForce )
				if( ClockInputPin.IsConnected() )
					return;

			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );

			Graphics *AGraphics = FOwner.GetGraphics();

			if( FillColor & 0xFF000000 )
				AGraphics->fillRect( AParentX + X, AParentY + Y, Width, Height, FillColor );

			if( Color & 0xFF000000 )
				AGraphics->drawRect( AParentX + X, AParentY + Y, Width, Height, Color );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class GraphicsColorElementDrawRoundRectangle : public GraphicsElementClocked
	{
		typedef GraphicsElementClocked inherited;

	public:
		uint32_t	Color = 0xFFFFFFFF;
		uint32_t	FillColor = 0;
		TGraphicsPos		X = 0;
		TGraphicsPos		Y = 0;
		TGraphicsSize	Width = 40;
		TGraphicsSize	Height = 20;
		TArduinoGraphicsCornerSize	CornerSize;

	public:
		virtual void Render( bool AForce ) override
		{
			if( !( ( FillColor & 0xFF000000 ) || ( Color & 0xFF000000 )))
				return;

			if( ! AForce )
				if( ClockInputPin.IsConnected() )
					return;

			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );

			Graphics *AGraphics = FOwner.GetGraphics();

			if( FillColor & 0xFF000000 )
				AGraphics->fillRoundRect( AParentX + X, AParentY + Y, Width, Height, CornerSize.X, CornerSize.Y, FillColor );

			if( Color & 0xFF000000 )
				AGraphics->drawRoundRect( AParentX + X, AParentY + Y, Width, Height, CornerSize.X, CornerSize.Y, Color );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class GraphicsColorElementDrawEllipse : public GraphicsElementClocked
	{
		typedef GraphicsElementClocked inherited;

	public:
		uint32_t		Color = 0xFFFFFFFF;
		uint32_t		FillColor = 0;
		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;
		TGraphicsSize	Width = 40;
		TGraphicsSize	Height = 20;
		bool			Centered	= false;

	public:
		virtual void Render( bool AForce ) override
		{
			if( !( ( FillColor & 0xFF000000 ) || ( Color & 0xFF000000 )))
				return;

			if( ! AForce )
				if( ClockInputPin.IsConnected() )
					return;

			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );

			Graphics *AGraphics = FOwner.GetGraphics();

			TGraphicsPos	AX = X;
			TGraphicsPos	AY = Y;

			if( Centered )
			{
				AX -= Width / 2;
				AY -= Height / 2;
			}

			if( FillColor & 0xFF000000 )
				AGraphics->fillEllipseWH( AParentX + AX, AParentY + AY, Width, Height, FillColor );

			if( Color & 0xFF000000 )
				AGraphics->drawEllipseWH( AParentX + AX, AParentY + AY, Width, Height, Color );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class GraphicsColorElementDrawLine : public GraphicsElementClocked
	{
		typedef GraphicsElementClocked inherited;

	public:
		uint32_t	Color = 0xFFFFFFFF;
		TGraphicsPos		X = 0;
		TGraphicsPos		Y = 0;
		TGraphicsPos		Width = 40;
		TGraphicsPos		Height = 20;

	public:
		virtual void Render( bool AForce ) override
		{
			if( ! ( Color & 0xFF000000 ))
				return;

			if( ! AForce )
				if( ClockInputPin.IsConnected() )
					return;

			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );

			Graphics *AGraphics = FOwner.GetGraphics();

			AGraphics->drawLine( AParentX + X, AParentY + Y, AParentX + X + Width, AParentY + Y + Height, Color );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class GraphicsColorElementDrawLines : public GraphicsElementClocked
	{
		typedef GraphicsElementClocked inherited;

	public:
		uint32_t	Color = 0xFFFFFFFF;
		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;

	public:
		GraphicsPoint	*Points;
		uint32_t	_PointsCount = 0;

	public:
		virtual void Render( bool AForce ) override
		{
			if( _PointsCount == 0 )
				return;

			if( !( Color & 0xFF000000 ))
				return;

			if( ! AForce )
				if( ClockInputPin.IsConnected() )
					return;

			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );

			Graphics *AGraphics = FOwner.GetGraphics();

			TGraphicsPos	AX1 = X;
			TGraphicsPos	AY1 = Y;

			for( uint32_t i = 0; i < _PointsCount; ++i )
			{
				TGraphicsPos	AX2 = Points[ i ].X + X;
				TGraphicsPos	AY2 = Points[ i ].Y + Y;

				AGraphics->drawLine( AParentX + AX1, AParentY + AY1, AParentX + AX2, AParentY + AY2, Color );

				AX1 = AX2;
				AY1 = AY2;
			}
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class GraphicsColorElementDrawPolygon : public GraphicsElementClocked
	{
		typedef GraphicsElementClocked inherited;

	public:
		uint32_t	Color = 0xFFFFFFFF;
		uint32_t	FillColor = 0;
		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;

	public:
		GraphicsPoint	*Points;
		uint32_t	_PointsCount = 0;

	public:
		virtual void Render( bool AForce ) override
		{
			if( _PointsCount == 0 )
				return;

			if( !( ( FillColor & 0xFF000000 ) || ( Color & 0xFF000000 )))
				return;

			if( ! AForce )
				if( ClockInputPin.IsConnected() )
					return;

			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );

			Graphics *AGraphics = FOwner.GetGraphics();

			TGraphicsPos	AStartX = AParentX + X;
			TGraphicsPos	AStartY = AParentY + Y;

			if( FillColor & 0xFF000000 )
			{
				TGraphicsPos	AMinX = AStartX;
				TGraphicsPos	AMinY = AStartY;
				TGraphicsPos	AMaxX = AStartX;
				TGraphicsPos	AMaxY = AStartY;

				for( uint32_t i = 0; i < _PointsCount; ++i )
				{
					TGraphicsPos	AX2 = Points[ i ].X + AStartX;
					TGraphicsPos	AY2 = Points[ i ].Y + AStartY;

					if( AX2 < AMinX )
						AMinX = AX2;

					if( AX2 > AMaxX )
						AMaxX = AX2;

					if( AY2 < AMinY )
						AMinY = AY2;

					if( AY2 > AMaxY )
						AMaxY = AY2;

				}

				if( AMinX < 0 )
				 AMinX = 0;

				if( AMinY < 0 )
				 AMinY = 0;

				TGraphicsPos	AWidth = AGraphics->width();
				TGraphicsPos	AHeight = AGraphics->height();

				if( AMaxX > AWidth )
				 AMaxX = AWidth;

				if( AMaxY > AHeight )
				 AMaxY = AHeight;

				if( AMinX > AMaxX )
					return;

				if( AMinY > AMaxY )
					return;

				TGraphicsPos *nodeX = new TGraphicsPos[ _PointsCount ];
				for( int pixelY = AMinY; pixelY < AMaxY; ++ pixelY )
				{
					//  Build a list of nodes.
					int nodes = 0;
					int j = _PointsCount - 1;
					for( uint32_t i = 0; i < _PointsCount; i++ )
					{
						if ( (( Points[ i ].Y + AStartY ) < pixelY && ( Points[ j ].Y + AStartY ) >= pixelY ) || (( Points[ j ].Y + AStartY ) < pixelY && ( Points[ i ].Y + AStartY ) >= pixelY ))
							nodeX[nodes ++] = ( Points[i].X + AStartX ) + float( pixelY - ( Points[i].Y + AStartY ) ) / ( Points[j].Y - Points[i].Y ) * float( (Points[j].X - Points[i].X ) ) + 0.5;

						j = i;
					}

					//  Sort the nodes, via a simple “Bubble” sort.
					int i = 0;
					while( i < nodes - 1 )
					{
						if( nodeX[ i ] > nodeX[ i + 1 ] )
						{
							TGraphicsPos swap = nodeX[ i ];
							nodeX[ i ] = nodeX[ i + 1 ];
							nodeX[ i + 1 ] = swap;
							if( i )
								i--;
						}

						else
							i++;
						
					}

					//  Fill the pixels between node pairs.
					for( int i = 0; i < nodes; i += 2 ) 
					{
						TGraphicsPos ALeft = nodeX[ i ];
						TGraphicsPos ARight = nodeX[ i + 1 ];

						if( ALeft >= AWidth )
							break;

						if( ARight > 0 ) 
						{
							if( ALeft < 0 ) 
								ALeft = 0;

							if( ARight > AWidth )
								ARight = AWidth;

							AGraphics->drawFastHLine( ALeft, pixelY, ARight - ALeft, FillColor );

	//					for (pixelX=nodeX[i]; pixelX<nodeX[i+1]; pixelX++) fillPixel(pixelX,pixelY); 
						}
					}

				}

			}

			if( Color & 0xFF000000 )
			{
				TGraphicsPos	AX1 = X;
				TGraphicsPos	AY1 = Y;

				for( uint32_t i = 1; i < _PointsCount; ++i )
				{
					TGraphicsPos	AX2 = Points[ i ].X + X;
					TGraphicsPos	AY2 = Points[ i ].Y + Y;

					AGraphics->drawLine( AParentX + AX1, AParentY + AY1, AParentX + AX2, AParentY + AY2, Color );

					AX1 = AX2;
					AY1 = AY2;
				}

				AGraphics->drawLine( AParentX + AX1, AParentY + AY1, AParentX + X, AParentY + Y, Color );
			}
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class GraphicsColorElementDrawText : public GraphicsElementClocked, public GraphicsTextIntf
	{
		typedef GraphicsElementClocked inherited;

	public:
		uint32_t	Color = 0xFFFFFFFF;
		uint32_t	FillColor = 0;
		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;

		String	Text;

		uint8_t	Size : 4;
		bool	Wrap : 1;

	public:
		virtual void Render( bool AForce ) override
		{
			if( !( ( FillColor & 0xFF000000 ) || ( Color & 0xFF000000 )))
				return;

			if( ! AForce )
				if( ClockInputPin.IsConnected() )
					return;

			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );

			Graphics *AGraphics = FOwner.GetGraphics();

			int16_t ACursorY; // = AGraphics->getCursorY();
			int16_t ACursorX; // = AGraphics->getCursorX();

			AGraphics->getCursor( ACursorX, ACursorY );
			TUniColor AOldColor;
			TUniColor AOldBackgroundColor;

			uint8_t	AOldSize = AGraphics->getTextSize();
			bool AOldWrap = AGraphics->getTextWrap();

			for( int i = 0; i < FElements.size(); ++ i )
				FElements[ i ]->Enter( AGraphics );

			AGraphics->setTextSize( Size );
			AGraphics->setTextWrap( Wrap );			

			AGraphics->getTextColor( AOldColor, AOldBackgroundColor );

			AGraphics->setTextColor( Color, FillColor );

			AGraphics->setCursor( X, Y );
			AGraphics->print( Text );

			AGraphics->setTextColor( AOldColor, AOldBackgroundColor );

			AGraphics->setCursor( ACursorX, ACursorY );

			AGraphics->setTextWrap( AOldWrap );
			AGraphics->setTextSize( AOldSize );

			for( int i = 0; i < FElements.size(); ++ i )
				FElements[ i ]->Leave( AGraphics );

		}

	public:
		GraphicsColorElementDrawText( GraphicsIntf &AOwner ) :
			inherited( AOwner ),
			Size( 1 ),
			Wrap( true )
		{
		}

	};
//---------------------------------------------------------------------------
	class GraphicsColorElementTextField : public GraphicsElementClocked, public GraphicsTextIntf
	{
		typedef GraphicsElementClocked inherited;

	public:
		OpenWire::ValueSinkPin<String>	InputPin;

	public:
		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;
		uint16_t Width = 6;
		uint32_t	Color = 0xFFFFFFFF;
		uint32_t	FillColor = 0xFF000000;
		TArduinoTextHorizontalAlign	HorizontalAlign : 2;
		bool AutoSize : 1;
		bool Wrap : 1;

	protected:
		uint8_t	FOldSize = 0;

	public:
		virtual void Render( bool AForce ) override
		{
			if( !( ( FillColor & 0xFF000000 ) || ( Color & 0xFF000000 )))
				return;

			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );

			Graphics *AGraphics = FOwner.GetGraphics();

			int16_t ACursorY; // = AGraphics->getCursorY();
			int16_t ACursorX; // = AGraphics->getCursorX();

			AGraphics->getCursor( ACursorX, ACursorY );

			TUniColor AOldColor;
			TUniColor AOldBackgroundColor;

			for( int i = 0; i < FElements.size(); ++ i )
				FElements[ i ]->Enter( AGraphics );

			bool AOldWrap = AGraphics->getTextWrap();

			AGraphics->setTextWrap( Wrap );			

			AGraphics->getTextColor( AOldColor, AOldBackgroundColor );

			AGraphics->setTextColor( Color, FillColor );

			AGraphics->setCursor( X, Y );
			String AText = InputPin.Value;
			if( AutoSize )
			{
				uint8_t ANewSize = AText.length();
				while( AText.length() < FOldSize )
					AText += " ";				

				FOldSize = ANewSize;
			}

			else
			{
				String AText = InputPin.Value;
				while( AText.length() < Width )
				{
					switch( HorizontalAlign )
					{
						case thaLeft :
							AText += " ";
							break;

						case thaRight :
							AText = " " + AText;
							break;

						case thaCenter :
							AText = " " + AText + " ";
							break;
					}
				}

				AText = AText.substring( 0, Width );
			}

			AGraphics->print( AText );

			AGraphics->setTextColor( AOldColor, AOldBackgroundColor );
			AGraphics->setCursor( ACursorX, ACursorY );
			AGraphics->setTextWrap( AOldWrap );			

			for( int i = 0; i < FElements.size(); ++ i )
				FElements[ i ]->Leave( AGraphics );

		}

	protected:
		void DoReceiveData( void *_Data )
		{
			if( ! inherited::ClockInputPin.IsConnected() )
				Render( true );
		}

	public:
		GraphicsColorElementTextField( GraphicsIntf &AOwner ) :
			inherited( AOwner ),
			HorizontalAlign( thaLeft ),
			AutoSize( true ),
			Wrap( true )
		{			
			InputPin.SetCallback( MAKE_CALLBACK( GraphicsColorElementTextField::DoReceiveData ));
		}

		GraphicsColorElementTextField( GraphicsIntf &AOwner, char *AInitialValue ) :
			inherited( AOwner ),
			HorizontalAlign( thaLeft ),
			AutoSize( true ),
			Wrap( true )
		{
			InputPin.Value = AInitialValue;
			InputPin.SetCallback( MAKE_CALLBACK( GraphicsColorElementTextField::DoReceiveData ));
		}

	};
//---------------------------------------------------------------------------
	class GraphicsColorElementScroll : public GraphicsElementClocked
	{
		typedef GraphicsElementClocked inherited;

	public:
		uint32_t	Color = 0xFF000000;
		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;

	public:
		virtual void Render( bool AForce ) override
		{
			if( ! AForce )
				if( ClockInputPin.IsConnected() )
					return;

			FOwner.GetGraphics()->Scroll( X, Y, Color );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class GraphicsColorElementCheckPixel : public GraphicsElementClocked
	{
		typedef GraphicsElementClocked inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		uint32_t X = 0;
		uint32_t Y = 0;

	public:
		virtual void Render( bool AForce ) override
		{
			if( ! AForce )
				if( ClockInputPin.IsConnected() )
					return;

			uint32_t AValue = FOwner.GetGraphics()->GetPixelColor( X, Y );
			OutputPin.SendValue<uint32_t>( AValue );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
#ifdef _GRAPHICS_DRAW_ALPHA_COLOR_BITMAP_

	class GraphicsColorElementDrawAlphaBitmap : public Mitov::GraphicsElementClocked
	{
		typedef Mitov::GraphicsElementClocked inherited;

    public:
		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;
		TGraphicsSize	Width;
		TGraphicsSize	Height;

		const uint32_t *_Data;

	public:
		virtual void Render( bool AForce ) override
		{
			if( ! AForce )
				if( ClockInputPin.IsConnected() )
					return;

			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );

			FOwner.GetGraphics()->drawAlphaColorBitmap( AParentX + X, AParentY + Y, _Data, Width, Height );
		}

	public:
		using inherited::inherited;

	};

#endif // _GRAPHICS_DRAW_ALPHA_COLOR_BITMAP_
//---------------------------------------------------------------------------	
#ifdef _GRAPHICS_DRAW_REPEAT_ALPHA_COLOR_BITMAP_

	class GraphicsColorRepeatElementDrawAlphaBitmap : public Mitov::GraphicsElementClocked
	{
		typedef Mitov::GraphicsElementClocked inherited;

    public:
		TGraphicsPos		X = 0;
		TGraphicsPos		Y = 0;
		uint16_t	Width;
		uint16_t	Height;
		uint16_t	Count;

		const uint8_t *_Data;

	public:
		virtual void Render( bool AForce ) override
		{
			if( ! AForce )
				if( ClockInputPin.IsConnected() )
					return;

			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );

			FOwner.GetGraphics()->drawRepeatAlphaColorBitmap( AParentX + X, AParentY + Y, _Data, Count, Width, Height );
		}

	public:
		using inherited::inherited;

	};

#endif // _GRAPHICS_DRAW_REPEAT_ALPHA_COLOR_BITMAP_
//---------------------------------------------------------------------------	
#ifdef _GRAPHICS_DRAW_COLOR_BITMAP_

	class GraphicsColorElementDrawBitmap : public Mitov::GraphicsElementClocked
	{
		typedef Mitov::GraphicsElementClocked inherited;

    public:
		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;
		uint16_t	Width;
		uint16_t	Height;

		const uint8_t *_Bytes;

	public:
		virtual void Render( bool AForce ) override
		{
			if( ! AForce )
				if( ClockInputPin.IsConnected() )
					return;

			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );

			FOwner.GetGraphics()->drawColorBitmap( AParentX + X, AParentY + Y, _Bytes, Width, Height );
		}

	public:
		using inherited::inherited;

	};

#endif // _GRAPHICS_DRAW_COLOR_BITMAP_
//---------------------------------------------------------------------------	
#ifdef _GRAPHICS_DRAW_REPEAT_COLOR_BITMAP_

	class GraphicsColorRepeatElementDrawBitmap : public Mitov::GraphicsElementClocked
	{
		typedef Mitov::GraphicsElementClocked inherited;

    public:
		TGraphicsPos		X = 0;
		TGraphicsPos		Y = 0;
		uint16_t	Width;
		uint16_t	Height;
		uint16_t	Count;

		const uint8_t *_Data;

	public:
		virtual void Render( bool AForce ) override
		{
			if( ! AForce )
				if( ClockInputPin.IsConnected() )
					return;

			TGraphicsPos AParentX, AParentY;
			FOwner.GetPosition( AParentX, AParentY );

			FOwner.GetGraphics()->drawRepeatColorBitmap( AParentX + X, AParentY + Y, _Data, Count, Width, Height );
		}

	public:
		using inherited::inherited;

	};

#endif // _GRAPHICS_DRAW_REPEAT_COLOR_BITMAP_
//---------------------------------------------------------------------------	
	class GraphicsElementColorMonohromeDrawScene : public Mitov::GraphicsElementClocked, public GraphicsIntf, public Graphics
	{
		typedef Mitov::GraphicsElementClocked inherited;

    public:
		uint32_t	WhiteColor = 0xFFFFFFFF;
		uint32_t	BlackColor = 0xFF000000;
		uint32_t	BackgroundColor = 0;
		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;

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
			return this;
		}

	public:
		virtual TUniColor	GetBackgroundColor() override
		{
			return BackgroundColor;
		}

		virtual int16_t width(void) const override
		{
			return FOwner.GetGraphics()->width() - X;
		}

		virtual int16_t height(void) const override
		{
			return FOwner.GetGraphics()->height() - Y;
		}

		virtual void drawPixel(int16_t x, int16_t y, TUniColor color) override
		{
			if( color == tmcNone )
				return;

			Graphics *AGraphics = FOwner.GetGraphics();

			switch( color )
			{
				case tmcWhite:	color =	WhiteColor; break;
				case tmcBlack:	color =	BlackColor; break;
				case tmcInvert:	color =	( AGraphics->GetPixelColor( x, y ) > 0xFF000000 ) ? WhiteColor : BlackColor; break;
			}

			AGraphics->drawPixel( x, y, color );
		}

		virtual TUniColor GetPixelColor( int16_t x, int16_t y ) override
		{
			return ( FOwner.GetGraphics()->GetPixelColor( x, y ) > 0xFF000000 ) ? tmcWhite : tmcBlack;
		}

	public:
		virtual void Render( bool AForce ) override
		{
			for( int i = 0; i < GraphicsIntf::FElements.size(); ++ i )
				GraphicsIntf::FElements[ i ]->Render( AForce );
		}

	public:
		GraphicsElementColorMonohromeDrawScene( GraphicsIntf &AOwner, const unsigned char *AFont ) :
			inherited( AOwner ),
			Graphics( AFont )
		{
		}

	};
//---------------------------------------------------------------------------
	class GraphicsElementColorGrayscaleDrawScene : public Mitov::GraphicsElementClocked, public GraphicsIntf, public Graphics
	{
		typedef Mitov::GraphicsElementClocked inherited;

    public:
		uint32_t	BackgroundColor = 0;
		TGraphicsPos	X = 0;
		TGraphicsPos	Y = 0;

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
			return this;
		}

	public:
		virtual TUniColor	GetBackgroundColor() override
		{
			return BackgroundColor;
		}

		virtual int16_t width(void) const override
		{
			return FOwner.GetGraphics()->width() - X;
		}

		virtual int16_t height(void) const override
		{
			return FOwner.GetGraphics()->height() - Y;
		}

		virtual void drawPixel(int16_t x, int16_t y, TUniColor color) override
		{
			if( !( color & 0xFF00 ))
				return;

			Graphics *AGraphics = FOwner.GetGraphics();

			uint16_t AGray = color & 0xFF;
			color = ( color << 16 ) | ( AGray << 8 ) | AGray;

			AGraphics->drawPixel( x, y, color );
		}

		virtual TUniColor GetPixelColor( int16_t x, int16_t y ) override
		{
			uint32_t	AColor = FOwner.GetGraphics()->GetPixelColor( x, y );
			uint8_t ARed = AColor >> 16;
			uint8_t AGreen = AColor >> 8;
			uint8_t ABlue = AColor >> 8;
			uint16_t AGray = 0.2989 * ARed + 0.5870 * AGreen + 0.1140 * ABlue + 0.5;
			AGray |= ( AColor >> 16 ) & 0xFF00;
			return AGray;
		}

	public:
		virtual void Render( bool AForce ) override
		{
			for( int i = 0; i < GraphicsIntf::FElements.size(); ++ i )
				GraphicsIntf::FElements[ i ]->Render( AForce );
		}

	public:
		GraphicsElementColorGrayscaleDrawScene( GraphicsIntf &AOwner, const unsigned char *AFont ) :
			inherited( AOwner ),
			Graphics( AFont )
		{
		}

	};
//---------------------------------------------------------------------------
	class TGraphicsColorTextSettings
	{
	public:
		uint8_t	Size : 4;
		bool	Wrap : 1;
		uint32_t	Color = 0xFFFFFFFF;
		uint32_t	BackgroundColor = 0xFF000000;

	public:
		TGraphicsColorTextSettings() :
			Size( 1 ),
			Wrap( true )
		{
		}
	};
//---------------------------------------------------------------------------
}

#endif
