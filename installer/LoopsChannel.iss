; Loops Channel - Windows installer (Inno Setup 6)
#define MyAppName "Loops Channel"
#define MyAppVersion "1.0.0"
#define MyPublisher "Loops"
#define MyURL "https://loops.co.ke"
#define VstSrc "..\build\LoopsChannel_artefacts\Release\VST3\Loops Channel.vst3"

[Setup]
AppId={{6F1B3C52-8D0A-4E5B-9A57-4C0F5A1D2B77}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyPublisher}
AppPublisherURL={#MyURL}
AppSupportURL={#MyURL}
AppUpdatesURL={#MyURL}
DefaultDirName={commoncf64}\VST3
DisableDirPage=yes
DefaultGroupName={#MyPublisher}
DisableProgramGroupPage=yes
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
PrivilegesRequired=admin
LicenseFile=license.txt
InfoBeforeFile=info.txt
OutputDir=..\dist
OutputBaseFilename=LoopsChannel-Setup-{#MyAppVersion}
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
UninstallDisplayName={#MyAppName}

[Files]
Source: "{#VstSrc}\*"; DestDir: "{commoncf64}\VST3\Loops Channel.vst3"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{commonprograms}\{#MyPublisher}\Loops website"; Filename: "{#MyURL}"

[Messages]
FinishedLabel=Loops Channel was installed to:%n%nC:\Program Files\Common Files\VST3%n%nIn FL Studio open Options > Manage plugins, click "Find installed plugins" (verify plugins), then find Loops Channel under Installed > Effects.

[Code]
procedure CurStepChanged(CurStep: TSetupStep);
var
  ErrorCode: Integer;
begin
  // Opens loops.co.ke automatically as soon as the files are installed
  if CurStep = ssPostInstall then
    ShellExec('open', '{#MyURL}', '', '', SW_SHOWNORMAL, ewNoWait, ErrorCode);
end;
