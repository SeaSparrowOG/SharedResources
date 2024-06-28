Scriptname ARM_QuestMaitenanceScript extends Quest
{Maintenance script for Armillary. Resets necessary perks.}

Actor Property PlayerREF Auto
Message Property SSR_MSG_DLLNeedsUpdate Auto
Message Property ARM_Framework_MSG_Updating Auto 
Message Property ARM_Framework_MSG_IncompatibleUpdate Auto
Message Property ARM_Framework_MSG_Installed Auto

Perk[] Property BackgroundPerks Auto
Perk[] Property DeprecatedBackgroundPerks Auto
Spell[] Property BackgroundAbilities Auto
Spell[] Property DeprecatedBackgroundAbilities Auto

Int[] iVersion

;/
=======================================
Background maintenance
=======================================
/;
Function AddBackgroundStuff()

    SSR_UtilityFunctions.RefreshAbilities(BackgroundAbilities)
    SSR_UtilityFunctions.RefreshPerks(BackgroundPerks)

    SSR_UtilityFunctions.RefreshAbilities(DeprecatedBackgroundAbilities, False)
    SSR_UtilityFunctions.RefreshPerks(DeprecatedBackgroundPerks, False)
EndFunction

;/
=======================================
Startup and load detection
=======================================
/;
Int Function CheckVersion()

    Bool bResponse = SSR_UtilityFunctions.MeetsMinimumVersion(1, 0, 0)
    If (!bResponse) 

        SSR_MSG_DLLNeedsUpdate.Show()
        Return -1
    ElseIf (iVersion[0] < 1)

        ARM_Framework_MSG_IncompatibleUpdate.Show()
        Return -1
    ElseIf (iVersion[1] < 0)

        ARM_Framework_MSG_Updating.Show()
        Return 1
    EndIf

    Return 0
EndFunction

Function PlayerLoadedGame()

    If (CheckVersion() == 1)

        Self.Stop()
    EndIf
EndFunction

Event OnInit()

    iVersion = new Int[3]
    iVersion[0] = 1
    iVersion[1] = 0
    iVersion[2] = 0

    Int iResponse = CheckVersion()
    If (iResponse < 0)

        Return
    EndIf

    AddBackgroundStuff()
    ARM_Framework_MSG_Installed.Show()
EndEvent