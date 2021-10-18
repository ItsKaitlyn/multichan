# Multichan
Updated version of Ikachan with multiplayer included

Features: 50/60fps options, extra window sizes, widescreen, and multiplayer!

## Compilation
Currently, the only way to compile this project is with the Visual Studio solution. (Visual Studio 2019)

# To-Do
If possible, a portable build of this would be appreciated. I'm not sure how to program, really, so I'd need help porting it to something like SDL2.

## Multiplayer Basis
This is based off of the "Ikachan+" mod I made for Ikachan, which adds support for new resolution sizes, as well as 50/60fps.

(60fps wasn't in the original, though).

This also adds widescreen, which has been edited to work with the game.

(The only issues with widescreen were an NPC issue, and some images)

## Multiplayer Input Progress
I need help getting *controller support* functioning, probably.

Something I have been able to do is have 2 ikachans, both with different controls on the keyboard.

However, that is obviously not very comfortable to control. So, I attempted to add the controller support code from CSE2-Accurate.

You can look at it in Gamepad.cpp, and KeyControl.cpp.

## Multiplayer Gameplay-To-Completion Progress
In theory, items give equips to both players. There is only one inventory at the moment, as well.

NPCs *might* be able to detect both players. They prioritize Player 1 if they are nearby, but player 2 if they're the one closest.

The "sleeping npc" does *not* detect both players, so I need to fix that.

## Multiplayer Gameplay-Mechanics Progress
The first thing I want to do is make it so that you only die if *both* players are dead.

If one player dies, they will have to wait 60 seconds to respawn on top of the other player.

At least, that's my idea.

## Multiplayer "mod-ability"
If you're going to mod this version of Ikachan, it's really easy after a post-compiled build is made.

The actual npc code and stuff is pretty much the same, so you can just copy your data folder stuff over.

# Original Game
Decompilation (what this is based off of): https://github.com/cuckydev/Ikachan

Pixel's website: https://studiopixel.jp/archives/index.html

# Releases
https://itskaitlyn.github.io/games/ikachan/

# GOALS

-4 player multiplayer (?)

-Drop-In multiplayer (you can still play the original game?)

-Built-in better controller support.

-New pause menu (like in CSE2 Enhanced).
