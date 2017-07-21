////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_HEADER_ID_h
#define _MITOV_HEADER_ID_h

#include <Mitov.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	class AddPacketHeaderID : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		bool	Enabled = true;
		ConstBytes	ID;

	protected:
		void send( const uint8_t* data, uint8_t len )
		{
			if( ! Enabled )
				OutputPin.SendValue( Mitov::TDataBlock( len, data ));

			else
			{
				int ASize = len + ID._BytesSize;
				uint8_t *Adata = new uint8_t[ ASize ];

				memcpy( Adata, ID._Bytes, ID._BytesSize );
				memcpy( Adata + ID._BytesSize, data, len );

				OutputPin.SendValue( Mitov::TDataBlock( ASize, Adata ));

				delete [] Adata;
			}
		}

	public:
		void Print( String AValue )
		{
			AValue += "\r\n";
			send( (uint8_t *)AValue.c_str(), AValue.length() );
		}

		void Print( float AValue )
		{
			char AText[ 16 ];
			dtostrf( AValue,  1, 2, AText );
			Print( String( AText ));
		}

		void Print( int32_t AValue )
		{
			char AText[ 16 ];
			itoa( AValue, AText, 10 );
			Print( String( AText ));
		}

		void Print( uint32_t AValue )
		{
			char AText[ 16 ];
			itoa( AValue, AText, 10 );
			Print( String( AText ));
		}

		void PrintChar( char AValue )
		{
			send( (uint8_t*)&AValue, 1 );
		}

		void PrintChar( byte AValue )
		{
			send( &AValue, 1 );
		}

		void Write( uint8_t *AData, uint32_t ASize )
		{
			send( AData, ASize );
		}

	};
//---------------------------------------------------------------------------
	class DetectPacketHeaderID : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;
		OpenWire::SourcePin	OutputPin;

	public:
		bool	Enabled = true;
		ConstBytes	ID;

	protected:
		void DoReceive( void *_Data )
		{
			if( ! Enabled )
			{
				OutputPin.Notify( _Data );
				return;
			}

			Mitov::TDataBlock AInDataBlock = *(Mitov::TDataBlock*)_Data;

			if( AInDataBlock.Size < ID._BytesSize )
				return;

			if( memcmp( AInDataBlock.Data, ID._Bytes, ID._BytesSize ) != 0 )
				return;

			OutputPin.SendValue( Mitov::TDataBlock( AInDataBlock.Size - ID._BytesSize, AInDataBlock.Data + ID._BytesSize ));
		}

	public:
		DetectPacketHeaderID()
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&DetectPacketHeaderID::DoReceive );
		}
	};
//---------------------------------------------------------------------------
}

#endif
