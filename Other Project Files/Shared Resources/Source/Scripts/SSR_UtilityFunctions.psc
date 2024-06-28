Scriptname SSR_UtilityFunctions Hidden

;Returns the version of the DLL. Format is: [0] -> Major, [1] -> Minor, [2] -> Patch
Int[] Function GetVersion() Global Native
Int Function GetMajorVersion() Global Native
Int Function GetMinorVersion() Global Native 
Int Function GetPatchVersion() Global Native

;Returns true if the provided minimum version is lower than or equal to the DLL's version
Bool Function MeetsMinimumVersion(Int a_minMajor, Int a_minMinor, Int a_minPatch) Global Native

;Removes and (optionally) re-adds all specified spells to the player.
Function RefreshAbilities(Spell[] a_spells, Bool a_add = True) Global Native
;Removes and (optionally) re-adds all specified perks to the player.
Function RefreshPerks(Perk[] a_perks, Bool a_add = True) Global Native