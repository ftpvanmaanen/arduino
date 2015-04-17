////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2015 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_PACKET_h
#define _MITOV_PACKET_h

#include <Mitov.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	class BasicPacketElement : public OpenWire::Component
	{
	public:
		virtual	int	 GetSize( bool &Alligned ) { Alligned = true; return 0; }
		virtual	bool GetIsPopulated() { return true; }
		virtual	bool GetIsModified() { return false; }
		virtual	void ClearModified() {}
		virtual	void GetData( unsigned char *ADataStart, unsigned char *&AData, unsigned char &AOffset ) {}
		virtual	bool Expand( unsigned char *AInBuffer, unsigned char *AOutBuffer, int &ASize ) { return false; }
	};
//---------------------------------------------------------------------------
	class BasicPacketSinkElement : public BasicPacketElement
	{
	public:
		OpenWire::SinkPin	InputPin;

	protected:
		virtual void DoReceive( void *_Data ) = 0;

	public:
		BasicPacketSinkElement()
		{
			InputPin.OnReceiveObject = this;
			InputPin.OnReceive = (OpenWire::TOnPinReceive)&BasicPacketSinkElement::DoReceive;
		}

	};
//---------------------------------------------------------------------------
	template<typename T> class BasicTypedPacketSinkElement : public BasicPacketSinkElement
	{
	protected:
		T		FValue;
		bool	FPopulated;
		bool	FModified;

/*
	public:
		T	InitialValue;

		virtual void SystemInit()
		{
			inherited::SystemInit();
			FValue = InitialValue;
		}
*/
	public:
		virtual	int	 GetSize( bool &Alligned ) 
		{ 
			Alligned = true;
			return sizeof( T ) * 8; 
		}

		virtual	bool	GetIsPopulated() 
		{ 
			return FPopulated;
		}

		virtual	bool	GetIsModified() 
		{ 
			return FModified;
		}

		virtual	void ClearModified() 
		{
			FModified = false;
		}

		virtual	void GetData( unsigned char *ADataStart, unsigned char *&AData, unsigned char &AOffset )
		{ 
			if( AOffset )
			{
				++AData;
				AOffset = 0;
			}

			*(T*)AData = FValue;
//			memcpy( AData, &FValue, sizeof( T ));
			AData += sizeof( T );
		}

	protected:
		virtual void DoReceive( void *_Data )
		{
			T AValue = *(T*)_Data;
			FPopulated = true;
			if( AValue == FValue )
				return;

			FValue = AValue;
		}

	public:
		BasicTypedPacketSinkElement() :
			FPopulated( false ),
			FModified( false )
		{
		}

	};
//---------------------------------------------------------------------------
	class BasicMarkerBinaryElement : public BasicPacketElement
	{
	public:
		Mitov::Bytes	Bytes;

	};
//---------------------------------------------------------------------------
	class MarkerBinaryElement : public BasicMarkerBinaryElement
	{
	public:
	public:
		virtual	int	 GetSize( bool &Alligned ) 
		{ 
			Alligned = true;
			return Bytes._BytesSize * 8;
		}

		virtual	void GetData( unsigned char *ADataStart, unsigned char *&AData, unsigned char &AOffset )
		{
			if( Bytes._BytesSize == 0 )
				return;

			if( AOffset )
			{
				++AData;
				AOffset = 0;
			}

			memcpy( AData, Bytes._Bytes, Bytes._BytesSize );
			AData += Bytes._BytesSize;
		}

	};
//---------------------------------------------------------------------------
	class PacketChecksumElement : public BasicPacketElement
	{
	public:
		bool	Enabled;

	public:
		virtual	int	 GetSize( bool &Alligned ) 
		{ 
			if( Enabled )
			{
				Alligned = true;
				return 8;
			}

			return 0;
		}

		virtual	void GetData( unsigned char *ADataStart, unsigned char *&AData, unsigned char &AOffset )
		{
			if( !Enabled )
				return;

			if( AOffset )
			{
				++AData;
				AOffset = 0;
			}

			unsigned char	AChecksum = 0;
			for( ;ADataStart < AData; ++ADataStart )
				AChecksum ^= *ADataStart;

			*AData = AChecksum;
			AData += 1;
		}

	public:
		PacketChecksumElement() :
			Enabled( true )
		{
		}

	};
//---------------------------------------------------------------------------
	class PacketHeadMarkerBinaryElement : public MarkerBinaryElement
	{
	public:
		virtual	int	 GetSize( bool &Alligned ) 
		{ 
			Alligned = true;
			return Bytes._BytesSize * 2 * 8; 
		}

		virtual	void GetData( unsigned char *ADataStart, unsigned char *&AData, unsigned char &AOffset )
		{
			if( Bytes._BytesSize == 0 )
				return;

			if( AOffset )
			{
				++AData;
				AOffset = 0;
			}

			memcpy( AData, Bytes._Bytes, Bytes._BytesSize );
			AData += Bytes._BytesSize;

			memcpy( AData, Bytes._Bytes, Bytes._BytesSize );
			AData += Bytes._BytesSize;
		}

		virtual	bool Expand( unsigned char *AInBuffer, unsigned char *AOutBuffer, int &ASize ) 
		{
			if( Bytes._BytesSize == 0 )
				return false;

//			memcpy( AOutBuffer, AInBuffer, ASize );
//			return true;

			unsigned char *ABytes = Bytes._Bytes;
			int ABytesSize = Bytes._BytesSize;

			int AOutSize = ABytesSize * 2;

			memcpy( AOutBuffer, AInBuffer, AOutSize );
			AOutBuffer += AOutSize;
			AInBuffer += AOutSize;

			int i = AOutSize;
			for( ; i < ASize - ABytesSize * 2 + 1; ++i )
			{
				if( memcmp( AInBuffer, ABytes, ABytesSize ) == 0 )
					if( memcmp( AInBuffer + ABytesSize, ABytes, ABytesSize ) == 0 )
					{
						memcpy( AOutBuffer, AInBuffer, ABytesSize );
						AInBuffer += ABytesSize * 2;

//						Serial.println( "START" );
						AOutBuffer += ABytesSize;
						unsigned char ACount = 0;
						i += ABytesSize;
						for( ; i < ASize - ABytesSize + 1; i += ABytesSize )
						{
							if( memcmp( AInBuffer, ABytes, ABytesSize ) != 0 )
								break;

//							Serial.println( "INC" );
							++ACount;
							if( ACount == 255 )
								break;

							AInBuffer += ABytesSize;
						}

						if( ACount == *ABytes )
						{
							if( ACount > 0 )
							{
//								Serial.println( "DEC" );
								--ACount;
								AInBuffer -= ABytesSize;
								i -= ABytesSize;
							}
						}

						*AOutBuffer++ = ACount;
						AOutSize += ABytesSize + 1;
						continue;
					}

				*AOutBuffer++ = *AInBuffer++;
//Serial.print( "++AOutSize" );
				++AOutSize;
			}

			int ACorrection = ASize - i;
			memcpy( AOutBuffer, AInBuffer, ACorrection );
//			*AOutBuffer = 5;

//			Serial.print( "ADebugCount : " );
//			Serial.println( ADebugCount );

//			Serial.print( "AOutSize : " );
//			Serial.println( AOutSize );

//			Serial.print( "TEST : " );
//			Serial.println( ACorrection );
//			Serial.println( i );

			ASize = AOutSize + ACorrection;
			return true; 
		}

	};
//---------------------------------------------------------------------------
	class PacketIntegerBinaryElement : public BasicTypedPacketSinkElement<int>
	{
	};
//---------------------------------------------------------------------------
	class PacketUnsignedBinaryElement : public BasicTypedPacketSinkElement<unsigned int>
	{
	};
//---------------------------------------------------------------------------
	class PacketAnalogBinaryElement : public BasicTypedPacketSinkElement<float>
	{
	};
//---------------------------------------------------------------------------
	class PacketDigitalBinaryElement : public BasicTypedPacketSinkElement<bool>
	{
	public:
		virtual	int	 GetSize( bool &Alligned ) 
		{ 
			Alligned = false;
			return 1; 
		}

		virtual	void GetData( unsigned char *ADataStart, unsigned char *&AData, unsigned char &AOffset ) 
		{
			*AData &= 0xFF >> ( 8 - AOffset ); // Zero the upper bits
			if( FValue )
				*AData |= 1 << AOffset;	// Set the bit

			++AOffset;
			if( AOffset == 8 )
			{
				AOffset = 0;
				++AData;
			}

//			memcpy( AData, &FValue, sizeof( T ));
//			AData += sizeof( T );
		}
	};
//---------------------------------------------------------------------------
	class Packet : public Mitov::CommonSource
	{
		typedef Mitov::CommonSource inherited;

	public:
		OpenWire::SinkPin	ClockInputPin;

	public:
		Mitov::SimpleObjectList<BasicPacketElement*>	Elements;

		bool	OnlyModified;

		PacketHeadMarkerBinaryElement	HeadMarker;
		PacketChecksumElement			Checksum;

	protected:
		int	FBufferSize;

		unsigned char *FBuffers[ 2 ];
		Mitov::SimpleList<BasicPacketElement*>	FAllElements;

	protected:
		virtual void DoReceiveClock( void *_Data )
		{
//			Serial.println( "++++++" );
			int AElementCount = FAllElements.size();
			if( OnlyModified )
			{
				bool AModified = false;
				for( int i = 0; i < AElementCount; ++i )
					if( FAllElements[ i ]->GetIsModified() )
					{
						AModified = true;
						break;
					}

				if( !AModified )
					return;
			}
			else
			{
				for( int i = 0; i < AElementCount; ++i )
					if( ! FAllElements[ i ]->GetIsPopulated() )
						return;

			}

			unsigned char *ADataPtr = FBuffers[ 0 ];
			unsigned char AOffset = 0;
			for( int i = 0; i < AElementCount; ++i )
			{
				FAllElements[ i ]->GetData( FBuffers[ 0 ], ADataPtr, AOffset );
//				while( AElementSize-- )
//					OutputPin.Notify( AData++ );


			}

			int ASize = FBufferSize;
//			Serial.println( ASize );
			int ABufferIndex = 0;
//			Serial.println( "--------" );
			for( int i = 0; i < AElementCount; ++i )
			{
//				Serial.println( ABufferIndex & 1 );
//				Serial.println( 1 ^ ( ABufferIndex & 1 ) );
				unsigned char *AInBuffer = FBuffers[ ABufferIndex & 1 ];
				unsigned char *AOutBuffer = FBuffers[ 1 ^ ( ABufferIndex & 1 )];
				if( FAllElements[ i ]->Expand( AInBuffer, AOutBuffer, ASize ))
					++ ABufferIndex;
			}

			ADataPtr = FBuffers[ ABufferIndex & 1 ];

//			Serial.println( ASize );
			while( ASize-- )
				OutputPin.Notify( ADataPtr++ );

		}

	protected:
		virtual void SystemClock( unsigned long currentMicros )
		{
			if( !ClockInputPin.IsConnected() )
				DoReceiveClock( NULL );

//			delay( 1000 );
			inherited::SystemClock( currentMicros );
		}

		virtual void SystemStart()
		{
			FBufferSize = 0;
			FAllElements.push_back( &HeadMarker );
			for( int i = 0; i < Elements.size(); ++i )
				FAllElements.push_back( Elements[ i ] );

			FAllElements.push_back( &Checksum );

			for( int i = 0; i < FAllElements.size(); ++i )
			{
				bool AAlligned = false;
				FBufferSize += FAllElements[ i ]->GetSize( AAlligned );
				if( AAlligned )
				{
					FBufferSize = ( FBufferSize + 7 ) / 8;
					FBufferSize *= 8;
				}

			}

			FBufferSize = ( FBufferSize + 7 ) / 8;
			FBuffers[ 0 ] = new unsigned char[ FBufferSize * 2 ];
			FBuffers[ 1 ] = new unsigned char[ FBufferSize * 2 ];
			inherited::SystemStart();
		}

	public:
		Packet() :
			OnlyModified( false )
		{
			ClockInputPin.OnReceiveObject = this;
			ClockInputPin.OnReceive = (OpenWire::TOnPinReceive)&Packet::DoReceiveClock;
		}

		virtual ~Packet()
		{
			delete []FBuffers[ 1 ];
			delete []FBuffers[ 0 ];
		}
	};
//---------------------------------------------------------------------------
}

#endif
