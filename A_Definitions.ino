
#define IPOD Serial1 //rename serial to IPOD for Uno it'll be Serial
#define Kbus Serial2 //rename serial to IPOD for Uno it'll be Serial
#define timeoutlen 2
#define get_Kbus_data serialEvent2 //calls get_Kbus_data when there is activity on Serial 2
#define ipod_rx 10  //rx pin (first number) 8 for leonardo, 2 uno, 10 for Mega,purple on chord
int ipod_tx = ipod_rx +1; //tx pin (sendond number) 9 for leonardo, 3 uno, 11 for Mega, brown on chord
#define Kbus_rx 17  //rx pin for Kbus, Mega is 17 (tx =16),Uno and Leonardo rx 0, tx 1
#define Kbus_tx 16
//audio ground yellow, purple+ and white +

byte len;
byte dataSize;
byte dataBuffer[128]; // ipod data storage
void(* resetFunc) (void) = 0; //defines void for reset
/**
 * Helper macro for figuring out the length of command byte arrays.
 */
#define ARRAY_LEN(array) (sizeof(array) / sizeof(array[0]))

static const byte HEADER1 = 0xFF;  //ipod standard message heater byte 1
static const byte HEADER2 = 0x55;  //ipod standard message heater byte 2

static const int IPOD_SERIAL_RATE = 19200;
byte *pData;
byte checksum;
int Knumbytes; //number of bytes used in ibus recieve
const unsigned long Delay = 300;  //for waiting for next press was 300


enum ReceiveState
{
  WAITING_FOR_HEADER1 = 0,
  WAITING_FOR_HEADER2,
  WAITING_FOR_LENGTH,
  WAITING_FOR_DATA,
  WAITING_FOR_CHECKSUM
};
ReceiveState receiveState;
//************************************for inverted serial
  uint8_t _transmitBitMask;
  volatile uint8_t *_transmitPortRegister;
  uint16_t _tx_delay;

//************************************from advance remote
// enums keep these

enum ItemType
{
  TYPE_PLAYLIST = 0x01,
  TYPE_ARTIST = 0x02,
  TYPE_ALBUM = 0x03,
  TYPE_GENRE = 0x04,
  TYPE_SONG = 0x05,
  TYPE_COMPOSER = 0x06
};

enum PlaybackStatus
{
  STATUS_STOPPED = 0,
  STATUS_PLAYING = 1,
  STATUS_PAUSED = 2
};

enum PollingMode
{
  POLLING_START = 0x01,
  POLLING_STOP = 0x00
};

enum PollingCommand
{
  POLLING_TRACK_CHANGE = 0x01,
  POLLING_ELAPSED_TIME = 0x04
};

enum PlaybackControl
{
  PLAYBACK_CONTROL_PLAY_PAUSE = 0x01,
  PLAYBACK_CONTROL_STOP = 0x02,
  PLAYBACK_CONTROL_SKIP_FORWARD = 0x03,
  PLAYBACK_CONTROL_SKIP_BACKWARD = 0x04,
  PLAYBACK_CONTROL_FAST_FORWARD = 0x05,
  PLAYBACK_CONTROL_REVERSE = 0x06,
  PLAYBACK_CONTROL_STOP_FF_OR_REV = 0x07
};

enum ShuffleMode
{
  SHUFFLE_MODE_OFF = 0x00,
  SHUFFLE_MODE_SONGS = 0x01,
  SHUFFLE_MODE_ALBUMS = 0x02
};

enum RepeatMode
{
  REPEAT_MODE_OFF = 0x00,
  REPEAT_MODE_ONE_SONG = 0x01,
  REPEAT_MODE_ALL_SONGS = 0x02
};

enum Feedback
{
  FEEDBACK_SUCCESS = 0x00,
  FEEDBACK_FAILURE = 0x02,
  FEEDBACK_INVALID_PARAM = 0x04,
  FEEDBACK_SENT_RESPONSE = 0x05
};


boolean  currentlyEnabled = false;

//ours
byte Kdata[140]; //ibus data
byte* Kbuf[138]={
  &Kdata[2],&Kdata[3],&Kdata[4],&Kdata[5],&Kdata[6],&Kdata[7],&Kdata[8],&Kdata[9],&Kdata[10],&Kdata[11],&Kdata[12],&Kdata[13],&Kdata[14],&Kdata[15],
  &Kdata[16],&Kdata[17],&Kdata[18],&Kdata[19],&Kdata[20]};//pointer for reading Kbus with read bytes w/o overwriting first 2 bits refences kdata bytes 2-20;

byte data[140]; // ipod data 

long CurrentSongNu; //current song number
long TimeElapsed;
long time_ends;
long LengthOfSong;
//int song_ended = 0; //flag for end of song
byte CurrentType = 1;
long CurrentTypeMax = 1;
long CurrentTypePosition = 1; // which playlist, artist etc
long CurrentItemPosition = 1; // Song, album etc within that list
long CurrentItemMax = 1;
int menu3_wait = 0;
int LastTextByte;
char text[11] = {
  0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19}; //text to send out on Kbus
byte BusMessage[18]={ //first 6 bytes are fixed (18 bytes max)
  0xC8,0x10,0x80,0x23,0x42,0x30,0x44,0x61,0x75,0x67,0x68,0x74,0x72,0x79,0x19,0x19,0x19,0x30};//message default
#define CD_No   0x7
#define Track 0x1
byte BusStatusMessage[18]={0x18, 0x0A, 0x68, 0x39, 0x00, 0x02, 0x00, 0x3F, 0x00, CD_No, Track, 0xFF};//CD status Stop, pause default
byte BusStatus2Message[18]={0x18, 0x0A, 0x68, 0x39, 0x00, 0x02, 0x00, 0x3F, 0x00, CD_No, Track, 0xFF};//CD status Stop, pause default
int Ibus_STATUS_ready;
int Ibus_STATUS2_ready;

long debugtime;
int ButtonState;
int Ibus_x; //store reciving status
long time_status; //time to update status
long time_LCD; //time to update LCD display
long time_Kbus; //time waiting to see if messages stopped
long time2;
long time_CD; //time between CD announce
int menu_flag; //show what menu to call.  0=when radio playing, 1 and up when CD mode.
boolean ipod_sent;// flag to stop sending ipod messages until next recieved.  1 = wait to send
int radio_polled; //flag to show if radio sent poll command yet
int menu4_wait; 
boolean Ibus_MSG_ready; //is there text ready to send over ibus
int bus_started =0;
int CD_played = 0;
//int new_song = 1; //flag for when we just started a new song
//int title_try; //# of tryies getting song and title.
int song_message = 1; //which song message to send next
int LCD_wait; // pause flag between song, artist flags
//int LCD_count;
int ip; //index counter for ipod
int kb; //index counter for Kbus
byte IPODchecksum; //checksum for ipod
boolean rar;
char SongName[150] = "song title is very very long"; //currently playing song name
char UPDATING[11] = "UPDATING"; //currently playing song name
int update_LCD_now; //flag to force LCD to update 0 no force update, 1 update title, 2 update artist (and title);
byte song_name_len; //length of the title
int song_space; //character where last space in artist name is held
int r2; //auto set to length of text currently being output on Kbus
char ArtistName[150] = "artist Garrett Smith"; //currently playing Artist name
byte artist_name_len;  //length of artist name
int artist_space; //character where last space in artist name is held
char AlbumName[150]; //currently playing Album name
byte album_name_len; //length of the album name
char IpodName[50]; 
char CurrentTypeName[50];
byte CurrentShuffleMode;
byte CurrentRepeatMode;
byte PlayingStatus;
int play_please;
long NumberOfSongs;
int last_message;  //most recent button press
int rad_command; //radio command flag
int ButtonState2;  //button press history
int space;
void KbusSendText2(String message,int first = 0); //message to send, where break should be, first or second sending
//button setup definitions
#define one    1
#define two    2
#define three  3
#define four   4
#define five   5
#define six    6
#define cd_stop 7
#define right  8
#define left   9
#define play   10
#define select 11
#define cd_status 12
#define random_on 13
#define random_off 14
#define scan_on 15
#define scan_off 16
#define FRWD    17
#define FFWD    18
#define cd_pause 19
#define cd_stop_pause 20
#define RT_press 21
#define dial 22
#define menu_display 99


//defines for CD status bit 4
#define B4_stop 0x00
#define B4_pause 0x01
#define B4_play 0x02
#define B4_ffwd 0x03
#define B4_frwd 0x04

//defines for CD statuts bit 5
#define B5_stop 0x00
#define B5_pause 0x02
#define B5_play 0x09
//#define Rad_seek 0x10
int B5_status; //play status (stop, pause or play)
int B5_seek; //0x10 makes B5 seek
#define Rad_random 0x20
int B5_random; //0x20 makes B5 Random







