﻿//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#pragma once

#include "pch.h"
#include "CustomException.g.h"
#include "MainPage.xaml.h"
#include "Microsoft.SDKSamples.Kitchen.h"

namespace WRLInProcessWinRTComponent
{
[Windows::Foundation::Metadata::WebHostHidden]
public ref class CustomException sealed
{
public:
    CustomException();

protected:
    virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
private:
    MainPage^ rootPage;
    void CustomExceptionRun(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
};
}
