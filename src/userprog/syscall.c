#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

#include <devices/shutdown.h>
#include <threads/thread.h>
#include <filesys/filesys.h>

#include "threads/vaddr.h"

#include "devices/input.h"

static void syscall_handler (struct intr_frame *f);

struct lock filesys_lock;

void
syscall_init (void) 
{
  //printf("sibal");
	
  lock_init(&filesys_lock);	
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  // 1. 스택 포인터 -> 2. 스택포인터가 유저영역인지, check_address로 확인 -> 3. 스택에 저장된 인자값이 포인터면 check_address로 확인 (이거 다 get_argument에서 하는듯)
  int *sp = (int *)f-> esp;
  //printf("sibal");
  //printf("shitsibal:%d\n", *sp);

  check_address(sp);

  int systemcall_number = *sp; // sp가 주소 // systemcall_number 가 숫자
  //printf("shitsibal:%d\n",systemcall_number);
  switch(systemcall_number){
  	case SYS_HALT: //0
  		halt();
  		break;
  	case SYS_EXIT: //1
  		check_address(sp+1);
  		exit(*(uint32_t *)(sp + 1));
  		break;
  	case SYS_EXEC: //2
  		check_address((void *)*(sp+1));
  		check_address(sp+1);
  		f->eax = exec((char *)*(uint32_t *)(sp+1));
  		break;
  	case SYS_WAIT: //3
  		//check_address((void *)*(sp+1));
  		check_address(sp+1);
  		f->eax = wait((pid_t)*(uint32_t *)(sp+1));
  		break;
  	case SYS_CREATE: //4
  		check_address(sp+2);
  		check_address((void *)*(sp+1));
  		f->eax = create((const char *)*(uint32_t *)(sp+1), (unsigned)*(uint32_t *)(sp+2));
  		break;
  	case SYS_REMOVE: //5
  		check_address(sp+1);
  		check_address((void *)*(sp+1));
  		f->eax = remove((char *)*(uint32_t *)(sp + 1));
  		break;
  	case SYS_OPEN: //6
  		check_address(sp+1);
  		check_address((void *)*(sp+1));
  		f->eax = open((const char *)*(uint32_t *)(sp+1));
  		break;
  	case SYS_FILESIZE: //7
  		check_address(sp+1);
  		f->eax = filesize((int)*(uint32_t *)(sp+1));
  		break;
  	case SYS_READ: //8
  		check_address(sp+3);
  		check_address((void *)*(sp+2));
  		f->eax = read((int)*(uint32_t *)(sp+1), (void *)*(uint32_t *)(sp+2), (unsigned)*(uint32_t *)(sp+3));
  		break;
  	case SYS_WRITE: //9
  		check_address(sp+3);
  		check_address((void *)*(sp+2));
  		f->eax = write((int)*(uint32_t *)(sp + 1), (void *)*(uint32_t *)(sp + 2), (unsigned)*(uint32_t *)(sp + 3));
  		break;
  	case SYS_SEEK: //10
  		check_address(sp+2);
  		seek((int)*(uint32_t *)(sp+1), (unsigned)*(uint32_t *)(sp+2));
  		break;
  	case SYS_TELL: //11
  		check_address(sp+1);
  		f->eax = tell((int)*(uint32_t *)(sp+1));
  		break;
  	case SYS_CLOSE: //12
  		check_address(sp+1);
  		close((int)*(uint32_t *)(sp+1));
  		break;
  	default:
  		printf("Default %d\n", *sp);
  }

  //get_argument(sp, arg, );
}


void check_address(void *address){ // 포인터가 가리키는 주소가 유저영역의 주소인지 확인, 잘못된 접근 -> 프로세스 종료
	if(!is_user_vaddr(address)){
		exit(-1);
	}
}
//void get_argument(void *esp, int *arg, int count){ // 여기 안에서 checo_address도 해줘야 // 스택에서 4byte씩 count 갯수만큼 꺼내 arg에 저장
//}
////////////////////////////////////////////////////////////////
void halt(void){
	shutdown_power_off();
}
void exit(int status){
	printf("%s: exit(%d)\n", thread_name(), status);
	thread_current()->exit_status = status;
	int i;
	for (i = 3; i < 128; i++){
		close(i); 
	}
	thread_exit();
}
pid_t exec(const char *cmd_line){
	return process_execute(cmd_line);
}
int wait(pid_t pid){
	return process_wait(pid);
}
bool create(const char *file, unsigned initial_size){
	bool tf;
	if (file == NULL){
		exit(-1);
	}
	//lock_acquire(&filesys_lock);
	tf = filesys_create(file, initial_size);
	//lock_release(&filesys_lock);

	return tf;
}
bool remove(const char *file){
	bool re;
	//lock_acquire(&filesys_lock);
	re = filesys_remove(file);
	//lock_release(&filesys_lock);
	return re;
}
int open(const char *file){
	int i;
	struct file *f;
	int re;
	if(file == NULL){
		//printf("shitman");
		return -1;
	}
	lock_acquire(&filesys_lock);
	f = filesys_open(file);
	lock_release(&filesys_lock);

	if (f == NULL){
		//exit(-1);
		//return 0;
		re = -1;
	}
	else{
		re = -1;
		for (i = 3; i < 128; i++){
			if(thread_current()->fdt[i] == NULL){
				if(strcmp(thread_current()->name, file) == 0){
					file_deny_write(f);
				}
				thread_current()->fdt[i] = f;
				//printf("here: %d\n", i);
				re = i;
				break;
			}
		}
		//re = -1;		
	}
	//lock_release(&filesys_lock);
	//printf("here2: %d\n", re);
	return re;
}                                                                                    /////왜 3이 아니라 49가 filesize의 input에 들어가는거지?  => eax에 안넣어줘서 ㅜㅜ
int filesize(int fd){
	int re;
	//printf("sibla: %d\n", fd);
	lock_acquire(&filesys_lock);
	re = file_length(thread_current()->fdt[fd]);
	lock_release(&filesys_lock);
	return re;
}
int read(int fd, void *buffer, unsigned size){
	int i;
	if (fd == 0){
		for (i = 0; i < size; i++){
			*(uint8_t *)(buffer+i) = input_getc();
			if(*(char *)(buffer+i) == '\0'){
				break;
			}
		}
	}
	else{
		if(thread_current()->fdt[fd] == NULL){
			return -1;
		}
		lock_acquire(&filesys_lock);
		i = file_read(thread_current()->fdt[fd], buffer, size);
		lock_release(&filesys_lock);
	}
	return i;
}
int write(int fd, const void *buffer, unsigned size){
	if (fd == 1){
		putbuf(buffer, size);
		return size;
	}
	else if (fd > 2){
		if(thread_current()->fdt[fd] == NULL){
			return -1;
		}
		int i ;

		lock_acquire(&filesys_lock);
		i = file_write(thread_current()->fdt[fd], buffer, size);
		lock_release(&filesys_lock);
		return i;
	}
}
void seek(int fd, unsigned position){
	//lock_acquire(&filesys_lock);
	file_seek(thread_current()->fdt[fd], position);
	//lock_release(&filesys_lock);
}
unsigned tell(int fd){
	unsigned re;
	//lock_acquire(&filesys_lock);
	re = file_tell(thread_current()->fdt[fd]);
	//lock_release(&filesys_lock);
	return re;
}

void close(int fd){
	struct file *f;
	if(thread_current()->fdt[fd] != NULL){
		f = thread_current()->fdt[fd];
		thread_current()->fdt[fd] = NULL;
		//lock_acquire(&filesys_lock);
		file_close(f);
		//lock_acquire(&filesys_lock);
	}
}

/*
void close(int fd){
	struct file *f;
	if(thread_current()->fdt[fd] == NULL){
		exit(-1);
	}
	f = thread_current()->fdt[fd];
	thread_current()->fdt[fd] = NULL;
	//lock_acquire(&filesys_lock);
	file_close(f);
	//lock_acquire(&filesys_lock);
}*/