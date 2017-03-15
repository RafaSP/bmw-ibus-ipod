//**************************** get IPOD data ***********************************************
/*updated and improved read from IPOD.  tested good
 Returns
 499 - still more to come
 498 - no bytes at all
 400 - Message complete and good
 401 - Error in Check sum
 
 data[2]+3 = last data byte (checksum)
 Data[0] & [1] Header
 Data[2] = length byte
 Checksum does not include header
 */

int get_ipod_data()  //read one complete message
{ //versuib with while available at top
  int x = 498;  //set flag for if we have recieved any bytes to no
  while(IPOD.available()) //while data is in the buffer
  {
    //    Serial.println("get ipod data");
    data[ip] = IPOD.read();  //get byte from ipod
#if defined(IPOD_SERIAL_DEBUG)
    Serial.print(data[ip], HEX);  //debug
    Serial.print(" ");  //debug
#endif
    if(ip == 1)
    { 
      if(data[0] == 0xFF && data[1] == 0x55) //if first byte is FF and second byte is 55 then good message recieved
      {
        //this is a good message
      }
      else{//bad message
        IPOD.flush();
        Serial.println(" junk in ipod message stopping");
        ip = 0;
        return 401;
      }
    }
    if(ip == 2) IPODchecksum =  data[2]; //if the byte is the length byte start the checksum
    if(ip>2 && ip < data[2]+3) IPODchecksum  += data[ip];  // if data byte do checksum math

    x = 499;//partial message
    ipod_sent = 0; //set flag to allow next message to be sent
    ip++;  //index for next byte

    if(ip > data[2]+3 && ip > 3) //is the next byte past the end?
    { //Have we recieved all the bytes
      ip = 0; //prepare for next message     
      IPODchecksum = ((0x100 - IPODchecksum) & 0xFF); // Finish check checksum

      if(IPODchecksum == data[data[2]+3]) //is checksum correct
      {
#if defined(IPOD_SERIAL_DEBUG) 
        Serial.println(" good ipod message ");  //debug
#endif
        ProcessMessage();  //if validipod message recieved proces the message.
        return 400; //*****************return good message
      }//end is checksum correct

      Serial.print(" bad message ipod calculated checksum= "); //debug
      Serial.println(IPODchecksum,HEX); //debug
      IPOD.flush(); //clear buffer as something has messed up.
      return 401; //*****************return error
    } //end have we recieved all the bytes
    if(ip >100)
    {
      Serial.println("something has gone wrong with ipod");
      ip = 0;
      IPOD.flush();
      return 401;
    }
  }//end if data is in the buffer
  //  if(x==499){ //debug
  //    Serial.print(ip);  //debug
  //    Serial.println(" =Ip when leaving ipod on 499"); //debug
  //  } //debug
  return x;  //return 99 if partial message 98 if no bytes
}//end get_ipod_data() void
//************************************process message ***********************
//Code by Garrett Smith by himself.
void ProcessMessage()
{ // tested good

  switch(data[5])
  {
  case 0x01: //SwitchToPlaylist0[],SwitchToType[],ExecuteSwitch[],ControlPlayback[],SetShuffleMode[],SetRepeatMode[] feed back on execute command
    {
      if(data[6] != 0x00)
      {
        Serial.print(" ipod failed comand  0x");
        Serial.print(data[8],HEX);
        Serial.print(" error code");
        Serial.println(data[6],HEX);
        IPOD.flush();
        //      IPODsend(Ipod_On); ipod is on or it wouldn't have given failed command
        IPODsend(SwitchtoRemote); //Disable mode 4 //tends to pull ipod our of errors
        Serial.println("disable");
        delay (10);
        IPODsend(SwitchtoAir); //Enable mode 4
        Serial.println("  Enable  ");
        IPODsend(ControlPlayback,PLAYBACK_CONTROL_PLAY_PAUSE); //send play press as enabling pauses ipod
      }
      break;
    }
  case 0x15:  //GetiPodName[] feedback on ipod name
    {  
      OutputText(6);
      for(int i=6;i<data[2]+3;i++)  //start after header and length
      {
        IpodName[i-6]=data[i];
      }
      break;
    }
  case 0x19: //GetTypeCount[]  type count
    {
      CurrentTypeMax = MakeLong(6)- 1;
      Serial.print(CurrentTypeMax);
      Serial.println(" = current type max");
      menu3_wait = 1; //can move to next step in menu3
      break;
    }

  case 0x1B:  //GetTypeNames[]  feedback on current type name
    {  
      OutputText(10);
      for(int i=10;i<data[2]+3;i++)  //start after header and length
      {
        CurrentTypeName[i-10]=data[i];
      }
      Serial.print(CurrentTypeName);
      Serial.println(" = Currenttypea name");
      menu3_wait = 3;  //set flag to recieved type name  Duplicate also in main
      break;
    }

  case 0x1D: //GetTimeAndStatus[]  time and status result
    {
      PlayingStatus = data[14];
      Serial.print("playing status =");
      Serial.println(PlayingStatus);
      if(play_please == 1 && (PlayingStatus ==2 || PlayingStatus ==0))
      {
        Serial.println("sending play press from status to play");
        IPODsend(Play); //send play press
        IPODsend(Button_Release);
      }/*
       if(play_please == 0 && PlayingStatus == 1)
       {
       Serial.println("sending play press from status to stop");
       IPODsend(Play); //send play press
       IPODsend(Button_Release);
       }*/
      LengthOfSong = MakeLong(6);
      TimeElapsed = MakeLong(10); // get new time elapsed

      time_ends = millis() + LengthOfSong - TimeElapsed; // calculate clock time when song ends
      song_message = 0; //completed getting through the song messages
//      new_song = 0; //clear new song flag.

      break;
    }//end time and status

  case 0x1F: //GetPlaylistPosition[]  current position on playlist
    {//CurrentSongNu
      CurrentSongNu = MakeLong(6); // move data of position to variable
      song_message = 2; //ready to get song title next
      break;
    }
  case 0x21:  //GetSongTitle[]  title of song response
    {
      for(int i=6;i<data[2]+3;i++)  //start after header and length
      {
        SongName[i-6]=data[i];
      }
      //      if(SongName[0]== 'T' && SongName[1]== 'h' && SongName[2]== 'e' SongName[3]== ' ') Serial.println("the is at the start");
      song_message = 3; //ready to get song artist next
  //    time_LCD = millis()-10000; //subtract from timer so that it will update LCD next pass.
      update_LCD_now = 1; //set LCD timer so it updates now to refresh title, removed and replaced with timer reset
      LCD_wait=0; //set lcd mode to be at the beginning (title). removed and replaced with timer reset
      menu4_wait = 2; //cleared
      break;
    }
  case 0x23:  //GetSongArtist[]  artist of song response
    {
      for(int i=6;i<data[2]+3;i++)  //start after header and length
      {
        ArtistName[i-6]=data[i];  //move the text bits into artist Name
      }
      song_message = 4; //completed getting through the song messages
      break;
    }
  case 0x25:  //GetSongAlbum[]  album of song rsponse
    {
      //OutputText(6);
      for(int i=6;i<data[2]+3;i++)  //start after header and length
      {
        AlbumName[i-6]=data[i];
      }
      break;
    }
  case 0x27:  //SetPollingmode[]  time elapsed (polling mode)
    {
      TimeElapsed = MakeLong(6);
      break;
    }
  case 0x2D:  //GetShuffleMode[] set shuffle status icon
    {
      CurrentShuffleMode = data[6];
      if(CurrentShuffleMode == 1) 
      {
        Serial.println("shuffle on rec"); //set_BusMessage_ipod("shuffle_on");// 
        B5_random = Rad_random;  //set radio status flag value to random
      }
      if(CurrentShuffleMode == 0) 
      {
        Serial.println("shuffle off rec"); //set_BusMessage_ipod("shuffle_off");// 
        B5_random = 0; //clear radio random B5 status flag value  
      }
      CD_Status[5] = B5_play + B5_random + B5_seek;  //set request flag to play and random flag on or off
      Ibus_set_status_BusMessage(CD_Status);  //must send status for flag to change on screen
      break;
    }
  case 0x30:  //GetRepeatMode[] repeat status
    {
      CurrentRepeatMode = data[6];
      if(CurrentRepeatMode == 1) set_BusMessage_ipod("repeat_on");// Serial.println("shuffle on");// Serial.println("repeat on");
      if(CurrentRepeatMode == 0) set_BusMessage_ipod("repeat_off");// Serial.println("repeat off");
      break;
    }
  case 0x36: //GetNumberOfSongs[] songs on current playlist
    {
      CurrentItemMax = MakeLong(6)- 1;
      menu4_wait = 1; //ready for next step
      Serial.print(CurrentItemMax);
      Serial.println(" = current item max ");
      break;
    }
  }// end switch
}// end void prosses message


















