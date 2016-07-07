//z 2016-07-07 10:04:18 L.177'50142 T4144752563.K.F2429863289

If Not WScript.Arguments.Named.Exists("elevate") Then
  CreateObject("Shell.Application").ShellExecute WScript.FullName _
    , WScript.ScriptFullName & " /elevate", "", "runas", 1
  WScript.Quit
End If

Set WshShell = WScript.CreateObject("WScript.Shell")
 If WScript.Arguments.length = 0 Then
 Set ObjShell = CreateObject("Shell.Application")
 ObjShell.ShellExecute "wscript.exe", """" & _
 WScript.ScriptFullName & """" &_
 " RunAsAdministrator", , "runas", 1
 Wscript.Quit
End if
