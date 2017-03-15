//*****************************get Ibus data **********************
/*possible returns 
 399 - Bytes but not complete
 398 - no bytes at all
 300 - Message complete and good
 301 - Error in Check sum
 1,2,3,4 etc are button values
 
 Kdata[0] = from module
 Kdata[1] = Length of data bits + length and To
 Kdata[2] = to module
 Kdata[1] + 1 =  Length of data bits + 1 = last byte (checksum)
 */

void get_Kbus_data()
{
  byte CRC;



  // check if the Time between last message and now is > x then kb=0 (start new message) 
  //  if(kb > 0 && millis()-time_Kbus > 5 && bus_started == 0) //else check to see if last byte recieved was too long ago (only if buss not yet initialized) 
  //  {
  //    kb=0;
  //    Serial.println("new message based on time");
  //  }
int r = Kbus.read(); //Move data from buffer to variable
if(r == -1)return;

  Kdata[kb] = (byte)r;
  if(Kdata[0] == 0x18 || Kdata[0] == 0xC8) //do not reset bus idle timer if arduino sent the message.
  { //do nothing
  }
  else  time_Kbus = micros(); //reset timer if not our message
 
 // time_Kbus = micros(); //reset timer if buss message recieved
  Ibus_x=399; //set flag byte recieved (but not finished)

#if defined(Kbus_SERIAL_DEBUG)  //debug
  //  if(Kdata[0] == 0x68 || Kdata[0] == 0x50 || Kdata[0] == 0x80){ //print only radio and wheel messages
  Serial.print(Kdata[kb],HEX);  //debug
  Serial.print(" ");  //debug
  //  } // end if print only radio and wheel messages
#endif  //debug
  //checking message to see if typical error in error status sending has happened
  //this will not fix the bus send issue but will keep the program from loosing the bus connection. also opened up the length rejection to be always
  if(bus_started == 1 && kb == 1 && Kdata[0] == CD_Status[2] && Kdata[1] == CD_Status[3]) //dump data if bus not initialized and length byte looks too long
  {
    Kdata[3] = CD_Status[3]; // 0x39
    Kdata[2] = CD_Status[2]; //0x68
    Kdata[1] = CD_Status[1]; //0x0A; 
    Kdata[0] = CD_Status[0]; //0x18;
    kb = 3;
    Serial.println("18 A missing error");
    //this will not fix the bus send issue but will keep the program from loosing the bus connection. also opened up the length rejection to be always
  }

  //  if(bus_started == 0 && kb == 1 && Kdata[1] > 0x10) //dump data if bus not initialized and length byte looks too long
  if(kb == 1 && Kdata[1] > 0x15) //dump data if bus not initialized and length byte looks too long
  {
    Serial.println(" ibus junk length during initializing so stopping");
    Kbus.flush();
    kb=0;
    return; //leave void and wait for next byte
  }
  if(kb == 1 && Kdata[1] == 0) //Dump if length byte is zero
  {
    Serial.println(" ibus zero length initializing so stopping");
    Kbus.flush();
    kb=0;
    return;//leave void and wait for next byte
  }

  kb++; //index counter to be ready for next read

  if (kb>(Kdata[1]+1))//have we received all bytes
  { //is next byte to be recieved over max length
    kb=0; //Reset counter for next message
    CRC = iso_checksum(Kdata,Kdata[1]+1);  //calculate the CRC value

    if(CRC == Kdata[Kdata[1]+1]) //Check if CRC in data matches calculated
    {// if CRC matches it is a good message

#if defined(Kbus_SERIAL_DEBUG)  //debug
      //     if(Kdata[0] == 0x68 || Kdata[0] == 0x50 || Kdata[0] == 0x80){ //print only radio, wheel messages, IKE
      Serial.println(" ibus good");//debug 
      //      } // end if print only radio and wheel messages
#endif  //debug

      if(bus_started == 0) //if bus wasn't yet initialized
      {
        Serial.println(" ibus now initialized ");//debug
        //mimiking power off reset.  send CD announce to alert the Radio that the CD is ready then stop pause status.
        Ibus_set_status_BusMessage(CD_Announce); // load CD announce
        CD_Status[4] = B4_stop;  //set status flag to Pause
        CD_Status[5] = B5_pause + B5_random + B5_seek;  //set request flag to Pause + seek and random flag
        Ibus_set_status2_BusMessage(CD_Status); //load CD status into 2nd message ready to send
      } //end if first time through bus initialization
      
      bus_started = 1; //bus initialization complete flag
      Ibus_x=300; //300 - Message complete and good

      if(Kdata[0] == 0x68 && Kdata[2] == 0x18) radio_message();//if message recieved and was from the radio to CD take acctions      
      // +++++++++++++++++++++++++++++++++ call to process Radio message
      if(Kdata[0] == 0x50) wheel_message();//if message recieved and was from the radio to CD take acctions      
      // +++++++++++++++++++++++++++++++++ call to process wheel message
      if(Kdata[0] == 0x00 && Kdata[1] == 0x04 && Kdata[2] == 0xBF && Kdata[3] == 0x72) //if remote button pressed (locking car)
 //     (Kdata[0] == 0x00 && Kdata[1] == 0x05 && Kdata[2] == 0xBF && Kdata[3] == 0x7A && Kdata[4] == 0x10 && Kdata[5] == 0x0C))
          {
//        if(menu_flag >0) ipod_stop(); //Stop ipod
        Serial.println("car on/off");
//        delay(100);
//        digitalWrite(8,HIGH); //send turn Power off ALL signal
//        delay(10); //should not pass here if using self power off
//        digitalWrite(8,LOW); //reset to on (if using USB cable and Arduino didn't really go off)
//        time_Kbus = micros() - 60000;  //should not pass here if using self power off
      }//if car on/off
    
     
      if(Kdata[0] == 0x80 && Kdata[1] == 0x04 && Kdata[2] == 0xBF && Kdata[3] == 0x11 && Kdata[4] == 0x00)
      //if IKE turns off, stop ipod (maybe should use radio on/off?
      {
        if(menu_flag >0) ipod_stop(); //Stop ipod
        Serial.println("ike off");
  //      delay(100);
  //      digitalWrite(8,HIGH); //send turn Power off ALL signal
  //      delay(10);
  //      digitalWrite(8,LOW); //reset to on (if using USB cable and Arduino didn't really go off)
  //      time_Kbus = millis() - 60000;  //should not pass here if using self power off
      }//if IKE off
      time_Kbus = micros(); //capture time when message recieved (duplicate to ensure program process time doesn't cause the kick out)

      return; //return good message
    } //end CRC if 

    //if check sum failed (does not need to be an else as cannot pass here due to return)
    Kbus.flush(); //wipe out buffer when errors
    kb=0;
    Serial.print(" ibus CRC fail CRC= "); //debug
    Serial.println(CRC,HEX); //debug
    bus_started = 0; //ibus not initialized
    Ibus_x=301; //301 - Error in Check sum
    return; // return fail
    //cannot pass here must exit 300 or 301   
  } // End if have all bytes

  //only passes here if NOT all bytes rec'd  

  return; //return 399 if partial message 398 if no bytes
}// end get K bus

//**************************************Wheel message***************************
void wheel_message()
{
  if(Kdata[2] == 0x68 && Kdata[3] == 0x32)//if it volume button
  {  
    return;
  }

  if(Kdata[2] == 0x68 && Kdata[3] == 0x3B)//if it is a radio wheel skip button
  {
    //   if(Kdata[4] == 0x01) ButtonState = right; //skip forward
    //   if(Kdata[4] == 0x08) ButtonState = left; //skip back
    if(Kdata[4] == 0x11) ButtonState = FFWD; //Up held Fast FWD
    if(Kdata[4] == 0x18) ButtonState = FRWD; //down held Fast RWD
    if(Kdata[4] == 0x21 || Kdata[4] == 0x28) //pressed play **command //next or previous button released
    { 
      //    rad_command = play; 
      time_LCD = millis()-10000; //subtract from timer so that it will update LCD next pass. //refresh lcd screen as button release erases it.
    }
  }
  //
  if(Kdata[2] == 0xC8)//if it is a telephone button
  {
    if(Kdata[3] == 0x01) ButtonState = RT_press; //R/T pressed (execute)
    if(Kdata[4] == 0x80) ButtonState = dial; //Dial Pressed (menu)
    if(Kdata[4] == 0x90) ButtonState = FFWD; //Dial Held (execute?)
    if(Kdata[4] == 0xA0) rad_command = play; //pressed play **command //Dial Released
  }
  CD_status(); //act on messages
}//end wheel message void

//**************************************radio message***************************
void radio_message()
{
  if(Kdata[3] == 0x01) // Message recieved is poll CD.
  {  
    //Serial.println(" response to CD status request"); //debug
    Ibus_set_status_BusMessage(CD_Respond);
    return;
  }

  if(Kdata[3] == 0x38)//if it is a radio button 1-6
  { //rad commands changes CD status message and actions.  ButtonStates only for use in controlling the menus and ipod.
    if(Kdata[4] == 0x00 && Kdata[5] == 0x00) rad_command = cd_stop_pause; //rec'd stop_pause  **command
    if(Kdata[4] == 0x01 && Kdata[5] == 0x00) rad_command = cd_stop_pause; //rec'd stop  **command
    if(Kdata[4] == 0x02 && Kdata[5] == 0x00) rad_command = cd_pause; //rec'd pause **command
    if(Kdata[4] == 0x03 && Kdata[5] == 0x00) rad_command = play; //rec'd play **command

    if(Kdata[4] == 0x04 && Kdata[5] == 0x00) ButtonState = FRWD; //pressed Fast RWD
    if(Kdata[4] == 0x04 && Kdata[5] == 0x01) ButtonState = FFWD; //pressed Fast FWD

      if(Kdata[4] == 0x06 && Kdata[5] == 0x01) ButtonState = one; // pressed 1 "); //check the value
    if(Kdata[4] == 0x06 && Kdata[5] == 0x02) ButtonState = two; // pressed 2 "); //check the value
    if(Kdata[4] == 0x06 && Kdata[5] == 0x03) ButtonState = three; // pressed 3 "); //check the value
    if(Kdata[4] == 0x06 && Kdata[5] == 0x04) ButtonState = four; // pressed 4 "); //check the value
    if(Kdata[4] == 0x06 && Kdata[5] == 0x05) ButtonState = five; // pressed 5 "); //check the value
    if(Kdata[4] == 0x06 && Kdata[5] == 0x06) ButtonState = six; // pressed 6 "); //check the value

    if(Kdata[4] == 0x05 && Kdata[5] == 0x01) ButtonState = left; //skip back
    if(Kdata[4] == 0x05 && Kdata[5] == 0x00) ButtonState = right; //skip forward
    if(Kdata[4] == 0x0A && Kdata[5] == 0x01) ButtonState = left; //skip back
    if(Kdata[4] == 0x0A && Kdata[5] == 0x00) ButtonState = right; //skip forward

    if(Kdata[4] == 0x07 && Kdata[5] == 0x01) rad_command = scan_on; //check the value //pressed random
    if(Kdata[4] == 0x07 && Kdata[5] == 0x00) rad_command = scan_off; //check the value //pressed random

    if(Kdata[4] == 0x08 && Kdata[5] == 0x01) ButtonState = random_on; //check the value //pressed random 
    if(Kdata[4] == 0x08 && Kdata[5] == 0x00) rad_command = random_off; //check the value //pressed random or radio decided to send random off

      //      Serial.print(ButtonState); //debug
    //      Serial.println(" =button State"); //debug
  }//end B3 = 38
  if (rad_command > 0) CD_status(); //if it was a Radio command (not just menu button) act on messages
}// end Radio message void

//************************************** CD status ***************************
void CD_status()
{ // act on buttons or commands
  Serial.println("in CD status");//debug
  //********************** CD scan on **************
  if(rad_command == scan_on) //only set the flag.  Do nothing else 
  {
    B5_seek = 0x10;  // 0x10 makes scan status

    Serial.println("CD scan_on ");
  }//end cd scan on

    //********************** CD scan on **************
  if(rad_command == scan_off) //only set the flag.  Do nothing else 
  {
    B5_seek = 0x00;  // 0x10 makes scan status
    Serial.println("CD scan_off");
  }//end cd scan on

    //********************** CD scan on **************
  if(rad_command == random_off) //only set the flag.  Do nothing else 
  {
    B5_random = 0x00;  // 0x10 makes random on,  0x00 is random off
    Serial.println("CD random_off");
  }//end cd scan on

    //********************** CD stop **************
  if(rad_command == cd_stop) //means end of CD mode
  { 
    CD_Status[4] = B4_stop;  //set status to stop
    B5_status = B5_stop;
    Serial.println("CD stopped ");
    ipod_stop();  //stop ipod

    /*    if(menu_flag > 0) //if in CD mode (ipod mode) exit ipod mode 
     { 
     ipod_stop();  //stop ipod
     }//end if Menu >0
     else ipod_start(); //if in menu_flag == 0 then must be initialising CD, then start ipod 
     */
  } //end if cd_stop

    //********************** CD Stop pause **************
  if(rad_command == cd_stop_pause) //sent when radio wants to go to ipod mode (does this not send on the tape model?)
  {
    CD_Status[4] = B4_stop;  //set status flag to Pause
    B5_status = B5_pause;  //set request flag to pause and random flag on or off
    Serial.print("CD stop_pause1 menu flag = ");
    //    ipod_start(); //start up ipod if not already started

    Serial.print(menu_flag);
    Serial.println(" ");
    Serial.print("playing status (stop pause) =");
    Serial.println(PlayingStatus);

  }//end cd stop pause


  //********************** CD pause **************
  if(rad_command == cd_pause && menu_flag <2) //when is this sent?
  {
    CD_Status[4] = B4_pause;  //set status flag to pause
    B5_status = B5_pause;  //set request flag to pause  
    Serial.println("CD pause ");
    //    ipod_start(); //start up ipod if not already started

  }//end cd pause


  //********************** CD play **************
  if(rad_command == play) // && menu_flag <2) // when is this sent?
  {
    CD_Status[4] = B4_play;  //set status flag to play
    B5_status = B5_play; //set request flag to play 
    Serial.println("CD Play");
    ipod_start(); //start up ipod if not already started

    /*       if(menu_flag == 1) //to stop a forward scanning button.
     { //if command in root menu
     if(ButtonState2 == FFWD || ButtonState2 == FRWD) //if last button pressed was Fast forward or reverse
     {
     IPODsend(ControlPlayback,PLAYBACK_CONTROL_STOP_FF_OR_REV); //end fast mode
     //       set_BusMessage_ipod("Play Resume"); //removed for testing.
     ButtonState2 = play; // store status messages
     }
     else  keep_playing();
     }//end menu_flag 1 */

    /*     if(menu_flag > 1)// if radio sends play while in a menu ignore and update message on LCD
     {
     Serial.println("CD Play, menu_flag > 1");
     Ibus_MSG_ready = 1;
     } */

  }//end if play 

  CD_Status[5] = B5_status + B5_random + B5_seek;  //set request flag to playing status, seek and random flag
  Ibus_set_status_BusMessage(CD_Status); //load status message into Ibus send variable //send status (including random flags)
  //  time_CD = millis(); //set time_CD. status timer
  rad_command = 0; //clear Radio command flag.  (Not a button)

}//end CD status



// suggest finding when the commands are sent and when.  Look for when CD mode is started and set menu flag to 1 and start ipod playing.
// Look for when CD mode is stopped and put menu flag to zero and stop ipod


//************************  Ipod_start *******************
void ipod_start() // Start ipod when Radio switches to CD 
{
  Serial.println("ipod start");
  play_please = 1; //maybe?
  if(menu_flag == 0) //if switching to CD from radio mode start up ipod and menus
  { 
    menu_flag = 1; //enable menu 1 buttons
    Serial.println("  Enable in ipod start  ");
    IPODsend(Ipod_On); //do these three need to be on their own? (check for ipod activity then boot up?)
    IPODsend(Button_Release);
    IPODsend(SwitchtoAir); //Enable mode 4 (check for mode first?)

    IPODsend(ControlPlayback,PLAYBACK_CONTROL_PLAY_PAUSE); //send play press as enabling pauses ipod
    song_message = 1; //Initiate getting Song title, artist etc.
    time_ends = millis() + 8000;  //give ipod 8 seconds to get title, artist and status.
    //   Serial.print("song message = ");
    //   Serial.println(song_message);
  }

}//end ipod start


//************************  Ipod_stop *******************
void ipod_stop() // stop ipod when car or radio turned off 
{
  Serial.println("ipod stop");

  if(menu_flag >0)
  {
    play_please = 0;

    if(PlayingStatus == STATUS_PLAYING) //if playing press pause
    {
      Serial.println("sending pause from ipod stop");
      IPODsend(ControlPlayback,PLAYBACK_CONTROL_PLAY_PAUSE); //send play/pause press
      //    set_BusMessage_ipod("Normal Play"); 
    }//end if

    Serial.println("ipod off");
    //below should be part of the status.... IKE off statement is sent here.  Find what CD changer does
    CD_Status[4] = B4_stop;  //set status to stop
    CD_Status[5] = B5_stop + B5_random + B5_seek;  //set flag to stop and random flag on or off
    //  IPODsend(SwitchtoRemote); //Disable mode 4 //tends to pull ipod our of errors

      Ibus_set_status_BusMessage(clear_screen);

    //   Ibus_set_status_BusMessage(clear_screen); //load message into Ibus send variable 
    menu_flag=0; //stop message sending 
  }
  else Serial.println("already stopped");
}//end ipod stop


































































































