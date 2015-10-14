#include<stdio.h>

extern int etext, edata, end;

void printsegaddress(){
	kprintf("--------------------------------\n");	

	kprintf("(&etext-1):End of the text:\t 0x%x \n",&etext-1); 
	kprintf("(&edata-1):End of the data:\t 0x%x \n", &edata-1);
	kprintf("(&end-1):  End of the BSS:\t 0x%x \n\n", &end-1);

	kprintf("*(&etext-2):Content before the end of text:\t 0x%x \n", *(&etext-2));
        kprintf("*(&edata-2):Content before the end of data:\t 0x%x \n", *(&edata-2));
        kprintf("*(&end-2):  Content before the end of BSS:\t 0x%x \n\n", *(&end-2));

        kprintf("etext:Content after the end of text:\t 0x%x \n", etext);
        kprintf("edata:Content after the end of data:\t 0x%x \n", edata);
        kprintf("end:  Content after the end of BSS:\t 0x%x \n", end);
	kprintf("--------------------------------\n\n");	
}
