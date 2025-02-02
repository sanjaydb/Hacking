/*++

Module Name:

    public.h

Abstract:

    This module contains the common declarations shared by driver
    and user applications.

Environment:

    user and kernel

--*/

//
// Define an Interface Guid so that app can find the device and talk to it.
//

DEFINE_GUID (GUID_DEVINTERFACE_Driver,
    0xfb1356d3,0x91d2,0x49d7,0x97,0x79,0x52,0x0d,0xa0,0xfa,0x9f,0x43);
// {fb1356d3-91d2-49d7-9779-520da0fa9f43}
