Scriptname ARM_RefAliasLoadGame extends ReferenceAlias  
{Detects when the player loads the game and pings the main script.}

Event OnPlayerLoadGame()

	(GetOwningQuest() As ARM_QuestMaitenanceScript).PlayerLoadedGame()
EndEvent
