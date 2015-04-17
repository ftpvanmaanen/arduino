////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2015 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MITOV_BASIC_MUX_h
#define _MITOV_BASIC_MUX_h

#include <Mitov.h>

namespace Mitov
{
	template<typename T> class BasicCommonMux : public CommonMultiInput<T>
	{
		typedef CommonMultiInput<T> inherited;

	protected:
		unsigned int	FChannel;

	public:
		unsigned int	InitialChannel;

	public:
		OpenWire::SinkPin	SelectInputPin;

	protected:
		virtual T CalculateOutput() = 0;

	protected:
		virtual void DoReceiveSelect( void *_Data )
		{
			unsigned int AChannel = *(unsigned int *)_Data;
			if( AChannel >= inherited::InputPins.size() )
				AChannel = inherited::InputPins.size() - 1;

			if( FChannel == AChannel )
				return;

			FChannel = AChannel;
			CalculateOutput();
		}

	protected:
		virtual void SystemInit()
		{
			FChannel = InitialChannel;
			inherited::SystemInit();
		}

		virtual void SystemStart()
		{
			inherited::SystemStart();
			CalculateOutput();
		}

	public:
		BasicCommonMux() :
			FChannel( 0 ),
			InitialChannel( 0 )
		{
			SelectInputPin.OnReceiveObject = this;
			SelectInputPin.OnReceive = (OpenWire::TOnPinReceive)&BasicCommonMux::DoReceiveSelect;
		}

	};
//---------------------------------------------------------------------------
	template<typename T> class CommonMux : public BasicCommonMux<T>
	{
		typedef BasicCommonMux<T> inherited;

	protected:
		virtual T CalculateOutput()
		{
			return inherited::InputPins[ inherited::FChannel ].Value;
		}

	};
//---------------------------------------------------------------------------
}

#endif
