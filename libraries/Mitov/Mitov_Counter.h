////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2015 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_COUNTER_h
#define _MITOV_COUNTER_h

#include <Mitov.h>

namespace Mitov
{
	class BasicCounter : public Mitov::CommonEnableSource
	{
		typedef Mitov::CommonEnableSource inherited;

	public:
		OpenWire::SinkPin	ResetInputPin;

	protected:
		int	FCount;

	protected:
		void DoReceiveReset( void *_Data )
		{
			FCount = InitialValue;
		}

		virtual void SystemStart()
		{
			FCount = InitialValue;

			inherited::SystemStart();
		}

	public:
		int	InitialValue;

	public:
		BasicCounter() :
			FCount( 0 ),
			InitialValue( 0 )
		{
			ResetInputPin.OnReceiveObject = this;
			ResetInputPin.OnReceive = (OpenWire::TOnPinReceive)&BasicCounter::DoReceiveReset;
		}

	};
//---------------------------------------------------------------------------
	class Counter : public Mitov::BasicCounter
	{
		typedef Mitov::BasicCounter inherited;

	public:
		OpenWire::SinkPin	InputPin;

	public:
		bool	Reversed;

	public:
		virtual void DoReceive( void *_Data )
		{
            if( ! Enabled )
				return;

			if( Reversed )
				--FCount;

			else
				++FCount;

			OutputPin.Notify( &FCount );
		}

	public:
		Counter() :
			Reversed( false )
		{
			InputPin.OnReceiveObject = this;
			InputPin.OnReceive = (OpenWire::TOnPinReceive)&Counter::DoReceive;
		}

	};
//---------------------------------------------------------------------------
	class UpDownCounter : public Mitov::BasicCounter
	{
		typedef Mitov::BasicCounter inherited;

	public:
		OpenWire::SinkPin	UpInputPin;
		OpenWire::SinkPin	DownInputPin;

	protected:
		void DoReceiveUp( void *_Data )
		{
            if( ! Enabled )
				return;

			++FCount;
			OutputPin.Notify( &FCount );
		}

		void DoReceiveDown( void *_Data )
		{
            if( ! Enabled )
				return;

			--FCount;
			OutputPin.Notify( &FCount );
		}

	public:
		UpDownCounter()
		{
			UpInputPin.OnReceiveObject = this;
			UpInputPin.OnReceive = (OpenWire::TOnPinReceive)&UpDownCounter::DoReceiveUp;

			DownInputPin.OnReceiveObject = this;
			DownInputPin.OnReceive = (OpenWire::TOnPinReceive)&UpDownCounter::DoReceiveDown;
		}

	};
}

#endif
