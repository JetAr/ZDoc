/************************************************************************************
Filename    :   Win32_RoomTiny_Main.cpp
Content     :   First-person view test application for Oculus Rift
Created     :   18th Dec 2014
Authors     :   Tom Heath
Copyright   :   Copyright 2012 Oculus, Inc. All Rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*************************************************************************************/
/// This sample is an extension of the 'tilt controlled' sample,
/// but now, in order to not lose so much of the view as you tilt,
/// we will start to lock off the pitch and roll, to half of their
/// values.  This is very interesting, because it supports a perception
/// that it is not you that is rotating, but the world that is doing so.
/// Press '1' to disable the effect, although not advised to go
/// to and fro as its a non-natural transition.
/// When space is held, as in prior sample, to cease movement, we
/// then find the reduced tilt/roll weird, as there is no accompanying
/// acceleration, leading us to believe that the two should act in tandem.  BUT HOW TO TRANSITION???
/// IDEALLY, REALLY NEED TO PRECISELY MATCH WHAT GRAVITY IS DOING, NOT VAGUELY RIGHT AS THIS IS.
/// FURTHER RESEARCH IS TO DOUBLE THE EFFECT, BUT THEN CAP IT, AND ONCE CAPPED, THEN CONTINUE WITH 100% TILT and ROLL

#define   OVR_D3D_VERSION 11
#include "..\Common\Old\Win32_DirectXAppUtil.h" // DirectX
#include "..\Common\Old\Win32_BasicVR.h"  // Basic VR
#include "..\Common\Win32_ControlMethods.h"  // Control code

//-------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hinst, HINSTANCE, LPSTR, int)
{

    BasicVR basicVR(hinst);
    basicVR.Layer[0] = new VRLayer(basicVR.HMD);

    // Main loop
    while (basicVR.HandleMessages())
    {
        basicVR.ActionFromInput();
        ovrTrackingState trackingState = basicVR.Layer[0]->GetEyePoses();

        // Add velocity to camera
        basicVR.MainCam.Pos = XMVectorAdd(basicVR.MainCam.Pos, FindVelocityFromTilt(&basicVR, basicVR.Layer[0], &trackingState));

        // And lets freeze the reorientation, to not overcomplicate the
        // effects contained in the sample.
        basicVR.MainCam.Rot = XMQuaternionIdentity();

        for (int i = 0; i < 2; i++)
        {
            XMVECTOR storedOrientation;
            if (!DIRECTX.Key['1'])
            {
                storedOrientation = ConvertToXM(basicVR.Layer[0]->EyeRenderPose[i].Orientation);
                float proportionOfNormal = 0.5f;// 50:50 of normal tilt/roll/yaw, and only yaw;
                XMFLOAT3 rot = GetEulerAngles(storedOrientation);
                XMVECTOR baseOne = XMQuaternionRotationRollPitchYaw(0,rot.y,0);
                XMVECTOR lerpedQuat = XMQuaternionSlerp(baseOne, storedOrientation, proportionOfNormal);
                basicVR.Layer[0]->EyeRenderPose[i].Orientation.x = XMVectorGetX(lerpedQuat);
                basicVR.Layer[0]->EyeRenderPose[i].Orientation.y = XMVectorGetY(lerpedQuat);
                basicVR.Layer[0]->EyeRenderPose[i].Orientation.z = XMVectorGetZ(lerpedQuat);
                basicVR.Layer[0]->EyeRenderPose[i].Orientation.w = XMVectorGetW(lerpedQuat);
            }

            basicVR.Layer[0]->RenderSceneToEyeBuffer(&basicVR.MainCam, &basicVR.RoomScene, i);

            if (!DIRECTX.Key['1'])
            {
                // Now put it back
                basicVR.Layer[0]->EyeRenderPose[i].Orientation.x = XMVectorGetX(storedOrientation);
                basicVR.Layer[0]->EyeRenderPose[i].Orientation.y = XMVectorGetY(storedOrientation);
                basicVR.Layer[0]->EyeRenderPose[i].Orientation.z = XMVectorGetZ(storedOrientation);
                basicVR.Layer[0]->EyeRenderPose[i].Orientation.w = XMVectorGetW(storedOrientation);
            }
        }

        basicVR.Layer[0]->PrepareLayerHeader();
        basicVR.DistortAndPresent(1);
    }

    return (basicVR.Release(hinst));
}
