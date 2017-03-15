//*********************** send without checksum
void BasicSend(byte *input, byte add)
{
  sendHeader();  //send heater portion of message (constant)
  sendLength(input[0] + add);  //send length (clears checksum variable), adds length for Type or number numbers
  for (int i = 1; i < input[0]+1; i++)  
  {
    sendByte(input[i]);  //sent each message byte (adding to checksum)
  }
}  // end void send

//*****************************IPODsend *************************
//IPOD send then adds checksum ending (no additional types or numberes)
void IPODsend(byte *input)
{
  BasicSend(input, 0x00);  //send command portion of message (input), length adder for Type or number 
  sendChecksum();  //calculate and send the completed checksum
}  // end void Mode2 send

//********************************** IPODsend and type ***************
void IPODsend(byte *input, int type) ////send command, type
{
  BasicSend(input, 0x01);  //send command portion of message (input), length adder for Type or number
  sendByte(type);  //Send the type param
  //  sendNumber(number);  //Send number param
  sendChecksum();  //calculate and send the completed checksum
}

//*********************************IPODsend  number *****************************
void IPODsend(byte *input, long number1)  //send command, number
{
  BasicSend(input,0x04);  //send command portion of message (input), length adder for Type or number
  sendNumber(number1);  //Send number param
  sendChecksum();  //calculate and send the completed checksum
}

//********************************* IPODsend and type, number ***************
void IPODsend(byte *input, int type, long number) //send command, type, numberes
{
  BasicSend(input, 0x05);  //send command portion of message (input), length adder for Type or number
  sendByte(type);  //Send the type param
  sendNumber(number);  //Send number param
  sendChecksum();  //calculate and send the completed checksum
}

//********************************* IPODsend and type, number1, number2 ***************
void IPODsend(byte *input, int type,long number1,long number2)  //send command, type, 2 numberes
{
  BasicSend(input,0x09);  //send command portion of message (input), length adder for Type or number
  sendByte(type);  //Send the type param
  sendNumber(number1);  //Send number param
  sendNumber(number2);  //Send number param
  sendChecksum();  //calculate and send the completed checksum
}

//*************************** combine bits ************
long MakeLong(int i)  //combine the 4 bytes into long starting at bite i
{
  long rar;

  rar += (long)data[i]<<24;
  rar += (long)data[i+1]<<16;
  rar += (long)data[i+2]<<8;
  rar += (long)data[i+3];
  return rar;
}


//************************************ endianConvert
/*
 * iPod is big endian and arduino is little endian,
 * so we must byte swap the iPod's 4-byte integers
 * before we can use them on the arduino.
 * note that on arduino int is 16-bit and long is 32-bit.
 * TODO: Is that true of all the micrcontrollers on
 * arduino boards?
 */
unsigned long endianConvert(const byte *p)
{
  return
    (((unsigned long) p[3]) << 0)  |
    (((unsigned long) p[2]) << 8)  |
    (((unsigned long) p[1]) << 16) |
    (((unsigned long) p[0]) << 24);
}




