Scriptname SSR_QuestMaintenanceQuest extends Quest  
{Performs maintenance when the player loads the game and checks for updates.}

Actor Property PlayerREF Auto
Message Property SSR_MSG_SharedResourcesInstalled Auto
Message Property SSR_MSG_IncompatibleUpdateInstalled Auto
Message Property SSR_MSG_FrameworkUpdating Auto

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

    If (iVersion[0] < 1)

        SSR_MSG_IncompatibleUpdateInstalled.Show()
        Return -1
    ElseIf (iVersion[1] < 0)

        SSR_MSG_FrameworkUpdating.Show()
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

    iVersion = SSR_UtilityFunctions.GetVersion()

    Int iResponse = CheckVersion()
    If (iResponse < 0)

        Return
    EndIf

    AddBackgroundStuff()
    SSR_MSG_SharedResourcesInstalled.Show()
EndEvent