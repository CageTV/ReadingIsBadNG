# Reading Is Bad NG

A from-scratch rebuild of the plugin of [Reading Is Bad SKSE](https://www.nexusmods.com/skyrimspecialedition/mods/92524) by ThirdEyeSqueegee (the source was taken down). Cast **Absorb Knowledge** and every book around you is read.

## Install

Install the original mod first (it has the `ReadingIsBad.esp` with the spell, its tome and the placed tome in Dragonsreach), then install this zip over it so `SKSE/Plugins/ReadingIsBad.dll` is replaced. The ESP is not included, so saves made with the original keep working. Same file names and the same `ReadingIsBad.ini` keys (`fSpellRange`, `bSkipSpellTomes`).

## What is different

- Books a quest is tracking are left for you to read, so the quest does not break (`bSkipQuestItems`).
- Optionally reads the books lying inside containers too: chests, barrels, bookshelves (`bIncludeContainers`).
- A limit per cast (`iMaxBooksPerCast`) and a message saying how many were read (`bNotify`).
- Disabled, deleted and already read books are skipped; a book that exists several times is read once.
- The reading happens after the search, because reading a spell tome changes the scene.
- A settings page in the [SKSE Menu Framework](https://www.nexusmods.com/skyrimspecialedition/mods/120352) when it is installed.
- SE and AE, Skyrim 1.5.97 to 1.7.x. No game hooks: it listens to the spell cast event and calls the game's own book function.

## Settings (`Data/SKSE/Plugins/ReadingIsBad.ini`)

| key | default | |
|---|---|---|
| `fSpellRange` | 512 | radius around you, in game units (about 70 units = 1 m) |
| `bSkipSpellTomes` | false | do not learn spell tome spells |
| `bSkipQuestItems` | true | leave quest books alone |
| `bIncludeContainers` | false | also read books inside containers |
| `iMaxBooksPerCast` | 0 | 0 = no limit |
| `bNotify` | true | say how many books were read |

## License

GPL-3.0-or-later (it bundles the SKSE Menu Framework header, GPL-3.0).
