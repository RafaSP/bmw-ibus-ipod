//*********************** after pressing menu button
//************************* menu 1 *****************
void menu1()
{
  Serial.println("in menu 1");
  switch (ButtonState) //********************* Menu 1
  { //switch begin

  case left: //********************* Menu 1
    // reduce type by one 
    {
      Serial.println(" left");
      set_BusMessage_ipod("Skip Back");
      IPODsend(Skip_Backward);
      IPODsend(Button_Release);
      menu_execute();
      break;
    }
  case right: //********************* Menu 1
    // increase type by one 
    {   
      Serial.println(" right");
      set_BusMessage_ipod("Skip Fwd");
      IPODsend(Skip_Forward);
      IPODsend(Button_Release);
      menu_execute();
      break;
    }
  case one: //********************* Menu 1
    { 
      //Serial.println(" button 1");
      set_BusMessage_ipod("  Enable  ");
      IPODsend(SwitchtoAir); //Enable mode 4
      break;
    }
  case two: //********************* Menu 1
    {
      //Serial.println(" button 2");
      set_BusMessage_ipod(" Disable  ");
      IPODsend(SwitchtoRemote); //Enable mode 4
      break;
    }
  case three: //********************* Menu 1 
    {
      Serial.println("button 3, random on");
//      B5_random = Rad_random; //if B5_random = 0 random off if = 0x20 random is on (flag for radio)
//      CD_Status[5] = B5_play + B5_random + B5_seek;  //set request flag to play and random flag on or off
//      Ibus_set_status_BusMessage(CD_Status); //load status message into Ibus send variable //send status (including random flags)
      IPODsend(SetShuffleMode,SHUFFLE_MODE_SONGS);  //Set shuffle mode to Random
      IPODsend(GetShuffleMode); //check shuffle mode 
      break;
    }             
  case four: //********************* Menu 1  
    {
      Serial.println("Button4, random off");
//      B5_random = 0; //if B5_random = 0 random off if = 0x20 random is on (flag for radio)
 //     CD_Status[5] = B5_play + B5_random + B5_seek;  //set request flag to play and random flag on or off (do now or user will think it hasn't happened
//      Ibus_set_status_BusMessage(CD_Status); //load status message into Ibus send variable //send status (including random flags)
      IPODsend(SetShuffleMode,SHUFFLE_MODE_OFF);  //Set shuffle mode
      IPODsend(GetShuffleMode); //check shuffle mode 
      break;
    }                       
  case five: //********************* Menu 1
    {
      IPODsend(Just_Play);
      IPODsend(Button_Release);
      Serial.println(" button 5, Just play");
      play_please = 1;
      break;
    }                    
  case six: //********************* Menu 1
    {
      IPODsend(Play);
      IPODsend(Button_Release);
      play_please = 0;
      Serial.println(" button 6, play");
      break;
    }
  case random_off: //********************* Menu 1 /
    {
      Serial.println(" random off from Radio - ignore");
      //do nothing as radio sends this at odd times.
      //B5_random = 0; //if B5_random = 0 random off if = 0x20 random is on (flag for radio)
      break;
    }

  case random_on: //********************* Menu 1 //think this is ok.  Just not random off
    {
      Serial.println(" random on");
      B5_random = Rad_random; //if B5_random = 0 random off if = 0x20 random is on (flag for radio)
      IPODsend(SetShuffleMode,SHUFFLE_MODE_SONGS);  //Set shuffle mode to Random
      IPODsend(GetShuffleMode); //check shuffle mode 
      break;
    }
  case RT_press: //********************* Menu 1  //at startup there's a stray RT press.  So no menu 1 opton
    {
      Serial.println(" RT_press - ignore");
      //do nothing as radio sends at car start up
      break;
    }

  case dial: //********************* Menu 1 //used to go to menu 2.
    {
      Serial.println(" dial button");
      Serial.println(" dial button select in menu 1 -> menu 2");
      //     IPODsend(ControlPlayback,PLAYBACK_CONTROL_PLAY_PAUSE);  //counter act radio turning off play (need a check to see if play is on)
      //     keep_playing();
      menu_flag = 2;
      IPODsend(SwitchToPlaylist0);  //switch to playlist zero to show all songs and options when selecting
      // do we need to turn random off on the ipod?
      CurrentType=TYPE_PLAYLIST; //set default type to playlist
      PrintCurrentType(); //put current type on screen
      ButtonState = menu_display;
      return;
    }

  case FRWD: //********************* Menu 1
    {
      Serial.println(" Fast RWD");
      set_BusMessage_ipod("Fast RWD");
      IPODsend(ControlPlayback,PLAYBACK_CONTROL_SKIP_BACKWARD);
      menu_execute();
      break;
    }
  case FFWD: //********************* Menu 1
    {
      Serial.println(" Fast FWD");
      set_BusMessage_ipod("Fast FWD");
      IPODsend(ControlPlayback,PLAYBACK_CONTROL_FAST_FORWARD);
      menu_execute();
      break;
    }

    //scan_off, scan_on, random_off, RT_press not used as radio sends when it is not wanted. (RT_Press only on startup)
  default:
    {
      Serial.println(" undefined button, or no action button");
    }

  }// end switch
  ButtonState2 = ButtonState; //don't store status messages
  last_message = ButtonState; //this my not be needed?
  ButtonState = 0;
  Serial.println("return from 1");
}//end void menu1  return to main


//********************* Menu 2 Add choice for random on and off in playlist, album, etc.
void menu2()
{
  Serial.print(ButtonState);
  Serial.println("= button state in menu 2");
  //select between playlist, Album, Genere etc
  // moved to menu select IPODsend(SwitchToPlaylist0);  //switch to playlist zero to show all songs and options when selecting
  // need to do something with this  PrintCurrentType(); //put current type on screen


    switch (ButtonState) //********************* Menu 2
  {

    //case scan_off: //********************* Menu 1
  case dial: //********************* Menu 1 //same as scann_off
    { //menu or select
      Serial.println(" dial button");
      if(CurrentType==TYPE_SONG) 
      {
        Serial.println("selected menu2 -> menu 4");
        menu4_wait = 0;
        menu_flag = 4; //got to song menu select if song type selected

      }
      //      // Serial.println("menu 2");
      else {
        Serial.println("selected menu2 -> menu 3");
        menu_flag = 3; //
      }
      ButtonState = menu_display;
      return;
    }
    //  case random_off: //********************* Menu 2
    //  case random_on: //********************* Menu 2
  case RT_press: //********************* Menu 2
    {
      Serial.println("selected menu2 -> menu 1");
      // Serial.println("esc this menu");  //debug
      menu_execute();

      ButtonState = menu_display;
      return; //go up one Menu level 
    }

  case FFWD: //********************* Menu 2   
    { //play
      // Serial.println("play");
      keep_playing();
      //      set_BusMessage_ipod("Normal Play menu2");
      menu_execute();  //set flag for song display to update
      ButtonState = 0;
      break;
    }

  case left: //********************* Menu 2    
    { // reduce type by one
      if(CurrentType==1) CurrentType = 5;  //if current type is at min then set to max
      else CurrentType --;
      PrintCurrentType();
      break;
    }
  case right: //********************* Menu 2 
    { // increase type by one 
      if(CurrentType==5) CurrentType = 1; //if current type is already at max desired switch to 1
      else CurrentType ++; //else increase by 1
      PrintCurrentType();
      break;
    }
    
    //scan_off, scan_on, random_off, random_on not used as radio sends when it is not wanted. (Stray RT_Press only on startup)
  default:
    {
      if(ButtonState > 0 && ButtonState <7)
      {// set current state to 1 = playlist, 2 = artist, 3 = Album
        CurrentType = ButtonState; //4 = GENRE, 5 = SONG, 6 = COMPOSER
        PrintCurrentType();
        Ibus_MSG_ready = 1;  //flag text is ready to send.
      } //end if buttonstate
    }// end default  

  }// end switch
  ButtonState = 0;
  //  Serial.println("return from 2");
}// end menu void
//*********************** Menu 3 **********************************
void menu3() //playlist, album, artist
{//Select the item in the type
  long timer_pause;
  int x;
  Serial.println("in menu 3");
  switch (ButtonState) //*********************** Menu 3
  {

    // case scan_off: //*********************** Menu 3
  case dial: //*********************** Menu 3
    { //menu or select item go back to choose again
      Serial.println(" scan on");
      IPODsend(SwitchToType,CurrentType,CurrentTypePosition);  //switch to type

      IPODsend(ExecuteSwitch,(long) 0xFFFFFFFF);

      ButtonState = menu_display;
      menu3_wait = 0;

      if(CurrentType==TYPE_PLAYLIST || CurrentType==TYPE_ALBUM) // if CurrentType would be TYPE_SONG; just show songs
      {     
        //        CurrentType=TYPE_SONG;
        Serial.println("selected menu3 -> menu 4");
        set_BusMessage_ipod("Select Song");
        menu4_wait=0; //re get type max etc
        menu_flag = 4; //got to song menu select if song type selected
        return;
      }
      else CurrentType=TYPE_ALBUM;
      PrintCurrentType();  //refresh current type before heading back to Menu2
      menu_flag = 2;
      Serial.println("selected menu3 -> menu 2 for select again");

      return;
    }

  case RT_press: //*********************** Menu 3
    //back to previous menue
    {
      // Serial.println("esc");//debug
      PrintCurrentType();  //refresh current type before heading back to Menu2
      menu_flag = 2;
      ButtonState = menu_display;
      menu3_wait = 0; //set wait for new time through
      return;

    }

  case FFWD: //*********************** Menu 3
    //play
    {
      // Serial.println("play");
      IPODsend(SwitchToType,CurrentType,CurrentTypePosition);  //switch to type
      //       IPODsend(GetTypeNames,CurrentType,CurrentTypePosition,one); //debug
      IPODsend(ExecuteSwitch,(long) 0xFFFFFFFF);
      menu_execute();  //set flag for song display to update
      ButtonState = 0;
      menu3_wait = 0;
      return;
    }

  case left: //*********************** Menu 3  
    { // reduce type by one 
      if(CurrentTypePosition==0) CurrentTypePosition = CurrentTypeMax;
      else CurrentTypePosition --;
      IPODsend(GetTypeNames,CurrentType,CurrentTypePosition,one);  //what is the name of the one we are on now
      break;
    }
  case right: //*********************** Menu 3  
    {// increase type by one 
      if(CurrentTypePosition==CurrentTypeMax) CurrentTypePosition = 0;
      else CurrentTypePosition ++;
      IPODsend(GetTypeNames,CurrentType,CurrentTypePosition,one);  //what is the name of the one we are on now
      break;
    }
  default:
    {
      if(ButtonState > 0 && ButtonState <7)
      {// set type position to x/5 of total
        CurrentTypePosition=CurrentTypeMax  / 5L * (long) (ButtonState-1);
        IPODsend(GetTypeNames,CurrentType,CurrentTypePosition,one);  //what is the name of the one we are on now
      } //end if buttonstate
    }// end default
  }// end switch
  // Ibus_MSG_ready = 1;  //flag text is ready to send
  //  Serial.println("return from 3");
  ButtonState = 0;
}// end menu 3

//*********************** Menu 4 **********************************
void menu4()
{
  Serial.println("in menu 4");
  //get current type

  //print current type
  switch (ButtonState) //*********************** Menu 4
  {

  case FFWD: //*********************** Menu 4 
    // case scan_off: //*********************** Menu 4 
  case dial: //*********************** Menu 4    
    { //play 
      IPODsend(JumptoSong,CurrentItemPosition);  //switch to type
      menu_execute();
      ButtonState = 0;
      menu4_wait = 0;
      return;
    }

  case RT_press: //*********************** Menu 4    
    { // escape the menus
      menu_execute();  //set flag for song display to update
      ButtonState = 0;
      menu4_wait = 0;
      Serial.println("exit -> 1");
      return;
    }

  case left: //*********************** Menu 4
    { // reduce type by one 
      if(CurrentItemPosition==0) CurrentItemPosition = CurrentItemMax; //if already at the first song wrap back to last song
      else CurrentItemPosition --; //or just reduce song
      IPODsend(GetSongTitle,CurrentItemPosition);  //Get number title of what we are on now
      break;
    }
  case right: //*********************** Menu 4
    { // increase type by one 
      //        // Serial.print("type + ");
      if(CurrentItemPosition==CurrentItemMax) CurrentItemPosition = 0; //if already at the last song wrap back to first song
      else CurrentItemPosition ++;
      IPODsend(GetSongTitle,CurrentItemPosition);  //Get number title of what we are on now
      break;
    }

  default:
    {
      if(ButtonState > 0 && ButtonState <7)
      {// set type position to x/5 of total
        CurrentItemPosition=CurrentItemMax / 5L * (long) (ButtonState-1);
        IPODsend(GetSongTitle,CurrentItemPosition);  //Get number title of what we are on now
        Ibus_MSG_ready = 1;  //flag text is ready to send.
        break;
      } //end if buttonstate
    }// end default

  }// end switch
  // Serial.println("return to menu 3");

  menu4_wait = 3; //don't reset the song and redisplay
  ButtonState = 0; //clear the button stored
}// end menu void


//****************** PrintCurrent Type ************
void PrintCurrentType() //based on current type sets the send message
{
  switch (CurrentType)
  {
  case TYPE_PLAYLIST:
    {
      set_BusMessage_ipod("PLAYLIST");
      break;
    }
  case TYPE_ARTIST:
    {
      set_BusMessage_ipod("ARTIST");
      break;
    }
  case TYPE_ALBUM:
    {
      set_BusMessage_ipod("ALBUM");
      break;
    }
  case TYPE_GENRE:
    {
      set_BusMessage_ipod("GENRE");
      break;
    }
  case TYPE_SONG:
    {
      set_BusMessage_ipod("SONG");
      break;
    }
  case TYPE_COMPOSER:
    {
      set_BusMessage_ipod("COMPOSER");
      break;
    }
  }//end switch
}//end void






























































