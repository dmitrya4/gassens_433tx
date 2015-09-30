#include <io.h>
#include <tiny13.h>
#include <tiny13_bits.h>
#include <delay.h>
#include <sleep.h>

#define BitIsClear(reg, bit)    ((reg & (1<<bit)) == 0)
#define SetBit(reg, bit)          reg |= (1<<bit)           
#define ClearBit(reg, bit)       reg &= (~(1<<bit))
#define InvBit(reg, bit)          reg ^= (1<<bit)
#define BitIsSet(reg, bit)       ((reg & (1<<bit)) != 0)

unsigned int m,k, i, j, a, adc_data;
unsigned char txbit1[8]={1,0,1,1,0,0,1,1};
unsigned char txbit2[8]={1,1,1,1,0,0,0,1};
unsigned char txbit3[8]={1,0,0,0,1,1,1,1};

//Тревога, повторяется
void alarm(void){
//Передача сигнала аварии по 433МГц
        PORTB |= (1<<PINB0); //LED On  
    for(i=0;i<1000;i++){  
        //Старт-бит? 
        PORTB |= (1<<PINB4);
        delay_ms(5); //10ms
		PORTB &= ~(1<<PINB4);
        //Передача битов
		  for (m = 0;  m < 8; m++){
		    if(txbit1[m] == 1){
			    PORTB |= (1<<PINB4);
			}
			  else{
			    PORTB &= ~(1<<PINB4);
			  }
				delay_us(300);//5ms
				PORTB &= ~(1<<PINB4);
		  }
                                
          for (m = 0;  m < 8; m++){
		    if(txbit2[m] == 1){
			    PORTB |= (1<<PINB4);
			}
			  else{
			    PORTB &= ~(1<<PINB4);
			  }
				delay_us(300);//5ms
				PORTB &= ~(1<<PINB4);
		  }

          for (m = 0;  m < 8; m++){
		    if(txbit3[m] == 1){
			    PORTB |= (1<<PINB4);
			}
			  else{
			    PORTB &= ~(1<<PINB4);
			  }
				delay_us(300);//5ms
				PORTB &= ~(1<<PINB4);
		  }
        
               delay_ms(15);
    }		 

//Звуковая и световая сигнализация     
  for(k=0;k<5;k++){  
    for(j=0;j<65000;j++){
        PORTB |= (1<<PINB1); //Пищалка вкл. 
        delay_us(145);
       PORTB &= ~(1<<PINB1); //Пищалка выкл. 
        delay_us(145);
     }
   } 
 //#asm("sei");   
}

//Отслеживаем наступление срабатывания датчика по каналу AOUT (DOUT c компаратора)
void check(void){
  if(adc_data > 400){ // сравниваем результат преобразования 400 ~5в,(584 ~3.5в). На входе АЦП 2 вольта.  ADC = (Vin * 1023)/Vref
     alarm();
    }    
         
}

// Pin change interrupt service routine
//Проверка 
//interrupt [PC_INT0] void pin_change_isr(void){
//   }

// ADC interrupt service routine
interrupt [ADC_INT] void adc_isr(void){ 
   PORTB |= (1<<PINB0); //LED On
     delay_ms(100);
     PORTB &= ~(1<<PINB0);//LED Off
     delay_ms(2400);   
 
   for(a=0;a<50;a++) // Make 50 mesure
    {
        ADCSRA |= (1<<ADSC);                      // start new A/D conversion
        while (!(ADCSRA & (1 << ADIF)));      // wait until ADC is ready
        adc_data = adc_data+ADCW;         
    }     
    adc_data = adc_data/50;       
         check();       
}


// Watchdog timeout interrupt service routine
//interrupt [WDT] void wdt_timeout_isr(void){ 
//Греем датчик         
//	 PORTB |= (1<<PINB2);    
//   	 delay_ms(65500);// У watchdog частота меньше и когда стояло 30000 было 52 сек нагрев    
//     ADCSRA |= (1<<ADSC);
//     WDTCR |= (1<<WDTIE);  
//}


void main(void){
//Направление работы порта.
    DDRB |= (1<<PORTB2)|(1<<PORTB1)|(1<<PORTB4)|(1<<PORTB0); //Для подачи питания на датчик, выход на зуммер
//Выкл. аналог.компаратора    
    ACSR=0x80;
//Инициализация АЦП. 
	ADMUX |= (1<<MUX1)|(1<<MUX0)|(0<<REFS0); // ADC3, Vref = Uin.
	ADCSRA |= (1<<ADEN)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); // f = 128кГц          
//Инициализация WatchDog 8sec
//    WDTCR |= (1<<WDP3)|(1<<WDP0);
//	WDTCR |= (1<<WDTIE);
//Инициализация прерывания по кнопке опроса
//	GIMSK |= (1<<PCIE);
//	PCMSK |= (1<<PCINT0);     
//Конфигурация режима сна (PowerDown mode)
//    MCUCR |= (1<<SE)|(1<<SM1);
//Разрешаем глобальные прерывания.
    #asm("sei");             
    
//Греем датчик
   PORTB |= (1<<PINB2);    
   delay_ms(65000);    
   ADCSRA |= (1<<ADSC);   
          
	while(1);//{  
		//Сон.  
//	    #asm("sleep"); //пока отключим сначала калибруемся: 
    //}    
    
    
}
