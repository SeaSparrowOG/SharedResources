Scriptname SSR_RefAliasPlayerLoadListener extends ReferenceAlias  
{Detects when the player loads the game and pings the main quest to do its thing.}


Event OnPlayerLoadGame()

    (GetOwningQuest() As SSR_QuestMaintenanceQuest).PlayerLoadedGame()
EndEvent