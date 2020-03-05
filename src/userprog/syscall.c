#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

#include <devices/shutdown.h>
#include <threads/thread.h>
#include <filesys/filesys.h>

#include "threads/vaddr.h"

static void syscall_handler (struct intr_frame *f);

void
syscall_init (void) 
{
	//printf("tlqkffhaemfdk\n");
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  //printf("syscall : %d\n", *(uint32_t *)(f->esp));
  //hex_dump(f->esp, f->esp, 100, 1); 
  // 1. 스택 포인터 -> 2. 스택포인터가 유저영역인지, check_address로 확인 -> 3. 스택에 저장된 인자값이 포인터면 check_address로 확인 (이거 다 get_argument에서 하는듯)
  int *sp = (int *)f-> esp;

  check_address(sp);

  int systemcall_number = *sp; // sp가 주소 // systemcall_number 가 숫자

  //printf("sp: %8x\n", sp);
  //printf("*sp: %d\n", *sp);
  //printf("sp+16: %8x\n", sp+16);
  //printf("*sp+16: %d\n", *(sp+16));
  //printf("sp+20: %8x\n", sp+20);
  //printf("*sp+20: %d\n", *(sp+20));
  //printf("sp+24: %8x\n", sp+24);
  //printf("*sp+24: %d\n", *(sp+24));
  //printf("sp+4: %8x\n", sp+4);
  //printf("*sp+4: %d\n", *(sp+4));
  //printf("sp+1: %8x\n", sp+1);
  //printf("*sp+1: %d\n", *(sp+1));

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
  		check_address((void *)*(sp+1));
  		check_address(sp+1);
  		f->eax = wait((pid_t)*(uint32_t *)(sp+1));
  		break;
  	case SYS_CREATE: //4
  		check_address(sp+2);
  		f->eax = create((const char *)*(uint32_t *)(sp+1), (unsigned)*(uint32_t *)(sp+2));
  		break;
  	case SYS_REMOVE: //5
  		check_address(sp+1);
  		f->eax = remove((char *)*(uint32_t *)(sp + 1));
  		break;
  	case SYS_OPEN: //6
		 	
  		break;
  	case SYS_FILESIZE: //7
  		
  		break;
  	case SYS_READ: //8
  		check_address(sp+3);
  		check_address((void *)*(sp+2));
  		f->eax = read((int)*(uint32_t *)(sp+1), (void *)*(uint32_t *)(sp+2), (unsigned)*(uint32_t *)(sp+3));
  		break;
  	case SYS_WRITE: //9
  		check_address(sp+3);
  		f->eax = write((int)*(uint32_t *)(sp + 1), (void *)*(uint32_t *)(sp + 2), (unsigned)*(uint32_t *)(sp + 3));
  		break;
  	case SYS_SEEK: //10
  	
  		break;
  	case SYS_TELL: //11
  	
  		break;
  	case SYS_CLOSE: //12

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
	thread_exit();
}
pid_t exec(const char *cmd_line){
	return process_execute(cmd_line);
}
int wait(pid_t pid){
	return process_wait(pid);
}
bool create(const char *file, unsigned initial_size){
	return filesys_create(file, initial_size);
}
bool remove(const char *file){
	return filesys_remove(file);
}
int open(const char *file){
	
}
int filesize(int fd){

}
int read(int fd, void *buffer, unsigned size){
	if (fd == 0){
		int i;
		for (i = 0; i<size; i++){
			if(((char *)buffer)[i] == '\0'){
				break;
			}
		}
		return size;
	}
	else{
		return -1;
	}
}
int write(int fd, const void *buffer, unsigned size){
	if (fd == 1){
		putbuf(buffer, size);
		return size;
	}
	else{
		return -1;
	}
}
void seek(int fd, unsigned position){

}
unsigned tell(int fd){

}

void close(int fd){

}