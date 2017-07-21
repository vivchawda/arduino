////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_BLUETOOTH_LE_h
#define _MITOV_BLUETOOTH_LE_h

#include <Mitov.h>

#ifdef VISUINO_ARDUINO_101
  #include <CurieBLE.h>
#else // VISUINO_ARDUINO_101
  #include <BLEPeripheral.h>
#endif // VISUINO_ARDUINO_101

namespace Mitov
{
//---------------------------------------------------------------------------
	enum TArduinoBluetoothSensorLocation { slNone, slOther, slChest, slWrist, slFinger, slHand, slEarLobe, slFoot };
//---------------------------------------------------------------------------
	class ArduinoBlutoothUpdateIntf
	{
	public:
		virtual void UpdateValues() = 0;
	};
//---------------------------------------------------------------------------
	class ArduinoBluetoothLE : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SourcePin ConnectedOutputPin;
		OpenWire::SourcePin MACAddressOutputPin;

	public:
		String	LocalName;
		String	DeviceName;

	public:
		BLEPeripheral	FPeripheral;

	public:
		void RegisterUpdateElement( ArduinoBlutoothUpdateIntf *AElement )
		{
			FElements.push_back( AElement );
		}

	protected:
		Mitov::SimpleList<ArduinoBlutoothUpdateIntf *>	FElements;
		bool	FConnected = false;

	protected:
		virtual void SystemLoopBegin( unsigned long currentMicros ) override
		{
//			inherited::SystemLoopBegin( currentMicros );
			BLECentral ACentral = FPeripheral.central();
			if( FConnected )
			{
				if( !ACentral )
				{
					FConnected = false;
					ConnectedOutputPin.Notify( &FConnected );
				}
			}
			else
			{
				if( ACentral )
				{
					FConnected = true;
					ConnectedOutputPin.Notify( &FConnected );
					String AAddress = ACentral.address();
					MACAddressOutputPin.SendValue( AAddress );
				}
			}

			if( FConnected )
			{
				for( int i = 0; i < FElements.size(); ++ i )
					FElements[ i ]->UpdateValues();
			}
		}

		virtual void SystemInit() override
		{
			inherited::SystemInit();
			if( LocalName != "" )
				FPeripheral.setLocalName( LocalName.c_str() );

			if( DeviceName != "" )
				FPeripheral.setDeviceName( DeviceName.c_str() );
		}

		virtual void SystemStart() override
		{
			inherited::SystemStart();
			FPeripheral.begin();
		}
	};
//---------------------------------------------------------------------------
	class ArduinoBluetoothLEService : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		String	UUID;
		bool	Advertise = false;

	public:
		ArduinoBluetoothLE &FOwner;

	protected:
		BLEService	*FService;

	protected:
		virtual void SystemInit() override
		{
			inherited::SystemInit();
			FService = new BLEService( UUID.c_str() );
			FOwner.FPeripheral.addAttribute( *FService );
			if( Advertise )
				FOwner.FPeripheral.setAdvertisedServiceUuid( FService->uuid());
		}

	public:
		ArduinoBluetoothLEService( ArduinoBluetoothLE &AOwner ) :
			FOwner( AOwner )
		{
		}
	};
//---------------------------------------------------------------------------
	template<typename T_CHARACT> class ArduinoBluetoothLEBasicCharacteristic : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		String	UUID;

	protected:
		ArduinoBluetoothLEService &FOwner;
		T_CHARACT	*FCharacteristic;

	public:
		ArduinoBluetoothLEBasicCharacteristic( ArduinoBluetoothLEService &AOwner ) :
			FOwner( AOwner )
		{
		}
	};
//---------------------------------------------------------------------------
	template<typename T, typename T_CHARACT> class ArduinoBluetoothLETypedBasicWriteCharacteristic : public ArduinoBluetoothLEBasicCharacteristic<T_CHARACT>, public ArduinoBlutoothUpdateIntf
	{
		typedef ArduinoBluetoothLEBasicCharacteristic<T_CHARACT> inherited;

	public:
		OpenWire::TypedSourcePin<T>	OutputPin;

	public:
		T		InitialValue;
		bool	BigEndian = false;

	public:
		virtual void UpdateValues()
		{
			if( inherited::FCharacteristic->written() )
				OutputPin.SetValue( inherited::FCharacteristic->value(), true );

		}

	public:
		ArduinoBluetoothLETypedBasicWriteCharacteristic( ArduinoBluetoothLEService &AOwner ) :
			inherited( AOwner )
		{
			AOwner.FOwner.RegisterUpdateElement( this );
		}

	};
//---------------------------------------------------------------------------
	template<typename T, typename T_CHARACT> class ArduinoBluetoothLETypedWriteCharacteristic : public ArduinoBluetoothLETypedBasicWriteCharacteristic<T, T_CHARACT>
	{
		typedef ArduinoBluetoothLETypedBasicWriteCharacteristic<T, T_CHARACT> inherited;

	protected:
		virtual void SystemInit() override
		{
			inherited::SystemInit();
			inherited::FCharacteristic = new T_CHARACT( inherited::UUID.c_str(), BLEWrite );
			inherited::FOwner.FOwner.FPeripheral.addAttribute( *inherited::FCharacteristic );
			if( inherited::BigEndian )
				inherited::FCharacteristic->setValueBE( inherited::InitialValue );

			else
				inherited::FCharacteristic->setValueLE( inherited::InitialValue );

			inherited::OutputPin.SetValue( inherited::InitialValue, false );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<typename T, typename T_CHARACT> class ArduinoBluetoothLETypedReadCharacteristic : public ArduinoBluetoothLEBasicCharacteristic<T_CHARACT>
	{
		typedef ArduinoBluetoothLEBasicCharacteristic<T_CHARACT> inherited;

	public:
		OpenWire::SinkPin	InputPin;

	public:
		T		InitialValue;
		bool	Notify : 1;
		bool	BigEndian : 1;

	protected:
		virtual void SystemInit() override
		{
			inherited::SystemInit();
			inherited::FCharacteristic = new T_CHARACT( inherited::UUID.c_str(), Notify ? ( BLERead | BLENotify ) : BLERead );
			inherited::FOwner.FOwner.FPeripheral.addAttribute( *inherited::FCharacteristic );
			if( BigEndian )
				inherited::FCharacteristic->setValueBE( InitialValue );

			else
				inherited::FCharacteristic->setValueLE( InitialValue );

		}

	protected:
		void DoDataReceive( void *_Data )
		{
			T AValue = *(T*)_Data;
			if( AValue != inherited::FCharacteristic->value() )
			{
				if( BigEndian )
					inherited::FCharacteristic->setValueBE( AValue );

				else
					inherited::FCharacteristic->setValueLE( AValue );
			}
		}

	public:
		ArduinoBluetoothLETypedReadCharacteristic( ArduinoBluetoothLEService &AOwner ) :
			inherited( AOwner ),
			Notify( false ),
			BigEndian( false )
		{
			InputPin.SetCallback( MAKE_CALLBACK( ArduinoBluetoothLETypedReadCharacteristic::DoDataReceive ));
		}

	};
//---------------------------------------------------------------------------
	template<typename T, typename T_CHARACT> class ArduinoBluetoothLETypedReadWriteCharacteristic : public ArduinoBluetoothLETypedBasicWriteCharacteristic<T, T_CHARACT>
	{
		typedef ArduinoBluetoothLETypedBasicWriteCharacteristic<T, T_CHARACT> inherited;

	public:
		OpenWire::SinkPin	InputPin;

	protected:
		bool	Notify = false;

	protected:
		void DoDataReceive( void *_Data )
		{
			T AValue = *(T*)_Data;
			if( AValue != inherited::FCharacteristic->value() )
			{
				if( inherited::BigEndian )
					inherited::FCharacteristic->setValueBE( AValue );

				else
					inherited::FCharacteristic->setValueLE( AValue );
			}
		}

	protected:
		virtual void SystemInit() override
		{
			inherited::SystemInit();
			inherited::FCharacteristic = new T_CHARACT( inherited::UUID.c_str(), Notify ? ( BLERead | BLENotify | BLEWrite ) : BLERead | BLEWrite );
			inherited::FOwner.FOwner.FPeripheral.addAttribute( *inherited::FCharacteristic );
			if( inherited::BigEndian )
				inherited::FCharacteristic->setValueBE( inherited::InitialValue );

			else
				inherited::FCharacteristic->setValueLE( inherited::InitialValue );

			inherited::OutputPin.SetValue( inherited::InitialValue, false );
		}

	public:
		ArduinoBluetoothLETypedReadWriteCharacteristic( ArduinoBluetoothLEService &AOwner ) :
			inherited( AOwner )
		{
			InputPin.SetCallback( MAKE_CALLBACK( ArduinoBluetoothLETypedReadWriteCharacteristic::DoDataReceive ));
		}

	};
//---------------------------------------------------------------------------
	class ArduinoBluetoothLEBinaryBasicWriteCharacteristic : public ArduinoBluetoothLEBasicCharacteristic<BLECharacteristic>, public ArduinoBlutoothUpdateIntf
	{
		typedef ArduinoBluetoothLEBasicCharacteristic<BLECharacteristic> inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		Mitov::Bytes	InitialValue;

	public:
		virtual void UpdateValues()
		{
			if( inherited::FCharacteristic->written() )
				OutputPin.SendValue( Mitov::TDataBlock( inherited::FCharacteristic->valueLength(), inherited::FCharacteristic->value() ) );

		}

	public:
		ArduinoBluetoothLEBinaryBasicWriteCharacteristic( ArduinoBluetoothLEService &AOwner ) :
			inherited( AOwner )
		{
			AOwner.FOwner.RegisterUpdateElement( this );
		}
	};
//---------------------------------------------------------------------------
	template<int C_MAX_SIZE> class ArduinoBluetoothLEBinaryWriteCharacteristic : public ArduinoBluetoothLEBinaryBasicWriteCharacteristic
	{
		typedef ArduinoBluetoothLEBinaryBasicWriteCharacteristic inherited;

	protected:
		virtual void SystemInit() override
		{
			inherited::SystemInit();
			inherited::FCharacteristic = new BLECharacteristic( inherited::UUID.c_str(), BLEWrite, C_MAX_SIZE );
			inherited::FOwner.FOwner.FPeripheral.addAttribute( *inherited::FCharacteristic );
			inherited::FCharacteristic->setValue( inherited::InitialValue._Bytes, inherited::InitialValue._BytesSize );
//			inherited::OutputPin.SendValue( Mitov::TDataBlock( inherited::InitialValue._BytesSize, inherited::InitialValue._Bytes ) );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<int C_MAX_SIZE> class ArduinoBluetoothLEBinaryReadCharacteristic : public ArduinoBluetoothLEBasicCharacteristic<BLECharacteristic>
	{
		typedef ArduinoBluetoothLEBasicCharacteristic<BLECharacteristic> inherited;

	public:
		OpenWire::SinkPin	InputPin;

	public:
		Mitov::Bytes	InitialValue;
		bool	Notify = false;

	protected:
		virtual void SystemInit() override
		{
			inherited::SystemInit();
			inherited::FCharacteristic = new BLECharacteristic( inherited::UUID.c_str(), Notify ? ( BLERead | BLENotify ) : BLERead, C_MAX_SIZE );
			inherited::FOwner.FOwner.FPeripheral.addAttribute( *inherited::FCharacteristic );
			inherited::FCharacteristic->setValue( InitialValue._Bytes, InitialValue._BytesSize );
		}

	protected:
		void DoDataReceive( void *_Data )
		{
			Mitov::TDataBlock ABlock = *(Mitov::TDataBlock *)_Data;
			if( ABlock.Size == inherited::FCharacteristic->valueLength() )
				if( memcmp( ABlock.Data, inherited::FCharacteristic->value(), ABlock.Size ) == 0 )
					return;

			inherited::FCharacteristic->setValue( ABlock.Data, ABlock.Size );
		}

	public:
		ArduinoBluetoothLEBinaryReadCharacteristic( ArduinoBluetoothLEService &AOwner ) :
			inherited( AOwner )
		{
			InputPin.SetCallback( MAKE_CALLBACK( ArduinoBluetoothLEBinaryReadCharacteristic::DoDataReceive ));
		}
	};
//---------------------------------------------------------------------------
	template<int C_MAX_SIZE> class ArduinoBluetoothLEBinaryReadWriteCharacteristic : public ArduinoBluetoothLEBinaryBasicWriteCharacteristic
	{
		typedef ArduinoBluetoothLEBinaryBasicWriteCharacteristic inherited;

	public:
		OpenWire::SinkPin	InputPin;

	protected:
		bool	Notify = false;

	protected:
		void DoDataReceive( void *_Data )
		{
			Mitov::TDataBlock ABlock = *(Mitov::TDataBlock *)_Data;
			if( ABlock.Size == inherited::FCharacteristic->valueLength() )
				if( memcmp( ABlock.Data, inherited::FCharacteristic->value(), ABlock.Size ) == 0 )
					return;

			inherited::FCharacteristic->setValue( ABlock.Data, ABlock.Size );
		}

	protected:
		virtual void SystemInit() override
		{
			inherited::SystemInit();
			inherited::FCharacteristic = new BLECharacteristic( inherited::UUID.c_str(), Notify ? ( BLERead | BLENotify | BLEWrite ) : BLERead | BLEWrite, C_MAX_SIZE );
			inherited::FOwner.FOwner.FPeripheral.addAttribute( *inherited::FCharacteristic );
			inherited::FCharacteristic->setValue( inherited::InitialValue._Bytes, inherited::InitialValue._BytesSize );
//			inherited::OutputPin.SetValue( inherited::InitialValue, false );
		}

	public:
		ArduinoBluetoothLEBinaryReadWriteCharacteristic( ArduinoBluetoothLEService &AOwner ) :
			inherited( AOwner )
		{
			InputPin.SetCallback( MAKE_CALLBACK( ArduinoBluetoothLEBinaryReadWriteCharacteristic::DoDataReceive ));
		}
	};
//---------------------------------------------------------------------------
	class ArduinoBluetoothLETextBasicWriteCharacteristic : public ArduinoBluetoothLEBasicCharacteristic<BLECharacteristic>, public ArduinoBlutoothUpdateIntf
	{
		typedef ArduinoBluetoothLEBasicCharacteristic<BLECharacteristic> inherited;

	public:
		OpenWire::SourcePin	OutputPin;

	public:
		String	InitialValue;

	public:
		virtual void UpdateValues()
		{
			if( inherited::FCharacteristic->written() )
			{
				OutputPin.SendValue<String>( StringEx( (const char*)inherited::FCharacteristic->value(), inherited::FCharacteristic->valueLength() ) );
			}
		}

	public:
		ArduinoBluetoothLETextBasicWriteCharacteristic( ArduinoBluetoothLEService &AOwner ) :
			inherited( AOwner )
		{
			AOwner.FOwner.RegisterUpdateElement( this );
		}
	};
//---------------------------------------------------------------------------
	template<int C_MAX_SIZE> class ArduinoBluetoothLETextWriteCharacteristic : public ArduinoBluetoothLETextBasicWriteCharacteristic
	{
		typedef ArduinoBluetoothLETextBasicWriteCharacteristic inherited;

	protected:
		virtual void SystemInit() override
		{
			inherited::SystemInit();
			inherited::FCharacteristic = new BLECharacteristic( inherited::UUID.c_str(), BLEWrite, C_MAX_SIZE );
			inherited::FOwner.FOwner.FPeripheral.addAttribute( *inherited::FCharacteristic );
			inherited::FCharacteristic->setValue( (unsigned char *)inherited::InitialValue.c_str(), inherited::InitialValue.length() );
			inherited::OutputPin.SendValue<String>( StringEx( (const char*)inherited::FCharacteristic->value(), inherited::FCharacteristic->valueLength() ) );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	template<int C_MAX_SIZE> class ArduinoBluetoothLETextReadCharacteristic : public ArduinoBluetoothLEBasicCharacteristic<BLECharacteristic>
	{
		typedef ArduinoBluetoothLEBasicCharacteristic<BLECharacteristic> inherited;

	public:
		OpenWire::SinkPin	InputPin;

	public:
		String	InitialValue;
		bool	Notify = false;

	protected:
		virtual void SystemInit() override
		{
			inherited::SystemInit();
			inherited::FCharacteristic = new BLECharacteristic( inherited::UUID.c_str(), Notify ? ( BLERead | BLENotify ) : BLERead, C_MAX_SIZE );
			inherited::FOwner.FOwner.FPeripheral.addAttribute( *inherited::FCharacteristic );
			inherited::FCharacteristic->setValue( (unsigned char *)InitialValue.c_str(), InitialValue.length() );
		}

	protected:
		void DoDataReceive( void *_Data )
		{
			String AData = (char *)_Data;
			if( AData.length() == inherited::FCharacteristic->valueLength() )
				if( memcmp( AData.c_str(), inherited::FCharacteristic->value(), AData.length() ) == 0 )
					return;

			inherited::FCharacteristic->setValue( (unsigned char *)AData.c_str(), AData.length() );
		}

	public:
		ArduinoBluetoothLETextReadCharacteristic( ArduinoBluetoothLEService &AOwner ) :
			inherited( AOwner )
		{
			InputPin.SetCallback( MAKE_CALLBACK( ArduinoBluetoothLETextReadCharacteristic::DoDataReceive ));
		}
	};
//---------------------------------------------------------------------------
	template<int C_MAX_SIZE> class ArduinoBluetoothLETextReadWriteCharacteristic : public ArduinoBluetoothLETextBasicWriteCharacteristic
	{
		typedef ArduinoBluetoothLETextBasicWriteCharacteristic inherited;

	public:
		OpenWire::SinkPin	InputPin;

	protected:
		bool	Notify = false;

	protected:
		void DoDataReceive( void *_Data )
		{
			String AData = (char *)_Data;
			if( AData.length() == inherited::FCharacteristic->valueLength() )
				if( memcmp( AData.c_str(), inherited::FCharacteristic->value(), AData.length() ) == 0 )
					return;

			inherited::FCharacteristic->setValue( (unsigned char *)AData.c_str(), AData.length() );
		}

	protected:
		virtual void SystemInit() override
		{
			inherited::SystemInit();
			inherited::FCharacteristic = new BLECharacteristic( inherited::UUID.c_str(), Notify ? ( BLERead | BLENotify | BLEWrite ) : BLERead | BLEWrite, C_MAX_SIZE );
			inherited::FOwner.FOwner.FPeripheral.addAttribute( *inherited::FCharacteristic );
			inherited::FCharacteristic->setValue( (unsigned char *)inherited::InitialValue.c_str(), inherited::InitialValue.length() );
			inherited::OutputPin.SendValue<String>( StringEx( (const char*)inherited::FCharacteristic->value(), inherited::FCharacteristic->valueLength() ) );
		}

	public:
		ArduinoBluetoothLETextReadWriteCharacteristic( ArduinoBluetoothLEService &AOwner ) :
			inherited( AOwner )
		{
			InputPin.SetCallback( MAKE_CALLBACK( ArduinoBluetoothLETextReadWriteCharacteristic::DoDataReceive ));
		}
	};
//---------------------------------------------------------------------------
}

#endif
