# libretro Command-Line Options

You can pass the ScummVM command line parameters to the libretro core in
a couple different ways...

## Game ID Folders

Place the game data files inside a folder named by the game short name
from ScummVM. The game IDs can be found in ScummVM's
[compatibility list](http://scummvm.org/compatibility).

### Example for Day of the Tentacle

1. Create a folder named "tentacle"
2. Add [the Day of the Tentacle datafiles](http://wiki.scummvm.org/index.php/Datafiles#Day_of_the_Tentacle) to the folder
  - TENTACLE.000
  - TENTACLE.001
  - MONSTER.SOU

Launch retroarch by referencing one of the files in this directory:

    retroarch -L scummvm_libretro.so "/myroms/tentacle/TENTACLE.000"

## .scummvm Files

Creating a text file containing the commands and then using that file as
a rom file for the core.

### Example for Day of the Tentacle

1. Add "Day of the Tentacle" to ScummVM using the built-in ScummVM GUI.
2. Make note of the gameid (in this case "tentacle")
3. Create a text file containing the game id

For example:
"Day of the Tentacle.scummvm" --> inside the file is written a single line with
the word "tentacle"

Launch retroarch with the scummvm core and with this text file as the rom:

    retroarch -L scummvm_libretro.so "/myroms/Day of the Tentacle.scummvm"

The game will now launch directly instead of showing the ScummVM GUI.
You can pass other ScummVM parameters in the text file besides the gameid.