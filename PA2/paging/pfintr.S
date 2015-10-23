/* pfintr.S - pfintr */
/* low level page fault handler */

    	   .text
pferrcode: .long 0
           .globl  pfintr 		/* high level interrupt handler     */
           .globl  pferrcode 	/* store interrupt error code    */
pfintr:

    popl pf_error_code  /* pop the pf error code off the stack      */
    pushfl              /* save the eflag register                  */
    cli                 /* disable all interrupts                   */
    pushal              /* save all general purpose registers       */
    call pf_handler     /* call high level pf handler code          */
    popal               /* restore all general purpose registers    */
    popfl               /* restore eflag register                   */
    iret                /* return from interrupt handling           */

