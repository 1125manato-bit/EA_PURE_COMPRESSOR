[Setup]
AppName=EA PURE COMPRESSOR
AppVersion=0.0.1
AppPublisher=EMU AUDIO
DefaultDirName={commoncf}\VST3\EA PURE COMPRESSOR.vst3
DisableDirPage=yes
ArchitecturesInstallIn64BitMode=x64
OutputBaseFilename=EA_PURE_COMPRESSOR_Windows_Installer
Compression=lzma
SolidCompression=yes
WizardStyle=modern

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "japanese"; MessagesFile: "compiler:Languages\Japanese.isl"

[Files]
; VST3 Plugin
Source: "..\..\build\EA_PURE_COMPRESSOR_artefacts\Release\VST3\EA PURE COMPRESSOR.vst3\*"; DestDir: "{commoncf}\VST3\EA PURE COMPRESSOR.vst3"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\Uninstall EA PURE COMPRESSOR"; Filename: "{uninstallexe}"
