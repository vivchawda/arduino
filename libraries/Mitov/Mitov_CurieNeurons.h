////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2017 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_CURIE_NEURONS_h
#define _MITOV_CURIE_NEURONS_h

#include <Mitov.h>
//#include <CurieNeurons.h>

namespace Mitov
{
	namespace CurieNeuronRegisters
	{
		const int NEURONSIZE = 128;

		volatile uint8_t* PVP_REGISTER_BASE = (volatile uint8_t*)0xB0600000;
		volatile int*	const NM_NCR =      (volatile int*)(PVP_REGISTER_BASE + 0x00);
		volatile int*	const NM_COMP =     (volatile int*)(PVP_REGISTER_BASE + 0x04);
		volatile int*	const NM_LCOMP =    (volatile int*)(PVP_REGISTER_BASE + 0x08);
		volatile int*	const NM_DIST =     (volatile int*)(PVP_REGISTER_BASE + 0x0C);
		volatile int*	const NM_IDX =     	(volatile int*)(PVP_REGISTER_BASE + 0x0C);
		volatile int*	const NM_CAT =     	(volatile int*)(PVP_REGISTER_BASE + 0x10);
		volatile int*	const NM_AIF =     	(volatile int*)(PVP_REGISTER_BASE + 0x14);
		volatile int*	const NM_MINIF =    (volatile int*)(PVP_REGISTER_BASE + 0x18);
		volatile int*	const NM_MAXIF =    (volatile int*)(PVP_REGISTER_BASE + 0x1C);
		volatile int*	const NM_TESTCOMP = (volatile int*)(PVP_REGISTER_BASE + 0x20);
		volatile int*	const NM_TESTCAT =  (volatile int*)(PVP_REGISTER_BASE + 0x24);
		volatile int*	const NM_NID =  	(volatile int*)(PVP_REGISTER_BASE + 0x28);
		volatile int*	const NM_GCR =  	(volatile int*)(PVP_REGISTER_BASE + 0x2C);
		volatile int*	const NM_RSTCHAIN = (volatile int*)(PVP_REGISTER_BASE + 0x30);
		volatile int*	const NM_NSR =  	(volatile int*)(PVP_REGISTER_BASE + 0x34);
		volatile int*	const NM_FORGET =  	(volatile int*)(PVP_REGISTER_BASE + 0x3C);
		volatile int*	const NM_NCOUNT =  	(volatile int*)(PVP_REGISTER_BASE + 0x3C);
	}
//---------------------------------------------------------------------------
	class Arduino101CurieNeurons;
	class Arduino101NeuronTrainRecognizeOperation;
	class Arduino101NeuronItemBasic;
//---------------------------------------------------------------------------
/*
	class Arduino101NeuronBasicFeature
	{
	public:
//		virtual int GetCount() = 0;
//		virtual void GetFeatures( uint8_t *&ABuffer ) = 0;
		virtual void FillFeatures( bool ALastGroup ) = 0;

	public:
		Arduino101NeuronBasicFeature( Arduino101NeuronTrainRecognizeOperation &AOwner );
	};
	//---------------------------------------------------------------------------
	template<int C_NUM_INPUTS> class Arduino101NeuronAnalogFeatures : public Arduino101NeuronBasicFeature
	{
		typedef Arduino101NeuronBasicFeature inherited;

	public:
		OpenWire::ValueSimpleSinkPin<float> InputPins[ C_NUM_INPUTS ];

	public:
/ *
		virtual int GetCount() override
		{
			return 1;
		}

		virtual void GetFeatures( uint8_t *&ABuffer ) override
		{
			float AValue = constrain( InputPin.Value, 0.0, 1.0 ) * 255 + 0.5;
			*ABuffer++ = AValue;
		}
* /
		virtual void FillFeatures( bool ALastGroup ) override
		{
			for( int i = 0; i < C_NUM_INPUTS; ++i )
			{
				float AValue = constrain( InputPins[ i ].Value, 0.0, 1.0 ) * 255 + 0.5;
				if( ALastGroup && ( i == C_NUM_INPUTS - 1 ) )
					*CurieNeuronRegisters::NM_LCOMP = AValue;

				else
					*CurieNeuronRegisters::NM_COMP = AValue;

			}

		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class Arduino101NeuronAnalogSlidingWindowFeature : public OpenWire::Component, public Arduino101NeuronBasicFeature
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;

	public:
		uint8_t	Count = 128;

	protected:
		uint8_t	FIndex;
		uint8_t *FQueue;

	public:
/ *
		virtual int GetCount() override
		{
			return Count;
		}

		virtual void GetFeatures( uint8_t *&ABuffer ) override
		{
			uint8_t	AIndex = FIndex;
			for( int i = 0; i < Count; ++i )
			{
				*ABuffer++ = FQueue[ AIndex ++ ];
				if( AIndex >= Count )
					AIndex = 0;
			}
		}
* /
		virtual void FillFeatures( bool ALastGroup ) override
		{
			uint8_t	AIndex = FIndex;
			for( int i = 0; i < Count - 1; ++i )
			{
				*CurieNeuronRegisters::NM_COMP = FQueue[ AIndex ++ ];
				if( AIndex >= Count )
					AIndex = 0;
			}

			if( ALastGroup )
				*CurieNeuronRegisters::NM_LCOMP = FQueue[ AIndex ];

			else
				*CurieNeuronRegisters::NM_COMP = FQueue[ AIndex ];
		}

	protected:
		void DoDataReceive( void *_Data )
		{

			float AValue = constrain( *(float *)_Data, 0.0, 1.0 ) * 255 + 0.5;
			FQueue[ FIndex ++ ] = AValue;
			if( FIndex >= Count )
				FIndex = 0;

		}

	protected:
		virtual void SystemInit() override
		{
			inherited::SystemInit();
			FQueue = new uint8_t[ Count ];
			memset( FQueue, 0, Count );
			FIndex = 0;
//			FNeurons.Init();
		}

	public:
		Arduino101NeuronAnalogSlidingWindowFeature( Arduino101NeuronTrainRecognizeOperation &AOwner ) :
			Arduino101NeuronBasicFeature( AOwner )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&Arduino101NeuronAnalogSlidingWindowFeature::DoDataReceive );
		}

	};
//---------------------------------------------------------------------------
	class Arduino101NeuronAnalogAutoScallingSlidingWindowFeature : public OpenWire::Component, public Arduino101NeuronBasicFeature
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	InputPin;

	public:
		uint8_t	Count = 128;

	protected:
		uint8_t	FIndex;
		float *FQueue;

	public:
/ *
		virtual int GetCount() override
		{
			return Count;
		}

		virtual void GetFeatures( uint8_t *&ABuffer ) override
		{
			uint8_t	AIndex = FIndex;
			for( int i = 0; i < Count; ++i )
			{
				*ABuffer++ = FQueue[ AIndex ++ ];
				if( AIndex >= Count )
					AIndex = 0;
			}
		}
* /
		virtual void FillFeatures( bool ALastGroup ) override
		{
			float AMin = FQueue[ 0 ];
			float AMax = FQueue[ 0 ];
			for( int i = 1; i < Count - 1; ++i )
			{
				AMin = MitovMin( AMin, FQueue[ i ] );
				AMax = MitovMax( AMax, FQueue[ i ] );
			}

			uint8_t	AIndex = FIndex;
			for( int i = 0; i < Count; ++i )
			{
				float AValue = (( FQueue[ AIndex ++ ] - AMin ) / ( AMax - AMin )) * 255 + 0.5;
				if( ALastGroup && ( i == Count - 1 ) )
					*CurieNeuronRegisters::NM_LCOMP = AValue;

				else
					*CurieNeuronRegisters::NM_COMP = AValue;

				if( AIndex >= Count )
					AIndex = 0;
			}

		}

	protected:
		void DoDataReceive( void *_Data )
		{
			FQueue[ FIndex ++ ] = *(float *)_Data;
			if( FIndex >= Count )
				FIndex = 0;

		}

	protected:
		virtual void SystemInit() override
		{
			inherited::SystemInit();
			FQueue = new float[ Count ];
			memset( FQueue, 0, Count );
			FIndex = 0;
//			FNeurons.Init();
		}

	public:
		Arduino101NeuronAnalogAutoScallingSlidingWindowFeature( Arduino101NeuronTrainRecognizeOperation &AOwner ) :
			Arduino101NeuronBasicFeature( AOwner )
		{
			InputPin.SetCallback( this, (OpenWire::TOnPinReceive)&Arduino101NeuronAnalogAutoScallingSlidingWindowFeature::DoDataReceive );
		}

	};
*/
//---------------------------------------------------------------------------
	class Arduino101CurieNeurons : public OpenWire::Component
	{
		typedef OpenWire::Component inherited;

	public:
		OpenWire::SinkPin	ForgetInputPin;

	public:
//		static CurieNeurons	FNeurons;

	protected:
		virtual void SystemStart() override
		{
			inherited::SystemStart();
//			FNeurons.Init();
			int TempNSR = *CurieNeuronRegisters::NM_NSR;
			*CurieNeuronRegisters::NM_FORGET = 0;
			*CurieNeuronRegisters::NM_NSR = 16;
			for( int i=0; i < CurieNeuronRegisters::NEURONSIZE; i++ ) 
				*CurieNeuronRegisters::NM_TESTCOMP = 0;

			*CurieNeuronRegisters::NM_NSR = TempNSR;
		}

	protected:
		void DoForgetReceive( void *_Data )
		{
			*CurieNeuronRegisters::NM_FORGET = 0;
		}

	public:
		Arduino101CurieNeurons()
		{
			ForgetInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&Arduino101CurieNeurons::DoForgetReceive );
		}

	};
//---------------------------------------------------------------------------
	class Arduino101CurieNeuronsClassifier : public OpenWire::Object
	{
	public:
		bool	IsKNN = false;
		bool	UseInfinityDistance = false;

	public:
		uint8_t FContext;

	public:
		OpenWire::SinkPin	FeaturesInputPin;

	public:
		TValueArray<float>	FFeaturesValue;

	public:
		void FillFeatures()
		{
			int ACount = MitovMin( FFeaturesValue.Size, 128 );
			for( int i = 0; i < ACount; ++i )
			{
				float AValue = constrain( FFeaturesValue.Data[ i ], 0.0, 1.0 ) * 255 + 0.5;
				if( i == ACount - 1 )
					*CurieNeuronRegisters::NM_LCOMP = AValue;

				else
					*CurieNeuronRegisters::NM_COMP = AValue;
			}
		}

		void Forget()
		{
			int ANCount = *CurieNeuronRegisters::NM_NCOUNT;
			int ATempNSR = *CurieNeuronRegisters::NM_NSR;
			*CurieNeuronRegisters::NM_NSR = 0x0010;
			*CurieNeuronRegisters::NM_RSTCHAIN = 0;
			int ACountOtherNeurons = 0;
			for( int i=0; i < ANCount; i++ )
			{
				int Temp = *CurieNeuronRegisters::NM_NCR;
				if( Temp && 0x7F != FContext )
					++ACountOtherNeurons;

				Temp = *CurieNeuronRegisters::NM_CAT; // Move to the next neuron
			}

			if( ! ACountOtherNeurons )
				*CurieNeuronRegisters::NM_FORGET = 0;

			else
			{
				int *ANeuronData = new int[ ACountOtherNeurons * ( 4 + ( 128 / 2 )) ];

				*CurieNeuronRegisters::NM_RSTCHAIN = 0;
				int *ACurrentPtr = ANeuronData;
				for( int i=0; i < ANCount; ++i )
				{
					int ATemp = *CurieNeuronRegisters::NM_NCR;
					if( ATemp && 0x7F == FContext )
						ATemp = *CurieNeuronRegisters::NM_CAT; // Move to the next neuron

					else
					{
						*ACurrentPtr++ = ATemp; // Save context
						uint8_t *AKnowledgePtr = (uint8_t *)ACurrentPtr;
						ACurrentPtr += 64;
						for( int j = 0; j < 128; ++j )
							*AKnowledgePtr++ = *CurieNeuronRegisters::NM_COMP;

						*ACurrentPtr++ = *CurieNeuronRegisters::NM_AIF;
						*ACurrentPtr++ = *CurieNeuronRegisters::NM_MINIF;
						*ACurrentPtr++ = *CurieNeuronRegisters::NM_CAT; // Move to the next neuron
					}
				}

//				*CurieNeuronRegisters::NM_NSR = ATempNSR;
				*CurieNeuronRegisters::NM_FORGET = 0;
//				*CurieNeuronRegisters::NM_NSR = 0x0010;
				*CurieNeuronRegisters::NM_RSTCHAIN = 0;

				ACurrentPtr = ANeuronData;
				for( int i=0; i < ANCount; ++i )
				{
					*CurieNeuronRegisters::NM_NCR = *ACurrentPtr++;
					uint8_t *AKnowledgePtr = (uint8_t *)ACurrentPtr;
					ACurrentPtr += 64;
					for( int j = 0; j < 128; ++j )
						*CurieNeuronRegisters::NM_COMP = *AKnowledgePtr++;

					*CurieNeuronRegisters::NM_AIF = *ACurrentPtr++;
					*CurieNeuronRegisters::NM_MINIF = *ACurrentPtr++;
					*CurieNeuronRegisters::NM_CAT = *ACurrentPtr++; // Move to the next neuron
				}

				delete [] ANeuronData;
			}

			*CurieNeuronRegisters::NM_NSR = ATempNSR; // set the NN back to its calling status
		}

	protected:
		void DoFeaturesReceive( void *_Data )
		{
			FFeaturesValue = *(TArray<float>*)_Data;
		}

	public:
		Arduino101CurieNeuronsClassifier( uint8_t AContext ) :
			FContext( AContext )
		{
			FeaturesInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&Arduino101CurieNeuronsClassifier::DoFeaturesReceive );
		}
	};
//---------------------------------------------------------------------------
	class Arduino101NeuronBasicClockedOperation : public ClockingSupport
	{
		typedef ClockingSupport inherited;

	public:
		Arduino101CurieNeuronsClassifier &FOwner;

	public:
		Arduino101NeuronBasicClockedOperation( Arduino101CurieNeuronsClassifier &AOwner ) :
			FOwner( AOwner )
		{
		}

	};
//---------------------------------------------------------------------------
	class Arduino101NeuronBasicClockedOrStartOperation : public Arduino101NeuronBasicClockedOperation, public OpenWire::Component
	{
		typedef Arduino101NeuronBasicClockedOperation inherited;

	protected:
		virtual void SystemStart() override
		{
			if( ! ClockInputPin.IsConnected() )
				DoClockReceive( nullptr );

//			inherited::SystemStart();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class Arduino101NeuronForgetOperation : public Arduino101NeuronBasicClockedOperation
	{
		typedef Arduino101NeuronBasicClockedOperation inherited;

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			FOwner.Forget();
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class Arduino101NeuronReadKnowledgeOperation : public Arduino101NeuronBasicClockedOperation
	{
		typedef Arduino101NeuronBasicClockedOperation inherited;

	public:
		OpenWire::SourcePin	FeaturesOutputPin;
		OpenWire::SourcePin	CategoryOutputPin;
		OpenWire::SourcePin	InfluenceFieldOutputPin;
		OpenWire::SourcePin	MinInfluenceFieldOutputPin;
		OpenWire::SourcePin	NeuronCompletedOutputPin;
		OpenWire::SourcePin	CompletedOutputPin;

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			int ANCount = *CurieNeuronRegisters::NM_NCOUNT;
			int ATempNSR = *CurieNeuronRegisters::NM_NSR;

			*CurieNeuronRegisters::NM_NSR = 0x0010;
			*CurieNeuronRegisters::NM_RSTCHAIN = 0;

			float *AFeaturesData = new float[ 128 ];

			for( int i=0; i < ANCount; i++ )
			{
				int ATemp = *CurieNeuronRegisters::NM_NCR;
				if( ATemp && 0x7F == FOwner.FContext )
					ATemp = *CurieNeuronRegisters::NM_CAT; // Move to the next neuron

				else
				{
					float *ADataPtr = AFeaturesData;
					for( int j = 0; j < 128; ++j )
						*ADataPtr++ = float( *CurieNeuronRegisters::NM_COMP ) / 255;

					FeaturesOutputPin.SendValue( TArray<float>( 128, AFeaturesData ));
					InfluenceFieldOutputPin.SendValue<uint32_t>( *CurieNeuronRegisters::NM_AIF );
					MinInfluenceFieldOutputPin.SendValue<uint32_t>( *CurieNeuronRegisters::NM_MINIF );
					CategoryOutputPin.SendValue<int32_t>( *CurieNeuronRegisters::NM_CAT );
					NeuronCompletedOutputPin.Notify( nullptr );
				}
			}

			delete [] AFeaturesData;
			*CurieNeuronRegisters::NM_NSR = ATempNSR; // set the NN back to its calling status

			CompletedOutputPin.Notify( nullptr );
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class Arduino101NeuronBasicAddNeuronOperation : public Arduino101NeuronBasicClockedOrStartOperation
	{
		typedef Arduino101NeuronBasicClockedOrStartOperation inherited;

	public:
		using inherited::inherited;

	public:
		int16_t	Category = 1;
		int16_t	MinInfluenceField = 2;
		int16_t	InfluenceField = 0x4000;

	public:
		uint32_t _MaxIndex;

	protected:
		virtual void LoadFeatures() = 0;

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			int ATempNSR = *CurieNeuronRegisters::NM_NSR;
			*CurieNeuronRegisters::NM_NSR = 0x0010;

			*CurieNeuronRegisters::NM_NCR = FOwner.FContext;
			LoadFeatures();

			*CurieNeuronRegisters::NM_AIF = InfluenceField;
			*CurieNeuronRegisters::NM_MINIF = MinInfluenceField;
			*CurieNeuronRegisters::NM_CAT = Category; // Move to the next neuron

			*CurieNeuronRegisters::NM_NSR = ATempNSR; // set the NN back to its calling status
		}
	};
//---------------------------------------------------------------------------
	class Arduino101NeuronAddNeuronOperation : public Arduino101NeuronBasicAddNeuronOperation
	{
		typedef Arduino101NeuronBasicAddNeuronOperation inherited;

	public:
		float *_Values;

	protected:
		virtual void LoadFeatures() override
		{
			for( uint32_t i = 0; i <= MitovMin( _MaxIndex, 127u ); ++i )
				*CurieNeuronRegisters::NM_COMP = _Values[ i ] * 255 + 0.5;
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class Arduino101NeuronAddNeuronOperationConst : public Arduino101NeuronBasicAddNeuronOperation
	{
		typedef Arduino101NeuronBasicAddNeuronOperation inherited;

	public:
		const /*PROGMEM*/ float *_Values;

	protected:
		virtual void LoadFeatures() override
		{
			for( uint32_t i = 0; i <= MitovMin( _MaxIndex, 127u ); ++i )
				*CurieNeuronRegisters::NM_COMP = pgm_read_float( _Values + i ) * 255 + 0.5;
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class Arduino101NeuronLoadKnowledgeOperation : public Arduino101NeuronBasicClockedOrStartOperation
	{
		typedef Arduino101NeuronBasicClockedOrStartOperation inherited;

	public:
		SimpleList<Arduino101NeuronItemBasic*>	FNeurons;

	protected:
		virtual void DoClockReceive( void *_Data ) override;

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class Arduino101NeuronItemBasic
	{
//		typedef ClockingSupport inherited;

	public:
		int16_t	Category = 1;
		int16_t	MinInfluenceField = 2;
		int16_t	InfluenceField = 0x4000;

	public:
		uint32_t _MaxIndex;

//	public:
//		Arduino101NeuronLoadKnowledgeOperation &FOwner;

	public:
		virtual void LoadFeatures() = 0;

	public:
		Arduino101NeuronItemBasic( Arduino101NeuronLoadKnowledgeOperation &AOwner )
//			FOwner( AOwner )
		{
			AOwner.FNeurons.push_back( this );
		}

	};
//---------------------------------------------------------------------------
	class Arduino101NeuronItem : public Arduino101NeuronItemBasic
	{
		typedef Arduino101NeuronItemBasic inherited;

	public:
		float *_Values;

	public:
		virtual void LoadFeatures() override
		{
			for( uint32_t i = 0; i <= MitovMin( _MaxIndex, 127u ); ++i )
				*CurieNeuronRegisters::NM_COMP = _Values[ i ] * 255 + 0.5;
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
	class Arduino101NeuronItemConst : public Arduino101NeuronItemBasic
	{
		typedef Arduino101NeuronItemBasic inherited;

	public:
		const /*PROGMEM*/ float *_Values;

	public:
		virtual void LoadFeatures() override
		{
			for( uint32_t i = 0; i <= MitovMin( _MaxIndex, 127u ); ++i )
				*CurieNeuronRegisters::NM_COMP = pgm_read_float( _Values + i ) * 255 + 0.5;
		}

	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
/*
	class Arduino101NeuronTrainRecognizeOperation : public OpenWire::Object
	{
	public:
		OpenWire::SinkPin	FeaturesInputPin;

	public:
		TValueArray<float>	FFeaturesValue;

//	public:
//		Mitov::SimpleList<Arduino101NeuronBasicFeature *>	FFeatures;

	public:
		Arduino101CurieNeuronsClassifier &FOwner;

/ *
	public:
		inline void RegisterFeature( Arduino101NeuronBasicFeature *AFeature )
		{
			FFeatures.push_back( AFeature );
		}
* /
		void FillFeatures()
		{
			int ACount = FFeaturesValue.Size;
			for( int i = 0; i < ACount; ++i )
			{
				float AValue = constrain( FFeaturesValue.Data[ i ], 0.0, 1.0 ) * 255 + 0.5;
				if( i == ACount - 1 )
					*CurieNeuronRegisters::NM_LCOMP = AValue;

				else
					*CurieNeuronRegisters::NM_COMP = AValue;
			}
		}

	protected:
		void DoFeaturesReceive( void *_Data )
		{
			FFeaturesValue = *(TArray<float>*)_Data;
		}

	public:
		Arduino101NeuronTrainRecognizeOperation( Arduino101CurieNeuronsClassifier &AOwner ) :
			FOwner( AOwner )
		{
			FeaturesInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&Arduino101NeuronTrainRecognizeOperation::DoFeaturesReceive );
		}
		
	};
*/
//---------------------------------------------------------------------------
	class Arduino101CurieTrainNeurons : public OpenWire::Object 
	{
		typedef OpenWire::Object  inherited;

	public:
		OpenWire::ValueSimpleSinkPin<int32_t>	CategoryInputPin;
		OpenWire::SinkPin						TrainInputPin;
		OpenWire::TypedSourcePin<uint32_t>		CountUsedOutputPin;

	public:
		int16_t	Category = 1;
		int16_t	MinInfluenceField = 2;
		int16_t	MaxInfluenceField = 0x4000;

	protected:
		Arduino101CurieNeuronsClassifier &FOwner;

	protected:
		void DoTrainReceive( void *_Data )
		{
/*
			int ACount = 0;
			for( int i = 0; i < FOwner.FFeatures.size(); ++i )
				ACount += FOwner.FFeatures[ i ]->GetCount();

			uint8_t *ABuffer = new uint8_t[ ACount ];
			uint8_t *APtr = ABuffer;
			for( int i = 0; i < FOwner.FFeatures.size(); ++i )
				FOwner.FFeatures[ i ]->GetFeatures( APtr );

			Arduino101CurieNeurons::FNeurons.Learn( ABuffer, ACount, constrain( CategoryInputPin.Value, -32768, 32767 ));

			delete []ABuffer;
*/
			if( FOwner.IsKNN )
				*CurieNeuronRegisters::NM_NSR = *CurieNeuronRegisters::NM_NSR | 0x20;

			else
				*CurieNeuronRegisters::NM_NSR = *CurieNeuronRegisters::NM_NSR & ~0x20;

//			*CurieNeuronRegisters::NM_GCR = FOwner.FContext;
			if( FOwner.UseInfinityDistance )
				*CurieNeuronRegisters::NM_GCR = ( FOwner.FContext & 0x7F ) | 0x80;

			else
				*CurieNeuronRegisters::NM_GCR = FOwner.FContext & 0x7F;

			*CurieNeuronRegisters::NM_MINIF = MinInfluenceField;
			*CurieNeuronRegisters::NM_MAXIF = MaxInfluenceField;

			FOwner.FillFeatures();

//			Serial.println( "TRAIN" );
			*CurieNeuronRegisters::NM_CAT = Category;
		}

	public:
		Arduino101CurieTrainNeurons( Arduino101CurieNeuronsClassifier &AOwner ) :
			FOwner( AOwner )
		{
			TrainInputPin.SetCallback( this, (OpenWire::TOnPinReceive)&Arduino101CurieTrainNeurons::DoTrainReceive );
		}

	};
//---------------------------------------------------------------------------
	class Arduino101CurieRecognizeNeurons : public ClockingSupport
	{
		typedef ClockingSupport  inherited;

	public:
		OpenWire::SourcePin	CategoryOutputPin;
		OpenWire::SourcePin	DistanceOutputPin;
		OpenWire::SourcePin	NeuronIndexOutputPin;
		OpenWire::SourcePin	DegeneratedOutputPin;
		OpenWire::SourcePin	RecognizedOutputPin;
		OpenWire::SourcePin	DisagreenmentOutputPin;
		OpenWire::SourcePin	CountNeuronsOutputPin;

	protected:
		Arduino101CurieNeuronsClassifier &FOwner;

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
/*
			int ACount = 0;
			for( int i = 0; i < FOwner.FFeatures.size(); ++i )
				ACount += FOwner.FFeatures[ i ]->GetCount();

			uint8_t *ABuffer = new uint8_t[ ACount ];
			uint8_t *APtr = ABuffer;
			for( int i = 0; i < FOwner.FFeatures.size(); ++i )
				FOwner.FFeatures[ i ]->GetFeatures( APtr );
*/
			*CurieNeuronRegisters::NM_GCR = FOwner.FContext;

			FOwner.FillFeatures();

			int AResult = *CurieNeuronRegisters::NM_NSR;

//			Arduino101CurieNeurons::FNeurons.Learn( ABuffer, ACount, constrain( CategoryInputPin.Value, -32768, 32767 ));
/*
			int Adistance;
			int ACategory;
			int ANid;
			int AResult = Arduino101CurieNeurons::FNeurons.Classify( ABuffer, ACount, &Adistance, &ACategory, &ANid );
*/
			switch( AResult )
			{
				case 4:
//					CategoryOutputPin.SendValue<int32_t>( ACategory );
					DisagreenmentOutputPin.SendValue( true );
					RecognizedOutputPin.SendValue( true );
					break;

				case 8:
//					CategoryOutputPin.SendValue<int32_t>( ACategory );
					DisagreenmentOutputPin.SendValue( false );
					RecognizedOutputPin.SendValue( true );
					break;

				default:
					DisagreenmentOutputPin.SendValue( false );
					RecognizedOutputPin.SendValue( false );
			}

			uint32_t ANeuronCount = 0;

			for(;;)
			{
				uint32_t Adistance = *CurieNeuronRegisters::NM_DIST;
				if( Adistance == 0xFFFF )
					break;

				int ACategory = *CurieNeuronRegisters::NM_CAT;
				bool ADegenerated = ( ( ACategory & 0x8000 ) != 0 );
				ACategory &= 0x7FFF;

				uint32_t ANeuronId = *CurieNeuronRegisters::NM_NID;

				++ ANeuronCount;
				DegeneratedOutputPin.Notify( &ADegenerated );
				NeuronIndexOutputPin.Notify( &ANeuronId );
				DistanceOutputPin.Notify( &Adistance );
				CategoryOutputPin.SendValue<int32_t>( ACategory );
			}

			CountNeuronsOutputPin.Notify( &ANeuronCount );
//			delete []ABuffer;
		}

	public:
		Arduino101CurieRecognizeNeurons( Arduino101CurieNeuronsClassifier &AOwner ) :
			FOwner( AOwner )
		{
		}
	};
//---------------------------------------------------------------------------
/*
	class Arduino101NeuronForgetOperation : public ClockingSupport
	{
		typedef ClockingSupport  inherited;

	protected:
		virtual void DoClockReceive( void *_Data ) override
		{
			Arduino101CurieNeurons::FNeurons.Forget();
		}
	};
*/
//---------------------------------------------------------------------------
//	CurieNeurons	Arduino101CurieNeurons::FNeurons;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/*
	Arduino101NeuronBasicFeature::Arduino101NeuronBasicFeature( Arduino101NeuronTrainRecognizeOperation &AOwner )
	{
		AOwner.RegisterFeature( this );
	}
*/
//---------------------------------------------------------------------------
	void Arduino101NeuronLoadKnowledgeOperation::DoClockReceive( void *_Data )
	{
		int ATempNSR = *CurieNeuronRegisters::NM_NSR;
		*CurieNeuronRegisters::NM_NSR = 0x0010;

		*CurieNeuronRegisters::NM_NCR = FOwner.FContext;

		for( int i = 0; i < FNeurons.size(); ++i )
		{
			FNeurons[ i ]->LoadFeatures();
			*CurieNeuronRegisters::NM_AIF = FNeurons[ i ]->InfluenceField;
			*CurieNeuronRegisters::NM_MINIF = FNeurons[ i ]->MinInfluenceField;
			*CurieNeuronRegisters::NM_CAT = FNeurons[ i ]->Category; // Move to the next neuron
		}

		*CurieNeuronRegisters::NM_NSR = ATempNSR; // set the NN back to its calling status
	}
//---------------------------------------------------------------------------
}

#endif
