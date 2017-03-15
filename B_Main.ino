//SoftwareSerial IPOD(ipod_rx,ipod_tx); // RX, TX;  IPOD will be serial name //for testing only on uno
void setup()
{
  // uncomment to enable debugging
 // #define IPOD_SERIAL_DEBUG //uncomment to see ipod sending and receiving messages
#define Kbus_SERIAL_DEBUG  //uncomment to see some ibus message
//    #define IPOD_only_DEBUG //uncomment ONLY if bench testing ipod commands with no ibus

  pinMode(ipod_rx,INPUT);
  pinMode(ipod_tx,OUTPUT);  
  // pinMode(Kbus_rx,INPUT_PULLUP); //added to reduce noise on Kbus rx line as High is idle  (not needed?)
  pinMode(8,OUTPUT); //Set pin for turn off signal
  digitalWrite(8,HIGH); //low is OFF requst, need tx pin low as well for off (high turns TX ENABLE)

  //set up serial port for debugging with data to PC screen
  Serial.begin(115200); //screen for debuging
  Serial.println(VERSION);
  IPOD.begin(IPOD_SERIAL_RATE);  //IPOD_SERIAL_RATE = 19200

  // start serial communication at the adapter defined baudrate
  // Kbus.begin(9600);
  Kbus.begin(9600, SERIAL_8E1); //Baud rate, Parity, stopbits (1 if blank))

  /* removed due to power off during starting of '99 and '00 models causes this to rerun
   IPODsend(Ipod_On);
   IPODsend(Button_Release);
   IPODsend(SwitchtoRemote); //Disable mode 4 //tends to pull ipod our of errors
   Serial.println("disable");
   */

  time_LCD = millis(); //set LCD timer so it doesn't jump right it.
  time_ends = millis() + 20000;//set timer way out.  Time and status in ipod start will reset it

  Serial.println("finished ipod startup ");
  rar=0;
  menu_flag = 0;
#if defined(IPOD_only_DEBUG)  //debug withoug the ibus
  Serial.println("ipod without ibus config.  Simulating CD enable");
  ipod_start();
  Serial.println("ipod without ibus config");
  //  menu_flag = 1;
#endif  //debug

}

void loop()
{

#if defined(IPOD_only_DEBUG)  //debug withoug the ibus
  menu_flag = 1;
#endif  //debug
#if !defined(IPOD_only_DEBUG)  //debug withoug the ibus
  //  menu_flag = 1;


  //********************* check if Kbus is ready for send **************************** (added one to see if it would quit steping on each other.
  //check to see if bus was idle for more than 1 millisecond && bus was initialized and there is a message waiting to send
  if(micros()- time_Kbus > 1500  && bus_started == 1 && Ibus_STATUS_ready == 1 && kb == 0) Ibus_send_status_BusMessage();  //bus idle, bus initilized, message ready, kbus not in middle of recieving message
  if(micros()- time_Kbus > 1500  && bus_started == 1 && Ibus_MSG_ready == 1 && kb == 0) ibus_send_BusMessage();  //bus idle, bus initilized, message ready, kbus not in recieve

time2 = micros()-time_Kbus;
    //************************  kbus shut down if no cativity on the kbus *******************
  if(time2 > 60000000) //1 min (60 mSec)of ibus inactivity turn off
  {
       Serial.println(time2);
 //   Serial.print("  ");
//Serial.println(time_Kbus);
    Serial.println("Kbus shut down");
    menu_flag = 0; //ensure not left stuck in a menu

    IPODsend(SwitchtoRemote); //turn ipod to remote to recieve ipod off
    IPODsend(Ipod_Off);  // send ipod off command
    delay(10);
    IPODsend(Button_Release); //send button release

    digitalWrite(8,LOW); //low is OFF requst, need tx pin low as well for off (high turns TX ENABLE)
    Kbus.end();
    pinMode(Kbus_tx,OUTPUT);  
    digitalWrite(Kbus_tx,LOW); //TX signal low with wake pin low turns off linbus chip
    delay(1000);
    
    digitalWrite(8,HIGH); //reset to on (if using USB cable and didn't really go off)
  Kbus.begin(9600, SERIAL_8E1); //Baud rate, Parity, stopbits (1 if blank))
    while(!Kbus.available()) //wait here until kbus signals come  Should not get here if power off worked.
    {//just wait.
    }// end if available
    Serial.println("Kbus started");

  }// end ibus time out
#endif  //debug

  //******************** get Ipod message *******************************
  get_ipod_data();//check for ipod messages. check for messages always.  
  // Messages may continue after ipod stopped.  Want to clear data



    //**************************menu routing ***********************************
  if(menu_flag == 1) //**111111111111111111111111111111111111111111111111111111111111111111111111111111111111111
      //this is the root menu and if just playing music will be in this menu.  Menu 0 is Radio mode (not CD mode)
  {
    if(ButtonState > 0) //if a button has been pressed
    {
      Serial.println("menu 1 testing");
      menu1(); //go to menu 1, play, skip, menu
    }// end if ButtonState >0 

    if(millis() > (time_ends + 500)) //if clock is past expected time song will end plus 500 mSec
    {
      song_message = 1;  //set flag to get title
      time_ends = millis() + 60000;  //give ipod 8 seconds to get title, artist and status. (otherwise it'll go through here again)
      Serial.println("song end timer passsed");
    }//end song timer end

    //    if((millis()-time_status) > 4000)    //check to see if time and status needs updated (could add variable to > x and subtract time remaining to speed up refresh)
    //    { //update time and play status every xx seconds
    //      IPODsend(GetTimeAndStatus);  //ask for time and status  
    //      time_status = millis(); //reset timer timer
    //    } //end if time and status timer expired

    //    song_flag();  //update song flags

    if (song_message > 0 && song_message < 10) get_title(); // if ready to get song, artist or title

      if ((millis()-time_LCD) > 5000 || update_LCD_now > 0)  update_LCD(); //update LCD screen if time spent or update flag is set 
    //     if ((millis()-time_LCD) > 5000)  update_LCD(); //update LCD screen if time spent or update flag is set 

  }// end menu 1


  if(menu_flag == 2) //**22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222
  { // the menu to pick playlist, artist, album etc.
    if(ButtonState > 0) 
    {
      Serial.println("menu 2 testing");
      PrintCurrentType(); //put current type on screen
      menu2(); // artist, album
    }// end if ButtonState >0 
  } //end menu 2
  if(menu_flag == 3) //**33333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333
  {
    if(menu3_wait == 0) //if type count flag is set to 0 (need to get it) 
    {
      Serial.println("type count get");
      if(ipod_sent == 0)
      {
        IPODsend(GetTypeCount,CurrentType);  //Get number of items in this type (number of artist or playlist...)
        menu3_wait = 2; //waiting for ipod update
      }
    }

    if(menu3_wait == 1)//if 1 type answer has been rec'd 
    { 
      Serial.println("type count have");
      if (CurrentType == TYPE_PLAYLIST) CurrentTypePosition =  1;
      else CurrentTypePosition =  0; // reset position if coming through the menu again 
      //if(ipod_sent == 0)
      IPODsend(GetTypeNames,CurrentType,CurrentTypePosition,one);  //what is the name of the one we are on now
      menu3_wait = 2; //waiting for ipod update

      Serial.print("max items ");  //debug
      Serial.println(CurrentTypeMax); //debug


    }
    if(ButtonState > 0  && menu3_wait == 3) //button pressed and type flag to recieved type name
    {
      Serial.println("menu 3 testing");
      menu3(); //Select the item in the type  
      //        type_count_recd = 0;

    }
  }//end menu flag 3

    if(menu_flag == 4) //******44444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444
  {
    if(menu4_wait==0) 
    {
      Serial.println("menu 4 get item");
      CurrentItemPosition =  0; // reset position if coming through the menu again
      IPODsend(GetNumberOfSongs);  //Get number of songs
      menu4_wait = 3; //set flag to asked for it but don't have it
    }

    if(menu4_wait==1) 
    {
      Serial.println("menu 4 get title");
      IPODsend(GetSongTitle,CurrentItemPosition);  //Get number title of what we are on now
      // IPODsend(GetTypeNames,CurrentType,CurrentTypePosition,one);  //what is the name of the one we are on now
      menu4_wait = 3; //cleared 
    }
    if(menu4_wait==2) 
    {
      Serial.println("menu4 set output");  
      set_BusMessage_ipod(SongName); //processing the Get songTitle puts title into SongName
      menu4_wait = 3; //cleared
    }

    if(ButtonState > 0) 
    {
      Serial.println("menu 4 button pressed");
      menu4(); //select song
    }// end if ButtonState >0
  }//end case 4
  // ___________________ end menu 4

}//end void loop

/*
//******************************** song_flag  ****************************************
void song_flag()  //update song flags, not used
{ 
  if(new_song == 1 && song_message == 0) //TimeElapsed<6000 && TimeElapsed > 1 &&  if song is near the beginning and it's a new song and not working on the artist title
  {// will not go through this loop twice as song_message immediately altered and not back to zero unless finished getting title or menu execute.
    song_message = 1; //update song artist, title from beginning of gathering data 
  }


  if(TimeElapsed>50000 && new_song == 1 && song_message != 0) //if song has been playing a long time and has not made it trough artist and title (song message not=0)
  {//& haven't gotten through collecting the artist and title.
    song_message = 1; //update song artist, title from beginning of gathering data
  }//end if song has played awhile 

//}// end update song timers
*/

//************************************** Get title and artist info ************************************** 
void get_title()
{ //enters this void if song_message is > 0 and less than 10;

  if (song_message == 1) 
  {
//    new_song ++; // not a new song if we've made it this far.  Flagging so doesn't come back
    Serial.println("get play list position in title");
    IPODsend(GetPlaylistPosition); //need to have this response prior to get song title
    song_message = 11; //clear song message flag until get playlist position rec'd
    time_LCD = millis(); //set LCD timer so it doesn't update until new title is rec'd.
  }

  if (song_message == 2) 
  {
    Serial.println("get Song Title");
    IPODsend(GetSongTitle,CurrentSongNu); //get song title.  Needs the Current song number from (IPOD play list position)
    song_message = 12; //clear song message flag until title rec'd
  }

  if (song_message == 3)     
  {
    Serial.println("get Artist");
    IPODsend(GetSongArtist,CurrentSongNu); //get Artist
    song_message = 13; //clear song message flag until get artist rec'd

  }

  if (song_message == 4)     
  {
    Serial.println("get time and status");
    IPODsend(GetTimeAndStatus);  //ask for time and status
    song_message = 14; //clear song message flag until time and status rec'd
    // add check random status?
  }

}//end get title



//********************* menu execute ******************************************
void menu_execute()
{ //cleanup variables after a menu choice selected and set so LCD will update properly
  Serial.println("menu execute");
  menu_flag = 1;
//  new_song = 1;  //if song has just started and it has not yet went through get info loop
  //song_message = 1; //Initiate getting Song title, artist etc., removed.  have time ends initiate
  time_ends = millis() + 2000;  //give ipod 8 seconds to get title, artist and status.
  time_LCD = millis(); //set LCD timer so it doesn't update until new title is rec'd.
  set_BusMessage_ipod(".Slow ipod.");
  play_please = 1;
}

//************************** ipod keep playing ****************
void keep_playing() //check play status and push play if not playing
{
  if(PlayingStatus == STATUS_STOPPED || PlayingStatus == STATUS_PAUSED)
  {
    Serial.print("keep playing, playing status= ");
    Serial.println(PlayingStatus);
    //   IPODsend(ControlPlayback,PLAYBACK_CONTROL_PLAY_PAUSE); //send play press *** removed as kept stopping and not sure why
    //    set_BusMessage_ipod("Normal Play");
  }
}
































































































