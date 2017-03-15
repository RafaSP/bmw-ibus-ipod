#define VERSION "KBUS_IPOD_534"

//#include <SoftwareSerial.h>
#include <ctype.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <Arduino.h> 
// notes
/* 
533 Uses full interrupt on ibus test 2 with hardware serial
532 alterations to use linbus chip
531 Added delay on menu execute so that new song for sure starts before getting title and artist.  Alter off message to go with lock the car. Removed New_song flag variable and song_flag void (wasn't beeing used)
530 revised status2 send.  LCD send seems to send twice at times.  Need to revise off message to go with no key message (device 44), removed LCD update now and moved to push timer out
529 Revised CD operation to match car
528 inverted using hardware.  Sending on Hardware serial
527 updates for performace issues with changes (ipod not start up unil in CD mode) etc.  Trying to fix occasional drop when status sends by adding 1mSec to wait time.  Improved but still occassionally dropp
//think the software serial may be too slow to initiate?  revised updating title calls (revised to song ends timer).  Still does not auto start in CD player mode.
526 Added pin 8 High turn off signal 1/1/2014
525 integrated kbus inverted sending
523 in work - refreshing song immediately after song ends in normal play based on arduino timer.  Random flag reset after playing awhile.  Random off when staring a menu?
add flag for mode 4 or not in ipod response.  moved time_CD to Ibus_set_status_BusMessage();  only appears to not be used during CD announce or respond.
522 Added Random on/off buttons 3 and 4.  Extended bus time out.  updated ipod on/off when CD started and stopped.  Revised song refresh routines.  Appears to start stop correctly on White BMW.
521 Stop when IKE turns off instead of K bus goes idle.  Restart when "stop pause" command recieved.  Add flag for Artist,title request.  Timer to resend request if not recieved by x seconds if not completed
520 take random and scan buttons out of menu choices. worked but would over write the text on screen added fix to that.  not yet tested.
519 Reworking start up sequence based on sampling from Car- CD changer interaction works on car.  bit buggy.
518  Adding in lessons from hooking to car.  Ibus First byte = 0 is possible. Working to resolve stray data issues on kbus
517
516
515 chopping text in menus removed.  First 11 characters only.
514 diplay speed of new title
512 new ibus senseing interrupts)  Fixed menu so it works right selecing albums, artist etc.
412 adding play sensing, turned on scan and random buttons for menu navigation.  Song doesn't refresh after pressing number button.  Starting some songs doesn't refresh song.  A few bus errors occasionally.
411 Further refinements (ipod not started until CD mode selected)
410 menu4 works
409 mostly working but menu 4.  Going to 410 with new ipod start stop 
Currently menus are still buggy but 1 2&3 sort of work.
406 seems stable.
405 works with ipod a little.

013 mega works with kbus on hardware port.  initialization done looking for delay in message packets and clearing bad bits.  Program runs faster than bus?
013 initilizes easily. Played for many songs without any drop.
012 attempting to work in sending text to screen nicely.
011 works great for accepting button presse
009 works with all the buttons
KBUS TEST 007 JUST DISPLAYING KBUS MESSAGES
sets up CD correctly


from v204b but modified for testing the kbus.  Fixed sleep when no kbus.  Fixed freeze in the while available statement
5/9/13 v204b spaces in artist and song print
5/9/13 v203 removing unneeded statements. added cycle on song title
5/9/13 v202 added back menu4.  Works using joystick.  removed joystick an runs over kbus.
5/8/13 v200 basically a total restart ***********************
5/6/13 V009bleonardo.  Works with leonardo.  Shows on screen while utilizing hardware serial and soft serial.


Defnitions from http://www.adriangame.co.uk/ipod-acc-pro.html
good exasmple arduion http://scuola.arduino.cc/en/content/arduino-comunication-iphone-using-podbreakout-v15 (broken link?)
Good interface but wire on RX needs to be on other side of transister http://blog.workingsi.com/2010/10/research-on-arduino-obd-ii-interface.html

// methods from Advance remote
  
     * Turn on Advanced Remote mode. This causes the iPod to
     * show a check mark and "OK to disconnect" on its display.
     * The iPod can only be controlled by the remote in this mode.
     *
     * You can just reset your iPod to get it out of this mode
     * if you end up with it stuck in this mode for some reason,
     * such as your sketch not calling disable().

//    void enable()
     * You must call this before calling any of the getXXX methods.
     
//    void disable();
//     * disabled Advanced Remote mode.
//     *
//     * You should call this to get your iPod to stop saying
//     * "OK to disconnect", and let you control it through its
//     * own interface again.

//    void getiPodName();
//     * Get the name of the iPod.

//    void switchToMainLibraryPlaylist();
//     * Switch to the Main Library playlist (playlist 0).

//    void switchToItem(ItemType itemType, long index);
//     * Switch to the item identified by the type and index given.
//     * This is the equivalent of drilling down from the Music
//     * menu to the item specified. If you want to select the item
//     * for playing you then need to call executeSwitch()

//    void getItemCount(ItemType itemType);
//     * Get the total number of items of the specified type.
//     * The response will be sent to the ItemCountHandler, if one
//     * has been registered (otherwise it will be ignored).
//     * The value returned is in the context of the currently
//     * selected playlist (except when you're asking for the count
//     * of playlists, obviously).
//     * The count returned is in the context of the mostly-recently
//     * selected playlist (via switchToItem). The playlist doesn't
//     * have to also have been executeSwitch()'d to however.

//    void getItemNames(ItemType itemType, unsigned long offset, unsigned long count);
//     * Get the names of a range of items.
//     * offset is the starting item (starts at 0, not 1)
//     * The offset is in the context of the mostly-recently
//     * selected playlist (via switchToItem). The playlist doesn't
//     * have to also have been executeSwitch()'d to however.

//    void getTimeAndStatusInfo();
//     * Ask the iPod for time and playback status information.
//     * The response will be sent to the TimeAndStatusHandler, if one
//     * has been registered (otherwise it will be ignored).

//    void getPlaylistPosition();
//     * Ask the iPod for the current position in the current playlist.
//     * The response will be sent to the PlaylistPositionHandler, if one
//     * has been registered (otherwise it will be ignored).
//     * The current playlist is the one last selected via executeSwitch
//     * or via the iPod's controls - call switchToItem for a playlist is
//     * not enough.

//    void getTitle(unsigned long index);
//     * ask the iPod for the title of a track.
//     * The count returned is in the context of the mostly-recently
//     * selected playlist (via switchToItem). The playlist doesn't
//     * have to also have been executeSwitch()'d to however.

//    void getArtist(unsigned long index);
//     * ask the iPod for the artist for a track.
//     * The count returned is in the context of the mostly-recently
//     * selected playlist (via switchToItem). The playlist doesn't
//     * have to also have been executeSwitch()'d to however.

//    void getAlbum(unsigned long index);
//     * ask the iPod for the album for a track.
//     * The count returned is in the context of the mostly-recently
//     * selected playlist (via switchToItem). The playlist doesn't
//     * have to also have been executeSwitch()'d to however.

//    void setPollingMode(PollingMode newMode);
//     * Start or stop polling mode.
//     * Polling mode causes the iPod to return the time elapsed every 500 milliseconds.

//    void executeSwitch(unsigned long index);
//     * Execute Playlist-switch specified in last setItem call, and jump to specified number
//     * in the playlist (0xFFFFFFFF means start at the beginning of the playlist, even when
//     * shuffled). After this call getSongCountInCurrentPlaylist(), getPlaylistPosition()
//     * and jumpToSongInCurrentPlaylist() will be in the context of this newly-switched-to
//     * playlist.

//    void controlPlayback(PlaybackControl command);
//     * Control playback (play/pause, etc)

//    void getShuffleMode();
//     * Ask the iPod for the current shuffle mode.
//     * The response will be sent to the ShuffleModeHandler, if one
//     * has been registered (otherwise it will be ignored).

//    void setShuffleMode(ShuffleMode newMode);
//     * Set the shuffle mode.

//    void getRepeatMode();
//     * Ask the iPod for the current repeat mode.
//     * The response will be sent to the RepeatModeHandler, if one
//     * has been registered (otherwise it will be ignored).

//    void setRepeatMode(RepeatMode newMode);
//     * Set the repeat mode.

//    void getSongCountInCurrentPlaylist();
//     * Get the count of songs in the current playlist.
//     * The response will be sent to the CurrentPlaylistSongCountHandler, if one
//     * has been registered (otherwise it will be ignored).
//     * The current playlist is the one last selected via executeSwitch
//     * or via the iPod's controls - call switchToItem for a playlist is
//     * not enough.
//     */

//
//    void jumpToSongInCurrentPlaylist(unsigned long index);
//     * Jump to the specified song in the current playlist.
//     * The current playlist is the one last selected via executeSwitch
//     * or via the iPod's controls - call switchToItem for a playlist is
//     * not enough.

//    bool isCurrentlyEnabled();
//     * returns true if advanced mode is enabled, false otherwise.
//     * Will be fooled if you put the iPod in advanced mode without called enable()


