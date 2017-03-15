//****************************sendHeader**************************************
void sendHeader()
{
#if defined(IPOD_SERIAL_DEBUG)
  // Serial.print("message sending ");  //Debug
#endif
  sendByte(HEADER1);
  sendByte(HEADER2);
  ipod_sent = 1;

}

//***************************sendLength****************************************
void sendLength(size_t length) // length is mode+command+parameters in bytes
{
  sendByte(length);
  checksum = length;
}

//*****************************sendByte**************************************
void sendByte(byte b)
{
  IPOD.write(b);
  checksum += b;

#if defined(IPOD_SERIAL_DEBUG)
  // likely to slow stuff down!
  Serial.print(b, HEX);
  Serial.print(" ");  //Debug
#endif
}

//******************************sendNumber************************************
void sendNumber(unsigned long n)
{
  // parameter (4-byte int sent big-endian)
  sendByte((n & 0xFF000000) >> 24);
  sendByte((n & 0x00FF0000) >> 16);
  sendByte((n & 0x0000FF00) >> 8);
  sendByte((n & 0x000000FF) >> 0);
}

//***************************sendChecksum****************************************
void sendChecksum()
{
//  IPOD.flush();//clear buffer prior to sending last bit of new command
  sendByte((0x100 - checksum) & 0xFF);
#if defined(IPOD_SERIAL_DEBUG)
   Serial.println(" end");  //Debug
#endif
}
//************************** checksum check ***************************************
bool validChecksum(byte actual) //not used 
{
  // Serial.println("checking checksum");//debug
  int expected = dataSize;
  for (int i = 0; i < dataSize; ++i)
  {
    expected += data[i];
  }

  expected = (0x100 - expected) & 0xFF;

  if (expected == actual)
  {
    return true;
  }
  else
  {
#if defined(IPOD_SERIAL_DEBUG)
    // Serial.print("checksum mismatch: expected ");
    // Serial.print(expected, HEX);
    // Serial.print(" but got ");
    // Serial.println(actual, HEX);
#endif

    return false;
  }
}











