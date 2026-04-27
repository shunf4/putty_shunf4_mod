<#

run with:

powershell -ExecutionPolicy Bypass -File query-shortcut-appid.ps1

#>

$shell = New-Object -ComObject Shell.Application
$p = 'D:\work\putty\pterm-copy-to-start-menu-programs.lnk'
$d = Split-Path $p
$n = Split-Path $p -Leaf
$folder = $shell.Namespace($d)
$item = $folder.ParseName($n)
Echo $item.ExtendedProperty("System.AppUserModel.ID")
