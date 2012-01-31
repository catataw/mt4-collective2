

!define APP_NAME "C2/MT4 Connector"
!define COMP_NAME "OpenThinkingSystems, LLC"
!define VERSION "3.00.00.00"
!define COPYRIGHT "OpenThinkingSystems, LLC © 2010"
!define DESCRIPTION "Application"
!define INSTALLER_NAME "C:\Projects\collective2mt4\Installer\Setup.exe"
!define MAIN_APP_EXE "TradeMonitorC2-Full.ex4"
!define INSTALL_TYPE "SetShellVarContext all"
!define LICENSE_TXT "C:\Projects\collective2mt4\Installer\license.rtf"
!define REG_ROOT "HKLM"
!define REG_APP_PATH "Software\Microsoft\Windows\CurrentVersion\App Paths\${MAIN_APP_EXE}"
!define UNINSTALL_PATH "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}"


######################################################################

VIProductVersion  "${VERSION}"
VIAddVersionKey "ProductName"  "${APP_NAME}"
VIAddVersionKey "CompanyName"  "${COMP_NAME}"
VIAddVersionKey "LegalCopyright"  "${COPYRIGHT}"
VIAddVersionKey "FileDescription"  "${DESCRIPTION}"
VIAddVersionKey "FileVersion"  "${VERSION}"

######################################################################

SetCompressor ZLIB
Name "${APP_NAME}"
Caption "${APP_NAME}"
OutFile "${INSTALLER_NAME}"
BrandingText "${APP_NAME}"
XPStyle on
InstallDir "$PROGRAMFILES\MFS"

######################################################################

!include "MUI.nsh"
!include "nsDialogs.nsh"
!include "LogicLib.nsh"
!include "winmessages.nsh"


!define MUI_ABORTWARNING
!define MUI_UNABORTWARNING

!insertmacro MUI_PAGE_WELCOME
Page custom MetaTrader MetaTraderLeave

Var Dialog
Var Cus_Label
Var Label
Var Cus_Text
Var Cus_Button
Var SelDir
Var Ch1
Var Ch2
Var ChFlg
Var Txt
Var trunc
Var loc
#Var Paths
Var drv
Var Fl
Var Fl3
Var DEST3
Var Checkbox1
Var Checkbox2
Var Checkbox3
Var Checkbox4
Var Checkbox5
Var Flg1
Var Flg2
Var Flg3
Var Flg4
Var Flg5
Var Checked



!macro CallFindFiles DIR FILE CBFUNC
       Push "${DIR}"
       Push "${FILE}"
       Push $0
       GetFunctionAddress $0 "${CBFUNC}"
       Exch $0
       Call FindFiles
!macroend

Function .onInit
	StrCpy $Flg1 0
	StrCpy $Flg2 0
	StrCpy $Flg3 0
	StrCpy $Flg4 0
	StrCpy $Flg5 0
	StrCpy $Checked 0
	StrCpy $SelDir $ProgramFiles
	StrCpy $ChFlg 0
	StrCpy $Txt 0

FunctionEnd


Function MetaTrader
       Delete "$INSTDIR\Location_Selected.ini"
       StrCpy $Flg1 0
       StrCpy $Flg2 0
       StrCpy $Flg3 0
       StrCpy $Flg4 0
       StrCpy $Flg5 0
       StrCpy $Checked 0
       StrCpy $SelDir $ProgramFiles
       StrCpy $ChFlg 0
       StrCpy $Txt 0
       Banner::show /NOUNLOAD /set 76 "Searching..." "Finding MT4."
       Banner::getWindow
	   Pop $1
       Call IsMetaTraderInstalled
       
       Pop $R0   ;$R0 at this point is "1" or "0", "1" installed and "0" for not installed
       
       ${If} $R0 == 0
             MessageBox MB_OK "MetaTrader is not installed.  Please download and install MetaTrader 4 from http://www.metaquotes.net"
             Quit
       ${EndIf}
       
       GetDlgItem $R2 $HWNDPARENT 1
       EnableWindow $R2 0
       
       ${If} $R0 == 1
              nsDialogs::Create /NOUNLOAD 1018
	      Pop $Dialog

	      ${If} $Dialog == error
	            Abort
	      ${EndIf}

	      SetOutPath "$INSTDIR"
              File "C:\Projects\collective2mt4\Installer\Location.ini"
              File "C:\Projects\collective2mt4\Installer\Location_Selected.ini"
              StrCpy $Fl 1
              ReadEnvStr $drv WINDIR
              StrCpy $drv "$drv" -8
              #Macro to Check MetaTrader
	      !insertmacro CallFindFiles $drv Accelerator.mqt FindMT
	      Banner::destroy
              ${NSD_CreateLabel} 0 0 100% 12u "Instance of MetaTrader is installed at:"
              StrCpy $Fl3 1
              StrCpy $loc 30
              #Display CheckBoxes
              Loop:
                   ReadINIStr $DEST3 $INSTDIR\Location.ini PATHS "Location$Fl3"
                   StrCmp $DEST3 "" Done Continue
                   Continue:
                            ${If} $Fl3 == 1
                                  ${NSD_CreateCheckbox} 30 $loc 100% 12u "$DEST3"
                                  Pop $Checkbox1
                                  GetFunctionAddress $0 OnCheckbox1
	                          nsDialogs::OnClick /NOUNLOAD $Checkbox1 $0
                            ${EndIf}
                            
                            ${If} $Fl3 == 2
                                  ${NSD_CreateCheckbox} 30 $loc 100% 12u "$DEST3"
                                  Pop $Checkbox2
                                  GetFunctionAddress $0 OnCheckbox2
	                          nsDialogs::OnClick /NOUNLOAD $Checkbox2 $0
                            ${EndIf}
                            
                            ${If} $Fl3 == 3
                                  ${NSD_CreateCheckbox} 30 $loc 100% 12u "$DEST3"
                                  Pop $Checkbox3
                                  GetFunctionAddress $0 OnCheckbox3
	                          nsDialogs::OnClick /NOUNLOAD $Checkbox3 $0
                            ${EndIf}
                            
                            ${If} $Fl3 == 4
                                  ${NSD_CreateCheckbox} 30 $loc 100% 12u "$DEST3"
                                  Pop $Checkbox4
                                  GetFunctionAddress $0 OnCheckbox4
	                          nsDialogs::OnClick /NOUNLOAD $Checkbox4 $0
                            ${EndIf}
                            
                            ${If} $Fl3 == 5
                                  MessageBox MB_ICONEXCLAMATION "More than 4 Metatrader setups are installed. This installer can update upto 4 instances of Metatrader only."
                                  Goto Done
                            ${EndIf}
                            IntOp $loc $loc + 30
                            IntOp $FL3 $Fl3 + 1
                            Goto Loop
              Done:
              

              ${NSD_CreateCheckbox} 30 $loc 3% 12u ""
              Pop $Checkbox5
              GetFunctionAddress $1 OnCheckbox5
	      nsDialogs::OnClick /NOUNLOAD $Checkbox5 $1

              IntOp $loc $loc + 3
              ${NSD_CreateLabel} 46 $loc 19% 12u "Custom Location:"
              Pop $Cus_Label

              ${NSD_CreateText} 135 $loc 30% 12u ""
              Pop $Cus_Text
              ${NSD_OnChange} $Cus_Text nsDialogsTextChange

              ${NSD_CreateButton} 275 $loc 10% 12u "Browse"
              Pop $Cus_Button
              ${NSD_OnClick} $Cus_Button nsDialogsBrowseFolder
              IntOp $loc $loc + 30
              
              ${NSD_CreateLabel} 0 $loc 100% 12u "Click 'Next' to continue"
              Pop $Label
              
              nsDialogs::Show
       ${EndIf}
FunctionEnd

Function nsDialogsBrowseFolder

         nsDialogs::SelectFolderDialog /NOUNLOAD "Select a folder..." $programfiles
         Pop $SelDir
         ${If} $SelDir == "error"
               Return
         ${EndIf}
         ${NSD_SetText} $Cus_Text $SelDir
         ${If} $Txt == 1
                  WriteINIStr $INSTDIR\Location_Selected.ini PATHS "Location5" "$SelDir"
         ${EndIf}
         
FunctionEnd

Function nsDialogsTextChange

         ${If} $Txt == 1
               Pop $Ch1
               ${NSD_GetText} $Cus_Text $Ch2
               WriteINIStr $INSTDIR\Location_Selected.ini PATHS "Location5" "$Ch2"
         ${EndIf}

FunctionEnd

Var DEST_SEL5
Function MetaTraderLeave

         ReadINIStr $DEST_SEL5 $INSTDIR\Location_Selected.ini PATHS "Location5"
        
FunctionEnd


#########################Start: Logic to toggle CheckBox#########################
Var DEST_SEL1
Function OnCheckbox1
	Pop $0
	ReadINIStr $DEST_SEL1 $INSTDIR\Location.ini PATHS "Location1"
	StrCmp $Flg1 0 Change UnChange
	Change:
	      WriteINIStr $INSTDIR\Location_Selected.ini PATHS "Location1" "$DEST_SEL1"
              StrCpy $Flg1 1
              IntOp $Checked $Checked + 1
              Goto Done
        UnChange:
              WriteINIStr $INSTDIR\Location_Selected.ini PATHS "Location1" ""
              StrCpy $Flg1 0
              IntOp $Checked $Checked - 1
              Goto Done
        Done:

        StrCmp $Checked 0 Disable Enable
        MessageBox MB_OK $Checked
        Enable:
              GetDlgItem $R2 $HWNDPARENT 1
              EnableWindow $R2 1
              Goto Done2
        Disable:
              GetDlgItem $R2 $HWNDPARENT 1
              EnableWindow $R2 0
        Done2:
        
FunctionEnd

Var DEST_SEL2
Function OnCheckbox2
	Pop $0
	ReadINIStr $DEST_SEL2 $INSTDIR\Location.ini PATHS "Location2"
	StrCmp $Flg2 0 Change UnChange
	Change:
	      WriteINIStr $INSTDIR\Location_Selected.ini PATHS "Location2" "$DEST_SEL2"
              StrCpy $Flg2 1
              IntOp $Checked $Checked + 1
              Goto Done
        UnChange:
              WriteINIStr $INSTDIR\Location_Selected.ini PATHS "Location2" ""
              StrCpy $Flg2 0
              IntOp $Checked $Checked - 1
              Goto Done
        Done:
        
        StrCmp $Checked 0 Disable Enable
        MessageBox MB_OK $Checked
        Enable:
              GetDlgItem $R2 $HWNDPARENT 1
              EnableWindow $R2 1
              Goto Done2
        Disable:
              GetDlgItem $R2 $HWNDPARENT 1
              EnableWindow $R2 0
        Done2:
        
FunctionEnd

Var DEST_SEL3
Function OnCheckbox3
	Pop $0
	ReadINIStr $DEST_SEL3 $INSTDIR\Location.ini PATHS "Location3"
	StrCmp $Flg3 0 Change UnChange
	Change:
	      WriteINIStr $INSTDIR\Location_Selected.ini PATHS "Location3" "$DEST_SEL3"
              StrCpy $Flg3 1
              IntOp $Checked $Checked + 1
              Goto Done
        UnChange:
              WriteINIStr $INSTDIR\Location_Selected.ini PATHS "Location3" ""
              StrCpy $Flg3 0
              IntOp $Checked $Checked - 1
              Goto Done
        Done:
        
        StrCmp $Checked 0 Disable Enable
        MessageBox MB_OK $Checked
        Enable:
              GetDlgItem $R2 $HWNDPARENT 1
              EnableWindow $R2 1
              Goto Done2
        Disable:
              GetDlgItem $R2 $HWNDPARENT 1
              EnableWindow $R2 0
        Done2:
        
FunctionEnd

Var DEST_SEL4
Function OnCheckbox4
	Pop $0
	ReadINIStr $DEST_SEL4 $INSTDIR\Location.ini PATHS "Location4"
	StrCmp $Flg4 0 Change UnChange
	Change:
	      WriteINIStr $INSTDIR\Location_Selected.ini PATHS "Location4" "$DEST_SEL4"
              StrCpy $Flg4 1
              IntOp $Checked $Checked + 1
              Goto Done
        UnChange:
              WriteINIStr $INSTDIR\Location_Selected.ini PATHS "Location4" ""
              StrCpy $Flg4 0
              IntOp $Checked $Checked - 1
              Goto Done
        Done:
        
        StrCmp $Checked 0 Disable Enable
        MessageBox MB_OK $Checked
        Enable:
              GetDlgItem $R2 $HWNDPARENT 1
              EnableWindow $R2 1
              Goto Done2
        Disable:
              GetDlgItem $R2 $HWNDPARENT 1
              EnableWindow $R2 0
        Done2:
        
FunctionEnd

Function OnCheckbox5
	Pop $0
	StrCmp $Flg5 0 Change UnChange
	Change:
              ${NSD_SetText} $Cus_Text ""
              #WriteINIStr $INSTDIR\Location_Selected.ini PATHS "Location5" "$ProgramFiles"
              StrCpy $Flg5 1
              StrCpy $Txt 1
              IntOp $Checked $Checked + 1
              Goto Done
        UnChange:
              ${NSD_SetText} $Cus_Text ""
              WriteINIStr $INSTDIR\Location_Selected.ini PATHS "Location5" ""
              StrCpy $Flg5 0
              StrCpy $Txt 0
              StrCpy $ChFlg 0
              IntOp $Checked $Checked - 1
              Goto Done
        Done:
        
        StrCmp $Checked 0 Disable Enable
        Enable:
              GetDlgItem $R2 $HWNDPARENT 1
              EnableWindow $R2 1
              Goto Done2
        Disable:
              GetDlgItem $R2 $HWNDPARENT 1
              EnableWindow $R2 0
        Done2:
        
FunctionEnd

#########################Start: Logic to toggle CheckBox#########################

#########################Start: Logic to Search File/Folder#########################

Function FindFiles
  Exch $R5 # callback function
  Exch
  Exch $R4 # file name
  Exch 2
  Exch $R0 # directory
  Push $R1
  Push $R2
  Push $R3
  Push $R6

  Push $R0 # first dir to search

  StrCpy $R3 1

  nextDir:
    Pop $R0
    IntOp $R3 $R3 - 1
    ClearErrors
    FindFirst $R1 $R2 "$R0\*.*"
    nextFile:
      StrCmp $R2 "." gotoNextFile
      StrCmp $R2 ".." gotoNextFile

      StrCmp $R2 $R4 0 isDir
        Push "$R0\$R2"
        Call $R5
        Pop $R6
        StrCmp $R6 "stop" 0 isDir
          loop:
            StrCmp $R3 0 done
            Pop $R0
            IntOp $R3 $R3 - 1
            Goto loop

      isDir:
        IfFileExists "$R0\$R2\*.*" 0 gotoNextFile
          IntOp $R3 $R3 + 1
          Push "$R0\$R2"

  gotoNextFile:
    FindNext $R1 $R2
    IfErrors 0 nextFile

  done:
    FindClose $R1
    StrCmp $R3 0 0 nextDir

  Pop $R6
  Pop $R3
  Pop $R2
  Pop $R1
  Pop $R0
  Pop $R5
  Pop $R4
FunctionEnd
#########################End: Logic to Search File/Folder#########################

###########Start: Logic to Write the Metatrader locations to Location.ini###########
Function FindMT
  Pop $0
  StrCpy $trunc "$0" -26
  WriteINIStr $INSTDIR\Location.ini PATHS "Location$Fl" $trunc
  IntOp $Fl $Fl + 1
  StrCmp "$trunc" "" 0 not
    StrCpy $9 $0
    Push "stop"
    Return
  not:
    Push "go"
FunctionEnd
###########End: Logic to Write the Metatrader locations to Location.ini###########


####################################################################################
# This function will check if Metatrader is installed or not.  Accordingly it will #
# return 1 if present and 0 if absent                                              #
####################################################################################
Function IsMetaTraderInstalled
  Push $R0
  ClearErrors
  ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{3E5CBADD-2E51-47C1-BBE2-B802DB6DA56A}\" "InstallLocation"
  IfErrors lbl_na
    StrCpy $R0 1
  Goto lbl_end
  lbl_na:
    StrCpy $R0 0
  lbl_end:
  Exch $R0
FunctionEnd




!ifdef LICENSE_TXT
!insertmacro MUI_PAGE_LICENSE "${LICENSE_TXT}"
!endif

#Disable the 'Back' Button
!define MUI_PAGE_CUSTOMFUNCTION_SHOW MyFuncDir
#!insertmacro MUI_PAGE_DIRECTORY

Function MyFuncDir
	GetDlgItem $R2 $HWNDPARENT 3
        EnableWindow $R2 0

FunctionEnd

######################################################################

!ifdef REG_START_MENU
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "MFS"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "${REG_ROOT}"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "${UNINSTALL_PATH}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "${REG_START_MENU}"
!insertmacro MUI_PAGE_STARTMENU Application $SM_Folder
!endif

!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM

!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_UNPAGE_FINISH

!insertmacro MUI_LANGUAGE "English"

######################################################################

Section -MainProgram
${INSTALL_TYPE}
SetOverwrite ifnewer

########################Copy the Test.txt File to Locations selected########################
Var /GLOBAL DEST
Var /GLOBAL Fl1


StrCpy $Fl1 1
Loop:
     ReadINIStr $DEST $INSTDIR\Location_Selected.ini PATHS "Location$Fl1"
     StrCmp $Fl1 6 Done Continue
     Continue:
              StrCmp $DEST "" NoInstall Install
              Install:
					  SetOutPath "$DEST"
					  File "C:\Projects\collective2mt4\Installer\Files\TradeMonitorC2-Full.ex4"
					  SetOutPath "$DEST\libraries"
					  File "C:\Projects\collective2mt4\Installer\Files\MT4C2SignalEntry.dll"
                      IntOp $Fl1 $Fl1 + 1
                      Goto Loop
              NoInstall:
                      IntOp $Fl1 $Fl1 + 1
                      Goto Loop
Done:


SectionEnd

######################################################################

Section -Icons_Reg
SetOutPath "$INSTDIR"
# Create the 'Uninstall.exe' under the 'experts folder
WriteUninstaller "$INSTDIR\uninstall.exe"

# Writing the Uninstall Registry Key.  This will create a entry in the Add/Remove Programs Window
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "DisplayName" "${APP_NAME}"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "UninstallString" "$INSTDIR\uninstall.exe"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "DisplayVersion" "${VERSION}"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "Publisher" "${COMP_NAME}"

!ifdef WEB_SITE
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "URLInfoAbout" "${WEB_SITE}"
!endif
SectionEnd

######################################################################

Section "Uninstall"
# Uninstall files
Var /GLOBAL DEST2
Var /GLOBAL Fl2
StrCpy $Fl2 1
Loop:
     ReadINIStr $DEST2 $INSTDIR\Location_Selected.ini PATHS "Location$Fl2"
     StrCmp $Fl2 6 Done Continue
     Continue:
              StrCmp $DEST2 "" NoInstall Install
              Install:
                      Delete "$DEST2\Test.txt"
                      IntOp $Fl2 $Fl2 + 1
                      Goto Loop
              NoInstall:
                      IntOp $Fl2 $Fl2 + 1
                      Goto Loop
Done:

Delete "$INSTDIR\uninstall.exe"
Delete "$INSTDIR\Location.ini"
Delete "$INSTDIR\Location_Selected.ini"
Delete "$INSTDIR\Test.txt"
RmDir "$INSTDIR"
DeleteRegKey ${REG_ROOT} "${UNINSTALL_PATH}"
SectionEnd

######################################################################
Function AdvReplaceInFile
Exch $0 ;file to replace in
Exch
Exch $1 ;number to replace after
Exch
Exch 2
Exch $2 ;replace and onwards
Exch 2
Exch 3
Exch $3 ;replace with
Exch 3
Exch 4
Exch $4 ;to replace
Exch 4
Push $5 ;minus count
Push $6 ;universal
Push $7 ;end string
Push $8 ;left string
Push $9 ;right string
Push $R0 ;file1
Push $R1 ;file2
Push $R2 ;read
Push $R3 ;universal
Push $R4 ;count (onwards)
Push $R5 ;count (after)
Push $R6 ;temp file name
 
  GetTempFileName $R6
  FileOpen $R1 $0 r ;file to search in
  FileOpen $R0 $R6 w ;temp file
   StrLen $R3 $4
   StrCpy $R4 -1
   StrCpy $R5 -1
 
loop_read:
 ClearErrors
 FileRead $R1 $R2 ;read line
 IfErrors exit
 
   StrCpy $5 0
   StrCpy $7 $R2
 
loop_filter:
   IntOp $5 $5 - 1
   StrCpy $6 $7 $R3 $5 ;search
   StrCmp $6 "" file_write2
   StrCmp $6 $4 0 loop_filter
 
StrCpy $8 $7 $5 ;left part
IntOp $6 $5 + $R3
IntCmp $6 0 is0 not0
is0:
StrCpy $9 ""
Goto done
not0:
StrCpy $9 $7 "" $6 ;right part
done:
StrCpy $7 $8$3$9 ;re-join
 
IntOp $R4 $R4 + 1
StrCmp $2 all file_write1
StrCmp $R4 $2 0 file_write2
IntOp $R4 $R4 - 1
 
IntOp $R5 $R5 + 1
StrCmp $1 all file_write1
StrCmp $R5 $1 0 file_write1
IntOp $R5 $R5 - 1
Goto file_write2
 
file_write1:
 FileWrite $R0 $7 ;write modified line
Goto loop_read
 
file_write2:
 FileWrite $R0 $R2 ;write unmodified line
Goto loop_read
 
exit:
  FileClose $R0
  FileClose $R1
 
   SetDetailsPrint none
  Delete $0
  Rename $R6 $0
  Delete $R6
   SetDetailsPrint both
 
Pop $R6
Pop $R5
Pop $R4
Pop $R3
Pop $R2
Pop $R1
Pop $R0
Pop $9
Pop $8
Pop $7
Pop $6
Pop $5
Pop $0
Pop $1
Pop $2
Pop $3
Pop $4
FunctionEnd
