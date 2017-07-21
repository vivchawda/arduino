////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_FUJITSU_FRAM_h
#define _MITOV_FUJITSU_FRAM_h

#include <Mitov.h>
#include <Mitov_Basic_I2C.h>
#include <Mitov_Memory.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	class FujitsuFRAMMB85I2C : public OpenWire::Component, public Mitov::MemoryIntf
	{
		typedef	OpenWire::Component	inherited;
		
	public:
		byte Address = 0x50;

	protected:
		bool		FCached = false;
		uint32_t	FCachedAddress = 0;
		byte		FCachedValue = 0;

		TwoWire &FWire;

	public:
		virtual bool ReadData( uint32_t AAddress, uint32_t ASize, byte *AData ) override
		{
			if( FCached )
			{
				if( AAddress == FCachedAddress )
				{
					*AData ++ = FCachedValue;
					--ASize;
					++ AAddress;
				}
			}

			if( ASize == 0 )
				return	true;

			bool AResult = I2C::ReadBytes_16bitAddress( FWire, Address, true, AAddress, ASize, AData );
			if( AResult )
			{
				FCached = true;
				FCachedAddress = AAddress + ASize - 1;
				FCachedValue = AData[ ASize - 1 ];
			}

			return AResult;
		}

		virtual bool WriteData( uint32_t AAddress, uint32_t ASize, byte *AData ) override
		{
			FCached = false;
			I2C::WriteBytes_16bitAddress( FWire, Address, true, AAddress, ASize, AData );
			return true;
		}

	protected:
		virtual void SystemInit() override
		{
			inherited::SystemInit();

			uint32_t AAddress = 0;
			byte AOffset = 0;
			for( int i = 0; i < FElements.size(); ++ i )
				FElements[ i ]->PopulateAddress( AAddress, AOffset );

		}

		virtual void SystemLoopUpdateHardware() override
		{
			for( int i = 0; i < FElements.size(); ++ i )
				FElements[ i ]->ProcessWrite( this );

//			inherited::SystemLoopEnd();
		}

	public:
		FujitsuFRAMMB85I2C( TwoWire &AWire ) :
			FWire( AWire )
		{
		}

	};
//---------------------------------------------------------------------------
	enum TFujitsuFRAMProtectedBlocks { pbNone, pbUppeQuarter, pbUppeHalf, pbAll };
//---------------------------------------------------------------------------
	class FujitsuFRAMMB85SPI : public Mitov::Basic_Enable_SPI, public Mitov::MemoryIntf
	{
		typedef	Mitov::Basic_Enable_SPI	inherited;

	protected:
		static const byte OPCODE_WREN   = 0b0110;     // Write Enable Latch
		static const byte OPCODE_WRDI   = 0b0100;     // Reset Write Enable Latch
		static const byte OPCODE_RDSR   = 0b0101;     // Read Status Register
		static const byte OPCODE_WRSR   = 0b0001;     // Write Status Register
		static const byte OPCODE_READ   = 0b0011;     // Read Memory
		static const byte OPCODE_WRITE  = 0b0010;     // Write Memory
		static const byte OPCODE_RDID   = 0b10011111;  // Read Device ID

	public:
		OpenWire::SourcePin	WriteProtectedOutputPin;

	public:
		bool	WriteEnable : 1; // = false;
		TFujitsuFRAMProtectedBlocks ProtectedBlocks : 2;

	protected:
		bool		FCached : 1;
		uint32_t	FCachedAddress = 0;
		byte		FCachedValue = 0;

	public:
		void UpdateWriteProtect()
		{
			ChipSelectOutputPin.SendValue( false );
			FSPI.transfer( WriteEnable ? OPCODE_WREN : OPCODE_WRDI );
			ChipSelectOutputPin.SendValue( true );
		}

		void UpdateProtectedBlocks()
		{
			ChipSelectOutputPin.SendValue( false );
			FSPI.transfer( OPCODE_WREN );
			ChipSelectOutputPin.SendValue( true );

			ChipSelectOutputPin.SendValue( false );
			FSPI.transfer( OPCODE_WRSR );
			FSPI.transfer( ((byte)ProtectedBlocks ) << 2 );
			ChipSelectOutputPin.SendValue( true );

		}
		
	public:
		virtual bool ReadData( uint32_t AAddress, uint32_t ASize, byte *AData ) override
		{
			if( FCached )
			{
				if( AAddress == FCachedAddress )
				{
					*AData ++ = FCachedValue;
					--ASize;
					++ AAddress;
				}
			}

			if( ASize == 0 )
				return	true;

			ChipSelectOutputPin.SendValue( false );

			FSPI.transfer( OPCODE_READ );
			FSPI.transfer((uint8_t)(AAddress >> 8));
			FSPI.transfer((uint8_t)(AAddress & 0xFF));
			for( int i = 0; i < ASize; ++i )
				*AData ++ = FSPI.transfer(0);

			ChipSelectOutputPin.SendValue( true );

			FCached = true;
			FCachedAddress = AAddress + ASize - 1;
			FCachedValue = AData[ -1 ];

			return true;
		}

		virtual bool WriteData( uint32_t AAddress, uint32_t ASize, byte *AData ) override
		{
			FCached = false;

			UpdateWriteProtect();

			ChipSelectOutputPin.SendValue( false );
			FSPI.transfer( OPCODE_WRITE );
			FSPI.transfer((uint8_t)(AAddress >> 8));
			FSPI.transfer((uint8_t)(AAddress & 0xFF));
			for( int i = 0; i < ASize; ++i )
				FSPI.transfer( *AData ++ );

			ChipSelectOutputPin.SendValue( true );
			return true;
		}

	protected:
		virtual void SystemInit() override
		{
			inherited::SystemInit();

			uint32_t AAddress = 0;
			byte AOffset = 0;
			for( int i = 0; i < FElements.size(); ++ i )
				FElements[ i ]->PopulateAddress( AAddress, AOffset );

			UpdateWriteProtect();
			UpdateProtectedBlocks();
		}

		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
			if( WriteProtectedOutputPin.IsConnected() )
			{
				ChipSelectOutputPin.SendValue( false );

				FSPI.transfer( OPCODE_RDSR );
				byte Avalue = FSPI.transfer(0);

				WriteProtectedOutputPin.SendValue( ( Avalue & 0x80 ) != 0 );

				ChipSelectOutputPin.SendValue( true );
			}
		}
/*
		virtual void SystemLoopEnd() override
		{
			for( int i = 0; i < FElements.size(); ++ i )
				FElements[ i ]->ProcessRead( this );

//			inherited::SystemLoopEnd();
		}
*/
		virtual void SystemLoopUpdateHardware() override
		{
			if( WriteEnable )
				for( int i = 0; i < FElements.size(); ++ i )
					FElements[ i ]->ProcessWrite( this );

//			inherited::SystemLoopEnd();
		}

	public:
		FujitsuFRAMMB85SPI( BasicSPI &ASPI ) :
			inherited( ASPI ),
			FCached( false ),
			WriteEnable( false ),
			ProtectedBlocks( pbNone )
		{
		}

	};
//---------------------------------------------------------------------------
}

#endif
