

#include "driver.h"
#include "driver.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
#pragma alloc_text (PAGE, DriverEvtDeviceAdd)
#pragma alloc_text (PAGE, DriverEvtDriverContextCleanup)
#endif


NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT  DriverObject,
    _In_ PUNICODE_STRING RegistryPath
    )

{
    WDF_DRIVER_CONFIG config;
    NTSTATUS status;
    WDF_OBJECT_ATTRIBUTES attributes;

    WPP_INIT_TRACING( DriverObject, RegistryPath );

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
    attributes.EvtCleanupCallback = DriverEvtDriverContextCleanup;

    WDF_DRIVER_CONFIG_INIT(&config,
                           DriverEvtDeviceAdd
                           );

    status = WdfDriverCreate(DriverObject,
                             RegistryPath,
                             &attributes,
                             &config,
                             WDF_NO_HANDLE
                             );

    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "WdfDriverCreate failed %!STATUS!", status);
        WPP_CLEANUP(DriverObject);
        return status;
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");

    return status;
}

NTSTATUS
DriverEvtDeviceAdd(
    _In_    WDFDRIVER       Driver,
    _Inout_ PWDFDEVICE_INIT DeviceInit
    )

{
    NTSTATUS status;

    UNREFERENCED_PARAMETER(Driver);

    PAGED_CODE();

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

    status = DriverCreateDevice(DeviceInit);

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");

    return status;
}

VOID
DriverEvtDriverContextCleanup(
    _In_ WDFOBJECT DriverObject
    )
{
    UNREFERENCED_PARAMETER(DriverObject);

    PAGED_CODE ();

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

    WPP_CLEANUP( WdfDriverWdmGetDriverObject( (WDFDRIVER) DriverObject) );

}
