void send(char*);
void receive();
char formlist[100];
char data_rec[100];
int respuestaid        = 0;
int MICRO_ADDR         = 10; //Verificar
int flag_uart          = 0;
char SerRx;
int data_len_rx        = 0;
int flag_accion        = 0;

void setup() {
  
    Serial.begin(9600);
}

void loop() {

    if (Serial.available() > 0){
        receive();
    }
    
}

void send(char* data_send)
{
    int data_len     = 0;
    char caracter    = 0;
    int header       = 0;
    int iheader      = 0;
    int PL           = 0;
    int mod_header   = 0;
    int mod_iheader  = 0;
    int size_l       = 0;
    int size_h       = 0;

    header   = 160;
    iheader  = 64;

    while(caracter!='!')
    {
        caracter = data_send[data_len];
        data_len++;
    }    
    data_len--;

    PL          = 16;
    mod_header  = header  + PL + ((data_len & 0xf0000)>>16);
    mod_iheader = iheader;
    size_l      = data_len & 0xff;
    size_h      = (data_len & 0xff00)>>8;
    formlist[0] = mod_header;
    formlist[1] = size_h;
    formlist[2] = size_l;
    formlist[3] = respuestaid;
    formlist[4] = MICRO_ADDR;

    for( int i = 5 ; i < 5 + data_len ; i++)
    {
        formlist[i] = data_send[i-5];
    }
    formlist[5+data_len] = mod_iheader;

    for(int i = 0 ; i < data_len + 6; i++)
    {
        Serial.println(formlist[i]);
        delay(1);
    }

}

void receive(){
  
     SerRx = Serial.read();
     //Serial.println(SerRx);
     if( ( flag_uart == 0 ) && ( (SerRx&0xE0) == 160) )//Encontramos el MIT
     {
        flag_uart++;
        data_len_rx = (SerRx & 0x0F);
        //Serial.println(SerRx);
     }
     else if ( flag_uart == 1 )
     {
         data_len_rx = data_len_rx + (SerRx<<8);
         flag_uart++;
         //Serial.println(SerRx);
     }
     else if ( flag_uart == 2 )
     {
         data_len_rx = data_len_rx + SerRx;
         flag_uart++;
         //Serial.println(SerRx);

     }
     else if( ( SerRx == MICRO_ADDR )&& ( flag_uart == 3 ) )
     {
        flag_uart++;
        //Serial.println(SerRx);
     }
     else if( flag_uart == 4 )
     {
        respuestaid = SerRx;
        flag_uart++;
        //Serial.println(SerRx);
     }
     else if ( (flag_uart > 4) && (flag_uart < 5 + data_len_rx) )
     {
         data_rec[flag_uart-5]=SerRx;
         flag_uart++;
         //Serial.println(SerRx);
     }
     else if( (SerRx == 64) && ( flag_uart == (5 + data_len_rx) ) )//termina la recepción de la trama
     {
        flag_uart = 0;
        //for(i = 0; i < data_len_rx; i++)
        //{
        //    Serial.println(data_rec[i]);    //Mandamos los datos que hemos recibido
        //}
       flag_accion = 1;   //Ejecutamos la accion en el loop principal
       
     }
     else flag_uart = 0;

 }

