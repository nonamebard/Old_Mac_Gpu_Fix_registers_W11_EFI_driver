// pcie_fix.c
#include <ntddk.h>
#include <wdm.h>
#include <initguid.h>
#include <devguid.h>

// Structure to store path to setpci
WCHAR g_SetpciPath[] = L"\\??\\C:\\bettercamp-main\\fix9400M_EFI\\setpci.exe";
WCHAR g_WorkDir[] = L"\\??\\C:\\bettercamp-main\\fix9400M_EFI";

// Function to launch a process from kernel mode
NTSTATUS RunProcessFromKernel(PWSTR ApplicationPath, PWSTR CommandLine, PWSTR WorkingDirectory) {
    NTSTATUS status;
    UNICODE_STRING appPath, cmdLine, workDir;
    RtlInitUnicodeString(&appPath, ApplicationPath);
    RtlInitUnicodeString(&cmdLine, CommandLine);
    RtlInitUnicodeString(&workDir, WorkingDirectory);
    
    // Structures for process creation
    PUNICODE_STRING pustrAppName = &appPath;
    PUNICODE_STRING pustrCmdLine = &cmdLine;
    
    // Use RtlCreateProcessParametersEx to create process parameters
    PRTL_USER_PROCESS_PARAMETERS ProcessParameters = NULL;
    status = RtlCreateProcessParametersEx(
        &ProcessParameters,
        pustrAppName,
        NULL,
        NULL,
        pustrCmdLine,
        NULL,
        &workDir,
        NULL,
        NULL,
        NULL,
        RTL_USER_PROCESS_PARAMETERS_NORMALIZED
    );
    
    if (!NT_SUCCESS(status)) {
        DbgPrint("PCIE_FIX: Failed to create process parameters: %08x\n", status);
        return status;
    }
    
    // Create the process
    HANDLE hProcess = NULL;
    HANDLE hThread = NULL;
    
    status = PsCreateSystemProcess(
        &hProcess,
        &hThread,
        PROCESS_ALL_ACCESS,
        NULL,
        ProcessParameters
    );
    
    if (NT_SUCCESS(status)) {
        DbgPrint("PCIE_FIX: Process created successfully\n");
        
        // Wait for process completion
        KeWaitForSingleObject(hProcess, Executive, KernelMode, FALSE, NULL);
        
        // Get return code
        PROCESS_BASIC_INFORMATION pbi;
        status = ZwQueryInformationProcess(
            hProcess,
            ProcessBasicInformation,
            &pbi,
            sizeof(pbi),
            NULL
        );
        
        if (NT_SUCCESS(status)) {
            DbgPrint("PCIE_FIX: Process exit code: %d\n", pbi.ExitStatus);
        }
        
        ZwClose(hThread);
        ZwClose(hProcess);
    } else {
        DbgPrint("PCIE_FIX: Failed to create process: %08x\n", status);
    }
    
    RtlDestroyProcessParameters(ProcessParameters);
    return status;
}

// Function that executes when driver loads
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
    UNREFERENCED_PARAMETER(RegistryPath);
    
    DbgPrint("PCIE_FIX: Driver loading - starting PCIe fix for MacBook 9400M\n");
    
    // Form commands
    WCHAR command1[] = L"setpci.exe -G";
    WCHAR command2[] = L"setpci.exe -s \"00:17.0\" 3e.b=8";
    WCHAR command3[] = L"setpci.exe -s \"02:00.0\" 04.b=7";
    
    // First do -G for global access
    DbgPrint("PCIE_FIX: Running: setpci -G\n");
    RunProcessFromKernel(g_SetpciPath, command1, g_WorkDir);
    
    // Small delay
    LARGE_INTEGER delay;
    delay.QuadPart = -500 * 10000; // 500ms
    KeDelayExecutionThread(KernelMode, FALSE, &delay);
    
    // Set first PCIe pin
    DbgPrint("PCIE_FIX: Setting 00:17.0 3e.b=8\n");
    RunProcessFromKernel(g_SetpciPath, command2, g_WorkDir);
    
    delay.QuadPart = -200 * 10000; // 200ms
    KeDelayExecutionThread(KernelMode, FALSE, &delay);
    
    // Set second PCIe pin
    DbgPrint("PCIE_FIX: Setting 02:00.0 04.b=7\n");
    RunProcessFromKernel(g_SetpciPath, command3, g_WorkDir);
    
    DbgPrint("PCIE_FIX: PCIe fix completed\n");
    
    // Driver doesn't need unload, but unload function is optional
    DriverObject->DriverUnload = NULL;
    
    return STATUS_SUCCESS;
}