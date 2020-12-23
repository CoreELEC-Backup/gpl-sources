mpc
===


Description
-----------

mpc is a command-line client for the `Music Player Daemon
<http://www.musicpd.org/>`__ (MPD).  It connects to a MPD and controls
it according to commands and arguments passed to it.  If no command is
given, the current status is printed (same as ":samp:`mpc status`").


Synopsis
--------

 mpc [options] <command> [<arguments>]


Options
-------

.. option:: -f, --format

 Configure the format used to display songs.

 The metadata delimiters are:

 ================== ======================================================
 Name               Description
 ================== ======================================================
 %name%             A name for this song.  This is not the song title. The exact meaning of this tag is not well-defined. It is often used by badly configured internet radio stations with broken tags to squeeze both the artist name and the song title in one tag.
 %artist%           Artist file tag
 %album%            Album file tag
 %albumartist%      Album Artist file tag
 %comment%          Comment file tag (not enabled by default in :file:`mpd.conf`'s metadata_to_use)
 %composer%         Composer file tag
 %date%             Date file tag
 %originaldate%     Original Date file tag
 %disc%             Disc file tag
 %genre%            Genre file tag
 %performer%        Performer file tag
 %title%            Title file tag
 %track%            Track file tag
 %time%             Duration of file
 %file%             Path of file, relative to MPD's ``music_directory`` variable
 %position%         Queue track number
 %id%               Queue track id number
 %prio%             Priority in the (random) queue.
 %mtime%            Date and time of last file modification
 %mdate%            Date of last file modification
 ================== ======================================================

 The ``[]`` operator is used to group output such that if no metadata
 delimiters are found or matched between ``[`` and ``]``, then none of the
 characters between ``[`` and ``]`` are output.  ``&`` and ``|`` are logical
 operators for and and or.  ``#`` is used to escape characters.  Some
 useful examples for format are: ":samp:`%file%`" and
 ":samp:`[[%artist% - ]%title%]|[%file%]`".  This command also takes
 the following defined escape sequences:

 ======== ===================
 \\       backslash
 \\[      left bracket
 \\]      right bracket
 \\a      alert
 \\b      backspace
 \\e      escape
 \\t      tab
 \\n      newline
 \\v      vertical tab
 \\f      form-feed
 \\r      carriage return
 ======== ===================

 If not given, the value of the environment variable
 :envvar:`MPC_FORMAT` is used.

.. option:: --wait

 Wait for operation to finish (e.g. database update).

.. option:: --range=[START]:[END]

 Operate on a range (e.g. when loading a playlist).  START is the
 first index of the range, END is the first index after the range
 (i.e. excluding).  START and END may be omitted, making the range
 open to that end.  Indexes start with zero.

.. option:: -q, --quiet, --no-status

 Prevents the current song status from being printed on completion of
 some of the commands.

.. option:: --verbose

 Verbose output.

.. option:: --host=HOST

 The MPD server to connect to.  This can be a hostname, IPv4/IPv6
 address, an absolute path (i.e. local socket) or a name starting with
 ``@`` (i.e. an abstract socket, Linux only).

 To use a password, provide a value of the form
 ":samp:`password@host`".

 If not given, the value of the environment variable
 :envvar:`MPD_HOST` is used.

.. option:: --port=PORT, -p PORT

 The TCP port of the MPD server to connect to.

 If not given, the value of the environment variable
 :envvar:`MPD_PORT` is used.


Commands
--------

Commands can be used from the least unambiguous prefix (e.g insert or
ins).


Player Commands
^^^^^^^^^^^^^^^

:command:`consume <on|off>` - Toggle consume mode if state (:samp:`on`
   or :samp:`off`) is not specified.

:command:`crossfade [<seconds>]` - Gets and sets the current amount of
   crossfading between songs (:samp:`0` disables crossfading).

:command:`current [--wait]` - Show the currently playing song.  With
   :option:`--wait`, mpc waits until the song changes (or until playback
   is started/stopped) before it queries the current song from the
   server.

:command:`queued` - Show the currently queued (next) song.

:command:`mixrampdb [<db>]` - Gets and sets the volume level at which
   songs with MixRamp tags will be overlapped. This disables the
   fading of the crossfade command and simply mixes the
   songs. :samp:`-50.0` will effectively remove any gaps, :samp:`0.0`
   will mash tracks together. The amount of overlap is limited by the
   audio_buffer_size MPD configuration parameter.

:command:`mixrampdelay [<seconds>]` - Gets and sets the current amount
   of extra delay added to the value computed from the MixRamp
   tags. (A negative value disables overlapping with MixRamp
   tagqs and restores the previous value of crossfade).

:command:`next` - Starts playing next song on queue.

:command:`pause` - Pauses playing.

:command:`play <position>` - Starts playing the song-number
   specified. If none is specified, plays number 1.

:command:`prev` - Starts playing previous song.

:command:`random <on|off>` - Toggle random mode if state (:samp:`on`
   or :samp:`off`) is not specified.

:command:`repeat <on|off>` - Toggle repeat mode if state (:samp:`on`
   or :samp:`off`) is not specified.

:command:`replaygain [<off|track|album>]` - Sets the replay gain mode.
   Without arguments, it prints the replay gain mode.

:command:`single <on|off>` - Toggle single mode if state (:samp:`on`
   or :samp:`off`) is not specified.

:command:`seek [+\-][<HH:MM:SS>] or <[+\-]<0-100>%>` - Seeks by hour,
   minute or seconds, hours or minutes can be omitted.  If seeking by
   percentage, seeks within the current song in the specified manner.
   If a :samp:`+` or :samp:`-` is used, the seek is done relative to
   the current song position. Absolute seeking by default.

:command:`seekthrough [+\-][<HH:MM:SS>]` - Seeks by hour,
   minute or seconds, hours or minutes can be omitted, relatively to
   the current position. If the duration exceeds the limit of the
   current song, the seek command proceeds to seek through the playlist
   until the duration is reached.
   If a :samp:`+` is used, the seek is forward. If a :samp:`-` is
   used, the seek is backward. Forward seeking by default.

:command:`stop` - Stops playing.

:command:`toggle` - Toggles between play and pause. If stopped starts
   playing.  Does not support start playing at song number (use play).


Queue Commands
^^^^^^^^^^^^^^

:command:`add <file>` - Adds a song from the music database to the
   queue. Can also read input from pipes. Use ":samp:`mpc add /`" to
   add all files to the queue.

:command:`insert <file>` - The insert command works similarly to
   :command:`add` except it adds song(s) after the currently playing
   one, rather than at the end.  When random mode is enabled, the new
   song is queued after the current song.

:command:`clear` - Empties the queue.

:command:`crop` - Remove all songs except for the currently playing
   song.

:command:`del <songpos>` - Removes a queue number from the queue. Can
   also read input from pipes (:samp:`0` deletes the current playing
   song).

:command:`mv, move <from> <to>` - Moves song at position <from> to the
   position <to> in the queue.

:command:`searchplay <type> <query> [<type> <query>]...` - Search the
   queue for a matching song and play it.

:command:`shuffle` - Shuffles all songs on the queue.


Playlist Commands
^^^^^^^^^^^^^^^^^

:command:`load <file>` - Loads <file> as queue.  The option
:option:`--range` may be used to load only a portion of the file
(requires libmpdclient 2.16).

:command:`lsplaylists`: - Lists available playlists.

:command:`playlist [<playlist>]` - Lists all songs in <playlist>. If
   no <playlist> is specified, lists all songs in the current queue.

:command:`rm <file>` - Deletes a specific playlist.

:command:`save <file>` - Saves playlist as <file>.


Database Commands
^^^^^^^^^^^^^^^^^

:command:`listall [<file>]` - Lists <file> from database.  If no
   <file> is specified, lists all songs in the database.

:command:`ls [<directory>]` - Lists all files/folders in
   <directory>. If no <directory> is specified, lists all files in
   music directory.

:command:`search <type> <query> [<type> <query>]...` - Searches for
   substrings in song tags.  Any number of tag type and query
   combinations can be specified.  Possible tag types are: artist,
   album, title, track, name, genre, date, composer, performer,
   comment, disc, filename, or any (to match any tag).

:command:`search <expression>` - Searches with a filter expression,
e.g. ``mpc search '((artist == "Kraftwerk") AND (title == "Metall auf
Metall"))'``.  Check the `MPD protocol documentation
<https://www.musicpd.org/doc/protocol/filter_syntax.html>`__ for
details.  This syntax can be used with :command:`find` andd
:command:`findadd` as well.  (Requires libmpdclient 2.16 and MPD 0.21)

:command:`find <type> <query> [<type> <query>]...` - Same as search,
   but tag values must match <query>s exactly instead of doing a
   substring match.

:command:`findadd <type> <query> [<type> <query>]...` - Same as find,
   but add the result to the current queue instead of printing them.

:command:`list <type> [<type> <query>]...` - Return a list of all tags
   of given tag <type>.  Optional search <type>s/<query>s limit
   results in a way similar to search.

:command:`stats` - Displays statistics about MPD.

:command:`update [\-\-wait] [<path>]` - Scans for updated files in the
   music directory.  The optional parameter <path> (relative to the
   music directory) may limit the scope of the update.

   With :option:`--wait`, mpc waits until MPD has finished the update.

:command:`rescan [\-\-wait] [<path>]` - Like update, but also rescans
   unmodified files.


Mount Commands
^^^^^^^^^^^^^^

:command:`mount` - Lists all mounts.

:command:`mount <uri> <storage>` - Create a new mount.

:command:`unmount <uri>` - Remove a mount.


Sticker Commands
^^^^^^^^^^^^^^^^^

The :command:`sticker` command allows you to get and set song
stickers.

:command:`sticker <file> set <key> <value>` - Set the value of a song
   sticker.

:command:`sticker <file> get <key>` - Print the value of a song
   sticker.

:command:`sticker <file> list` - List all stickers of a song.

:command:`sticker <file> delete <key>` - Delete a song sticker.

:command:`sticker <dir> find <key>` - Search for stickers with the
   specified name, below the specified directory.



Output Commands
^^^^^^^^^^^^^^^

:command:`volume [+\-]<num>` - Sets the volume to <num> (0-100).  If
   :samp:`+` or :samp:`-` is used, then it adjusts the volume relative to
   the current volume.

:command:`outputs` - Lists all available outputs

:command:`disable [only] <output # or name> [...]` - Disables the
   output(s); a list of one or more names or numbers is
   required. If "only" is the first argument, all other outputs
   are enabled.

:command:`enable [only] <output # or name> [...]` - Enables the
   output(s); a list of one or more names or numbers is required. If
   ":samp:`only`" is the first argument, all other outputs are
   disabled.

:command:`toggleoutput <output # or name> [...]` - Changes the
   status for the given output(s); a list of one or more names or
   numbers is required.

Client-to-client Commands
^^^^^^^^^^^^^^^^^^^^^^^^^

:command:`channels` - List the channels that other clients have
   subscribed to.

:command:`sendmessage <channel> <message>` - Send a message to the
   specified channel.

:command:`waitmessage <channel>` - Wait for at least one message on
   the specified channel.

:command:`subscribe <channel>` - Subscribe to the specified channel
   and continuously receive messages.


Other Commands
^^^^^^^^^^^^^^

:command:`idle [events]` - Waits until an event occurs.  Prints a list
   of event names, one per line.  See the MPD protocol documentation
   for further information.

   If you specify a list of events, only these events are considered.

:command:`idleloop [events]` - Similar to :command:`idle`, but
   re-enters "idle" state after events have been printed.

   If you specify a list of events, only these events are considered.

:command:`version` - Reports the version of MPD.


Environment Variables
---------------------

All environment variables are overridden by any values specified via
command line switches.

.. envvar:: MPC_FORMAT

 Configure the format used to display songs.  See option
 :option:`--format`.

.. envvar:: MPD_HOST

 The MPD server to connect to.  See option :option:`--host`.

.. envvar:: MPD_PORT

 The TCP port of the MPD server to connect to.  See option
 :option:`--port`.


Bugs
----

Report bugs on https://github.com/MusicPlayerDaemon/mpc/issues

Since MPD uses UTF-8, mpc needs to convert characters to the charset
used by the local system.  If you get character conversion errors when
you're running mpc you probably need to set up your locale.  This is
done by setting any of the LC_CTYPE, LANG or LC_ALL environment
variables (LC_CTYPE only affects character handling).

See also
--------

:manpage:`mpd(1)`


Author
------

See https://raw.githubusercontent.com/MusicPlayerDaemon/mpc/master/AUTHORS
