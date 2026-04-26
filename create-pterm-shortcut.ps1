<#
.SYNOPSIS
Creates a pterm shortcut (.lnk) with a custom AppUserModelID.

.DESCRIPTION
This script creates a Windows shortcut for pterm.exe and sets the
System.AppUserModel_ID property on it.

When the shortcut is placed in the Start Menu, Windows uses it to
determine how to "relaunch" the application.  This enables Shift+Click
and middle-click on the taskbar button to spawn a new pterm instance.

.PARAMETER ShortcutDir
    Directory where the .lnk file is created.  Default: current directory.

.PARAMETER ShortcutName
    Name of the shortcut file.  Default: pterm-copy-to-start-menu-programs.lnk.

.PARAMETER TargetPath
    Path to pterm.exe.  Default: .\Release\pterm.exe.

.PARAMETER Arguments
    Command-line arguments for pterm.exe.  Default: empty (default shell). Use \" to escape double quotes in win process command line.

.PARAMETER AppUserModelId
    The AppUserModelID to assign to the shortcut.
    Must match get_app_user_model_id() in windows/pterm.c.
    Default: SimonTatham.Pterm.shunf4-mod

.PARAMETER InstallStartMenu
    Also copy the shortcut to the user's Start Menu (Programs folder).

.EXAMPLE
    # Create shortcut in current directory with default settings
    powershell -ExecutionPolicy Bypass -File create-pterm-shortcut.ps1

.EXAMPLE
    # Create with custom font and install to Start Menu
    powershell -ExecutionPolicy Bypass -File create-pterm-shortcut.ps1 `
        -Arguments '-o "Font=Consolas,14" -o TermWidth=120' `
        -InstallStartMenu

.EXAMPLE
    # Create with -e command
    powershell -ExecutionPolicy Bypass -File create-pterm-shortcut.ps1 `
        -Arguments '-e wsl'

.EXAMPLE
    # Just install to Start Menu with default args
    powershell -ExecutionPolicy Bypass -File create-pterm-shortcut.ps1 `
        -InstallStartMenu
#>

param(
    [string]$ShortcutDir = ".",
    [string]$ShortcutName = "pterm-copy-to-start-menu-programs.lnk",
    [string]$TargetPath = ".\Release\pterm.exe",
    [string]$Arguments = "",
    [string]$AppUserModelId = "SimonTatham.Pterm.shunf4-mod",
    [switch]$InstallStartMenu
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

# --- Resolve paths ---
$TargetPath = [System.IO.Path]::GetFullPath($TargetPath)
if (-not (Test-Path $TargetPath)) {
    Write-Error "Target not found: $TargetPath"
    exit 1
}
echo '====='
echo $Arguments
echo '====='
echo $ShortcutDir
echo '====='
$ShortcutDir = [System.IO.Path]::GetFullPath($ShortcutDir)
if (-not (Test-Path $ShortcutDir)) {
    New-Item -ItemType Directory -Path $ShortcutDir | Out-Null
}
$ShortcutPath = [System.IO.Path]::Combine($ShortcutDir, $ShortcutName)
$WorkingDir = [System.IO.Path]::GetDirectoryName($TargetPath)

# --- Step 1: Create the .lnk file ---
Write-Host "Creating shortcut: $ShortcutPath"
$WshShell = New-Object -ComObject WScript.Shell
$lnk = $WshShell.CreateShortcut($ShortcutPath)
$lnk.TargetPath = $TargetPath
$lnk.Arguments = $Arguments
$lnk.WorkingDirectory = $WorkingDir
$lnk.Description = "pterm - PuTTY-style terminal emulator"
$lnk.Save()
Write-Host "  Target : $TargetPath"
Write-Host "  Args   : $(if ($Arguments) { $Arguments } else { '(none)' })"

# --- Step 2: Set AppUserModelID via IPropertyStore COM interop ---
Add-Type -TypeDefinition @"
using System;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

[StructLayout(LayoutKind.Sequential)]
public struct PROPERTYKEY {
    public Guid fmtid;
    public uint pid;
    public PROPERTYKEY(Guid f, uint p) { fmtid = f; pid = p; }
}

[StructLayout(LayoutKind.Sequential)]
public struct PROPVARIANT {
    public ushort vt;
    public ushort wReserved1;
    public ushort wReserved2;
    public ushort wReserved3;
    public IntPtr pwszVal;
}

[ComImport, Guid("886D8EEB-8CF2-4446-8D02-CDBA1DBDCF99"),
 InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
public interface IPropertyStore {
    [PreserveSig] int GetCount(out uint c);
    [PreserveSig] int GetAt(uint i, out PROPERTYKEY k);
    [PreserveSig] int GetValue(ref PROPERTYKEY k, out PROPVARIANT v);
    [PreserveSig] int SetValue(ref PROPERTYKEY k, ref PROPVARIANT v);
    [PreserveSig] int Commit();
}

public static class ShortcutProperty {

    [DllImport("shell32.dll", CharSet = CharSet.Unicode)]
    static extern int SHGetPropertyStoreFromParsingName(
        string path, IntPtr pbc, int flags, ref Guid riid, out IPropertyStore ppv);

    public static void SetAppId(string path, string appId) {
        var iid = new Guid("886D8EEB-8CF2-4446-8D02-CDBA1DBDCF99");
        var key = new PROPERTYKEY(
            new Guid("9F4C2855-9F79-4B39-A8D0-E1D42DE1D5F3"), 5);

        IPropertyStore store;
        int hr = SHGetPropertyStoreFromParsingName(
            path, IntPtr.Zero, 2, ref iid, out store);
        if (hr != 0) Marshal.ThrowExceptionForHR(hr);
        try {
            var pv = new PROPVARIANT();
            pv.vt = 31; // VT_LPWSTR
            pv.pwszVal = Marshal.StringToCoTaskMemUni(appId);
            try {
                hr = store.SetValue(ref key, ref pv);
                if (hr != 0) Marshal.ThrowExceptionForHR(hr);
                store.Commit();
            } finally {
                Marshal.FreeCoTaskMem(pv.pwszVal);
            }
        } finally {
            Marshal.ReleaseComObject(store);
        }
    }
}
"@ -Language CSharp

Write-Host "  AppID  : $AppUserModelId"
[ShortcutProperty]::SetAppId($ShortcutPath, $AppUserModelId)
Write-Host "  OK."

# --- Step 3 (optional): Install to Start Menu ---
if ($InstallStartMenu) {
    $programs = [System.IO.Path]::Combine(
        [System.Environment]::GetFolderPath('StartMenu'), "Programs")
    if (-not (Test-Path $programs)) {
        New-Item -ItemType Directory -Path $programs | Out-Null
    }
    $dest = [System.IO.Path]::Combine($programs, $ShortcutName)
    Copy-Item -Path $ShortcutPath -Destination $dest -Force
    Write-Host ""
    Write-Host "Installed to Start Menu: $dest"
    Write-Host "Shift+Click or middle-click on the pterm taskbar button"
    Write-Host "should now launch a new instance."
}

Write-Host ""
Write-Host "Done. Shortcut: $ShortcutPath"
if (-not $InstallStartMenu) {
    $programs = [System.IO.Path]::Combine(
        [System.Environment]::GetFolderPath('StartMenu'), "Programs")
    Write-Host ""
    Write-Host "NOTE: For Shift+Click / middle-click to work, the shortcut"
    Write-Host "must be in the Start Menu.  Either re-run with -InstallStartMenu"
    Write-Host "or copy it manually to:"
    Write-Host "  $programs\"
}
