void update_LCD() //called only once from the main program.  Could be rolled intogether but might be useful for menu 4?
{
  
  time_LCD = millis();
  if(update_LCD_now == 1) 
  {
    LCD_wait = 1; //if update now force to title 
    update_LCD_now = 0;//  Clear update now flag
  }// end update now

  if (LCD_wait < 2)   //LCD = song title part a   
  {
    set_BusMessage_ipod(SongName,0); 
    if(r2 > 11) LCD_wait = 2;
    else LCD_wait = 3; 
    return;
  }
 // update_LCD_now = 0;//  Clear update now flag  redundant?
  
  //send song title part 2 if needed
  if (LCD_wait == 2) //LCD = song title part b
  {
    set_BusMessage_ipod(SongName,1); 
    LCD_wait = 3;
    return;
  }
  //send song artist part 1
  if (LCD_wait == 3) //LCD = artist part a
  {
    set_BusMessage_ipod(ArtistName,0);
    if(r2 > 11) LCD_wait = 4;
    else LCD_wait = 1; 
    return;
  }
  //send song artist 2 if needed.
  if (LCD_wait == 4)  //LCD = artist part b
  {
    set_BusMessage_ipod(ArtistName,1);
    LCD_wait = 1 ; //set to does not need to update  
    return;
  }


}//end LCD update void


//*************************** send text to display ************
void OutputText(int textStarts) //supply byte number where the text starts
//for command GetTypeNames(1A) response start at 10 (call this subroutine using 10)
//for command GetSongTitle(20), GetSongArtist(22), GetSongAlbum(24) start at 6
{
  for(int i=textStarts;i<data[2]+3;i++)  //start after header and length
    //  for (size_t i = 0; i < 3; ++i)//was for (size_t i = 0; i < sizeof(dataBuffer); ++i)
  {
    // Serial.write(data[i]); //send to screen
    text[i-textStarts] = data[i];  //set text from IPOD data
    if(i-textStarts == 10) break;
  }
  // Serial.println("");
  set_BusMessage_ipod(text); //call Kbus send
}



