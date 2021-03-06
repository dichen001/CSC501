/* paging.h */

#ifndef _PAGING_H_
#define _PAGING_H_

typedef unsigned int	 bsd_t; //The backing store descriptor type is used to refer to a backing store.

/* Structure for a page directory entry */

typedef struct {

  unsigned int pd_pres	: 1;		/* page table present?		*/
  unsigned int pd_write : 1;		/* page is writable?		*/
  unsigned int pd_user	: 1;		/* is use level protection?	*/
  unsigned int pd_pwt	: 1;		/* write through cachine for pt?*/
  unsigned int pd_pcd	: 1;		/* cache disable for this pt?	*/
  unsigned int pd_acc	: 1;		/* page table was accessed?	*/
  unsigned int pd_mbz	: 1;		/* must be zero			*/
  unsigned int pd_fmb	: 1;		/* four MB pages?		*/
  unsigned int pd_global: 1;		/* global (ignored)		*/
  unsigned int pd_avail : 3;		/* for programmer's use		*/
  unsigned int pd_base	: 20;		/* location of page table?	*/
} pd_t;

/* Structure for a page table entry */

typedef struct {

  unsigned int pt_pres	: 1;		/* page is present?		*/
  unsigned int pt_write : 1;		/* page is writable?		*/
  unsigned int pt_user	: 1;		/* is use level protection?	*/
  unsigned int pt_pwt	: 1;		/* write through for this page? */
  unsigned int pt_pcd	: 1;		/* cache disable for this page? */
  unsigned int pt_acc	: 1;		/* page was accessed?		*/
  unsigned int pt_dirty : 1;		/* page was written?		*/
  unsigned int pt_mbz	: 1;		/* must be zero			*/
  unsigned int pt_global: 1;		/* should be zero in 586	*/
  unsigned int pt_avail : 3;		/* for programmer's use		*/
  unsigned int pt_base	: 20;		/* location of page?		*/
} pt_t;

typedef struct{  
  unsigned int pg_offset : 12;		/* page offset			*/
  unsigned int pt_offset : 10;		/* page table offset		*/
  unsigned int pd_offset : 10;		/* page directory offset	*/
} virt_addr_t;

typedef struct frame{
  struct frame *bs_next; // The list of all the frames for this bs
  int fr_id;
  int fr_upper_t ;  // if this frame is a page, it indicates the frm_id of page table
                    // if this frame is a page_table, it indicates the frm_id of page directory.

  int fr_status;      /* MAPPED or UNMAPPED   */
  int fr_pid;       /* process id using this frame  */
  int fr_vpno;        /* corresponding virtual page no*/
  int fr_refcnt;      /* reference count    */
  int fr_type;        /* FR_DIR, FR_TBL, FR_PAGE  */
  int fr_dirty;
  void *cookie;       /* private data structure */
  unsigned long int fr_loadtime;  /* when the page is loaded  */
}fr_map_t;

typedef struct{
  int mapping_num;    /* how many mappings on this bs */
  int private;        /* whether the other process can map or not */
  fr_map_t  *frames;  // the list of frames that maps this bs

  int bs_status;			/* MAPPED or UNMAPPED		*/
  int bs_pid;				/* process id using this slot   */
  int bs_vpno;				/* starting virtual page number */
  int bs_npages;			/* number of pages in the store */
  int bs_sem;				/* semaphore mechanism ?	*/
} bs_map_t;


extern int proc_frames[NPROC][1024];
extern bs_map_t bsm_tab[];
extern fr_map_t frm_tab[];
extern int curr_bs_num;    /* currently used backing store number  */
extern int Enter_From_restore_frame;
/* use some needed function here */
extern void eable_paging();
extern void set_PDBR(int pid);



/* Prototypes for required API calls */
SYSCALL xmmap(int virtpage, bsd_t source, int npages);
SYSCALL xmunmap(int virtpage );

/* given calls for dealing with frames */
SYSCALL init_frm_tab();
int get_frm();
SYSCALL init_frm(int i, int pid, int type);
SYSCALL reset_frm(int i);
SYSCALL free_frm(int i);
SYSCALL find_frm(int pid, int vpno);
SYSCALL dec_frm_refcnt(int pid, int s);
SYSCALL write_back();
SYSCALL recover_back(int pid);

/* given calls for dealing with backing store */
SYSCALL init_bsm(bs_map_t* bs);
SYSCALL get_bsm();
SYSCALL bsm_lookup(int pid, unsigned int vpno, int* store, int* pageth);
SYSCALL bsm_map(int pid, int vpno, int source, int npages);
SYSCALL bsm_unmap(int pid, int vpno, int flag);

int get_bs(bsd_t bs_id, unsigned int npages);
SYSCALL release_bs(bsd_t bs_id);
SYSCALL read_bs(char *, bsd_t, int);
SYSCALL write_bs(char *, bsd_t, int);

/* given calls for dealing with backing store */
SYSCALL init_gpt();
extern  int GDB;      /* for PA2 */

#define NBPG      4096  /* number of bytes per page	*/
#define FRAME0    1024	/* page id of the zero-th frame		*/
#define NFRAMES   1024	/* number of frames for PA2 */
#define NBS       16    /* number of backing store */
#define NBSP      2048  /* number of backing store pages */
#define NPGPBS    128   /* number of pages per backing store */


#define BSM_PRIVATE   1
#define BSM_NOTPRIVATE   0
#define BSM_UNMAPPED	0
#define BSM_MAPPED	1

#define FRM_UNMAPPED	0
#define FRM_MAPPED	1

#define FR_PAGE		0
#define FR_TBL		1
#define FR_DIR		2

#define FIFO		3
#define LRU		4

#define MAX_ID          15              /* You get 16 mappings, 0 - 15 */

//Modified for PA2 
#define BACKING_STORE_BASE	0x00800000         //starting from 2048th page 2^(11+12) = 2^23
#define BACKING_STORE_UNIT_SIZE 0x00080000    //128 pages = 128*4k = 2^(7+12) = 2^19


#define frid2vpno(i)  (FRAME0 + i)          //frame id to virtual page number
#define frid2pa(i)    ( (unsigned int) (FRAME0 + i)*NBPG ) //frame id to physical address
#define pa2frid(i)    ( (unsigned int) ( (i/NBPG) - FRAME0) )
#define bsid2pa(i)    ( (unsigned int) (NBSP+i*NPGPBS)*NBPG ) //backing store id TO physical address.
#define vp2pa(i)      ( (unsigned int) (i*NBPG) )            //virtual page to physical address.
#define a2pno(i)    ( (unsigned int) (i/NBPG) )            //Address to Page Number.  

#endif