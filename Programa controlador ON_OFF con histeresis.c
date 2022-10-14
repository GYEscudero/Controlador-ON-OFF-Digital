////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Laboratorio: Control Avanzado   Gpo: 1909A                                 //
// Practica 1 "Controlador ON-OFF con Hitéresis"                              //
// Profesor: Fernando Gudiño Peñaloza                                         //
// Alumno: Escudero Bejarano Gerson Yaser                                     //
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                          CONFIGURACION DEL PIC16F887                       //
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#include   <16f887.h>      //Libreria MCU PIC16F887
#device adc=10             //Convertido analogico digital de 10 bits
#use delay(clock=8M)       //Configura Reloj interno a 8MHz
#fuses INTRC_IO,FCMEN,IESO //Fuses del Oscilador
#fuses NOWDT,BROWNOUT,PUT,MCLR,  //Fuses del reset
#fuses NOPROTECT,NOCPD     //Fuses de Protección Códigos
#include <lcd_C_ON-OFF.c>  //Libreria de pantalla LCD
#use fast_io(A)            //Configura puerto A
#use fast_io(B)            //Configura puerto B
////////////////////////////////////////////////////////////////////////////////
//                         EMPIEZA PROGRAMA PRINCIPAL                         //
////////////////////////////////////////////////////////////////////////////////
void main () // funcion principal 
{
//----------------------------------------------------------------------------//
//------------------------Declaración de variables----------------------------//
//----------------------------------------------------------------------------//
   int y=0, x=0;                       //Variable entera de 8 bits
   int16 Conversion, ADC0, ADC1;       //Variable entera de 16 bits
   float32 Voltaje, Celsius, Vm, Tm;   //Variable flotante de 32 bits 
   float32 VHis,THis,TH,TL,AH;         //Variable flotante de 32 bits 
//----------------------------------------------------------------------------//
//-----------------------Configuración de pines I/O---------------------------//
//----------------------------------------------------------------------------//
   set_tris_A(0X07);    //Configura puerto A (00000111)
                        //PuertoA PinA0 - PinA2 ==> Entrada
                        //PuertoA PinA3-PinA7 ==> Salida
   set_tris_B(0X03);    //Configura puerto A (00000011)    
                        //PuertoA PinB0 y PinB0 ==> Entrada
                        //PuertoA PinB2-PinB7 ==> Salida
   OUTPUT_LOW(PIN_B2);  //Mantener apagado el pin B2
   OUTPUT_LOW(PIN_B3);  //Mantener apagado el pin B3  
//----------------------------------------------------------------------------// 
//---------------------------Textos de Bienvenida-----------------------------// 
//----------------------------------------------------------------------------//
   lcd_init();                                  //Inicializa el LCD
   printf(lcd_putc, "\f   BIENVENIDOS    \n");  //Imprime texto en LCD
   printf(lcd_putc, "          ");              //Imprime espacio en blanco
   delay_ms(200);                               //Tiempo de retardo del texto
   printf(lcd_putc, "\f  Lab. Control    \n");  //Imprime texto en LCD
   printf(lcd_putc, "    Avanzado     ");       //Imprime texto en LCD 
   delay_ms(200);                               //Tiempo de retardo del texto
   printf(lcd_putc, "\f   Controlador    \n");  //Imprime texto en LCD
   printf(lcd_putc, "     ON/OFF     ");        //Imprime texto en LCD
   delay_ms(200);                               //Tiempo de retardo del texto
   printf(lcd_putc, "\fOprima el PIN_B0 \n" );  //Imprime texto en                                                               //LCD      
   delay_ms(100);                               //Tiempo de retardo del texto 
   printf(lcd_putc, "\n");                      //Indica que el siguiente texto 
                                                //a imprimir en el LCD se 
                                                //imprime en el renglon superior
   while (input(pin_B0)==1)   //Al precionar el botón en el pin b0 se rompe el 
                              //ciclo
   {
      printf(lcd_putc, "\npara  ajustar H. ");  //Imprime texto en LCD  
   } 
//----------------------------------------------------------------------------//           
//----------------Configuración de los parametros de Hitéresis----------------//
//----------------------------------------------------------------------------//      
   printf(lcd_putc, "\f-T.  Histeresis-");   //Imprime texto en LCD     
   while (y==0)   //Ciclo para ajuste de parametros de Histéresis
   {
      delay_ms(22);  //Tiempo de sincronizacion para detectar "pin_B1=0"
      if (input(pin_B1)==0)   //Si se preciona el boton en el pin b1, se cumple
                              //la condición
      {
         x++;        //se incrementa el valor de x
         if (x>=6)   //Si x es igual o mayor a 6
         {
            x=0;     //x se va a 0
         }
      } 
      switch (x)  //sentencia switch con referencia a "x"
         {
         case 0:  //Caso 0: ajuste de temperatura de referencia Tm 
            setup_adc(ADC_CLOCK_INTERNAL);   //Configuracion del Convertidor con 
                                             //reloj  interno
            setup_adc_ports(sAN0);  //Activar entradas analogica del pin A0   
            set_adc_channel(0);  //Canal para A/D (PIN A0) (Se activa)
            delay_us(10);        //Tiempo de espera para el A/D para darle 
                                 //tiempo detrabajar (recomienda 10us)                 
            ADC0=read_adc();  //el valor el CAD0 se valga a la variable ADC0
            Vm=5*ADC0/1024.0; //Determinar el valor de la variable
                              //Vm=(ADC/2^n)*Vpp donde Vpp=5V, Vpp es 
                              //determinado por el voltaje de sat. del 
                              //operacional. ADC0=Valor binario                                     
            Tm=Vm/0.05;       //Relacion temperatura-voltaje del LM35 (en °C)        
            printf(lcd_putc, "\nTref=%3.2f%cC    ",Tm,0xDF);   //Imprime texto 
                                                               //en LCD  
         break;   //Termina caso 0         
         case 1:  //Caso 1: ajuste del ancho de temperatura de Histéresis AH                  
            setup_adc(ADC_CLOCK_INTERNAL);   //Configuracion del Convertidor con 
                                             //reloj  interno
            setup_adc_ports(sAN1);  //Activar entradas analogica del pin A1   
            set_adc_channel(1);     //Canal para A/D (PIN A1) (Se activa)
            delay_us(10);  //Tiempo de espera para el A/D para darle tiempo de
                           //trabajar (recomienda 10us)           
            ADC1=read_adc();  //el valor el CAD0 se valga a la variable ADC0
            VHis=5*ADC1/1024.0; //Determinar el valor de la variable en (V)                   
            THis=(VHis/0.5);  //Variable qu ayuda a determinar el ancho de 
                              //temperatura de la histeresis
            TL=Tm-THis; //Temperatura baja de la histéresis
            TH=Tm+THis; //Temperatura alta de la histéresis 
            AH=TH-TL;   //Ancho de la temperatur de histeresis (Maxima de 20°C)
            printf(lcd_putc, "\nAncho H.=%3.2f%cC",AH,0xDF);   //Imprime texto 
                                                               //en LCD                 
         break;   //Termina caso 1 
         case 2:  //Caso 2: Texto de Temperatura alta de histéresis TH
            printf(lcd_putc, "\n  TH=%3.2f%cC   ",TH,0xDF); //Imprime texto en  
                                                            //LCD                  
         break;   //Termina caso 2 
         case 3:  //Caso 3: Texto de Temperatura baja de histéresis TL
            printf(lcd_putc, "\n  TL=%3.2f%cC   ",TL,0xDF); //Imprime texto en  
                                                            //LCD                 
         break;   //Termina caso 3 
         case 4:  //Caso 4: Texto de reiniciar a editar parametros de histeresis
            printf(lcd_putc, "\nReiniciar PIN_B1");  //Imprime texto en LCD
            delay_ms(5); 
            if (input(pin_B0)==0)
            {
               y=1;         
            }            
         break;
         case 5:
            printf(lcd_putc, "\nEmpezar PIN_B0  ");
            delay_ms(5);   //Tiempo de sincronizacion para detectar "pin_B0=0" 
            if (input(pin_B0)==0)   //Si se preciona el boton en el pin b0, se 
                                    //cumple la condición
            {
               y=1;         
            }            
         break;
         } 
   }          
//----------------------------------------------------------------------------// 
//----------------Ciclo de trabajo A/D e impresión de texto en LCD------------//
//----------------------------------------------------------------------------//
    printf(lcd_putc, "\fParametros");  //Imprime texto en LCD 
   setup_adc(ADC_CLOCK_INTERNAL);   //Configuracion del Convertidor con reloj
                                    //interno
   setup_adc_ports(sAN2);           //Activar entradas analogica del pin A2
   set_adc_channel(2);  //Canal para A/D (PIN A2) (Se activa)
   delay_us(10);        //Tiempo de espera para el A/D para darle tiempo de
                        //trabajar (recomienda 10us)
   while (true) //Ciclo infinito
   {
               
      //Calculo para el SETPOINT ADC=[Celsius*0.05*2^n]/Vpp   
      //Para 60°C =624.64=625
      if(Celsius<=TH)   //Si el valor del ADC es igual o menor a 625 se
                              //cumple la siguiente sentencia
      {
         Conversion=read_adc();  //Lo que registre en el A/D, se guardara en la 
                                 //variable "Conversion" Conversion=ADC 
         Voltaje=5*Conversion/1024.0;//Determinar el valor de la variable
                                     //Voltaje=(ADC/2^n)*Vpp donde Vpp=5V
                                     //Vpp es determinado por el voltaje de sat.
                                     //del operacional
                                     //ADC=Valor binario
         Celsius=Voltaje/0.05;   //Determinar el valor de la variable Celsius 
         printf(lcd_putc, "\nT=%3.2f%cC ",Celsius,0xDF); //Texto de medición de
                                                       //Temperatora en °C
         //printf(lcd_putc, "  V=%1.2fV ",Voltaje); //Texto alterno de Voltaje
         //printf(lcd_putc, "\f ADC=%4ld",Conversion); //Texto alterno de ADC
         printf(lcd_putc, "\S= ON      f");  //Estado apagado del controlador en
                                             //ON
         OUTPUT_HIGH(PIN_B2); //Mantener en alto Pin B1
         OUTPUT_HIGH(PIN_B3); //Mantener en alto Pin B2
      }      
      else //Si el valor del ADC no es menor o igual a 625 se cumple la 
           //siguiente sentencia
      {
         Conversion=read_adc();  //Lo que registre en el A/D, se guardara en la 
                                 //variable "Conversion" Conversion=ADC 
         Voltaje=5*Conversion/1024.0;//Determinar el valor de la variable
                                     //Voltaje=(ADC/2^n)*Vpp donde Vpp=5V
                                     //Vpp es determinado por el voltaje de sat.
                                     //del operacional
                                     //ADC=Valor binario
         Celsius=Voltaje/0.05;    //Determinar el valor de la variable Celsius 
         printf(lcd_putc, "\nT=%3.2f%cC ",Celsius,0xDF); //Texto de medición de
                                                       //Temperatora en °C
         //printf(lcd_putc, "  V=%1.2fV ",Voltaje); //Texto alterno de Voltaje
         //printf(lcd_putc, "\f ADC=%4ld",Conversion); //Texto alterno de ADC
         printf(lcd_putc, "\S: OFF     ");  //Estado apagado del controlador en
                                             //OFF
         OUTPUT_LOW(PIN_B2); //Mantener en bajo Pin B1
         OUTPUT_LOW(PIN_B3); //Mantener en bajo Pin B2    
      }                       
   }
}
//--------------------------------Fin del programa----------------------------//
