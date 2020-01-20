XXL Inspector
Version 0.1.1
By AdrienTD

The XXL Inspector is a memory watching/editing/cheating tool for Asterix XXL
games (XXL1/XXL2/OG).

Features include:
 - Load a level or sector at any time.
 - Cheats: Teleport Asterix/Obelix, change the game speed, ...
 - Visualization of the scene graph
 - List of all loaded objects with class and instance names
 - Look inside the memory of a game object
 - Crate detector: displays all the positions of the active crate stacks on
   the screen, along with the number of crates in the stack. Useful for
   finding Tardis strats.

New in v0.1.1:
 - Common:
   - In Selected Object, you can now send events/messages to the object.
 - XXL1:
   - In Cheats, added Crate Randomizer: randomizes the content of crates on
     loading a level.
   - In Cheats, added Disable HUD: hides the HUD for cinematic experience
     (pause and unpause game to enable HUD).
   - In Selected Object > Movable, added Attach camera, to attach the camera
     on the selected node (from the Scene Tree).
     You open the Scene Tree window and select a node, then you open the
     Selected Object window, click Movable, then click Attach camera.
   - In Crate Detector, added "Center on Asterix" to center the crate view on
     Asterix's position.
 - XXL2/OG:
   - Added Trigger Tree window: allows you to explore the triggers in the game
     which is most of the game's scripting.
     A trigger [T] contains two things: a condition and a list of actions.
     The condition [C] is evaluated every frame (?), and if the result is True,
     then the list of actions is executed. An action is just sending an
     event/message to any kind of object (you can also try sending events
     in the Selected Object window).
     A trigger domain [D] is just a group of triggers.
     A condition [C] can either be a ==/>=/<= comparator with two compared data,
     or a and/or combiner of other conditions.

//// Setup ////

Copy one of the DLL files (d3d9_xxl1.dll, d3d9_xxl2.dll, d3d9_og.dll)
depending on the game to the game's folder (folder with GameModule),
then rename the copied DLL file to d3d9.dll.

If you had another d3d9.dll file before (for example DxWnd or ENB/Reshade...)
and you want to keep using it, rename this DLL to apd3d9.dll before copying
XXL Inspector's DLL.

Now run the game. If a message box appears that the XXL Inspector is enabled,
then it is installed correctly. You should then see a window over the game's
screen.

If you don't need the XXL Inspector anymore (for speedruns for example),
just remove/rename the d3d9.dll file (and rename apd3d9.dll back to d3d9.dll).


//// Crate detector ////

As this might not be obvious to understand, I will do a quick explanation.

After you check "Crate detector" in the main window, you will see a map
of crate stacks in the level over the screen.

It is important to note that the game doesn't have all the crates loaded.
Instead, there are only around let's say 50 crate stacks (more or less
depending on the level), and the game teleports these stacks when one
appears in or disappears from the view of the camera.

A crate stack is represented by a number that indicates the number of crates
in the stack. 1 means a single crate, 2 means two crates in a single stack, ...

0 means that the crate is outside of the camera view, thus inactive and
"ready to be TELEPORTED to the next crate stack that comes in front of the
camera".

When you try a Tardis skip and break the last crate, the crate
stack will become 0 and thus becomes inactive, but you still stand on it.
So when a new crate comes in view, then it is possible that the crate you're
standing will teleport to the new location, with you as well, but not directly
unfortunately. You need to do other actions, like jump or fall, to trigger
the character teleportation.

You can also see the letters A and B which indicate the position of Asterix 
and Obelix respectively (not O because I thought it could be mistaken by a 0,
so I chose Obelix's second character in his name).

A crate stack can also be surrounded by a green and/or blue rectangle:

- If it is surrounded by a blue rectangle, that means that Asterix is standing
on this crate. When you try to do a Tardis skip, you can notice how Asterix
somehow technically stands on another crate when moving the camera, even
before he is teleported!

- A crate stack surrounded by a green rectangle means that it is the first
crate. If it is inactive (0), then it is first crate chosen to be teleported to
the next crate coming into view.

