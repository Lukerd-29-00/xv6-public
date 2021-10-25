#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
int
sys_fork(void)
{
  return fork();
}

int
strlen(const char *s);

void
sys_crsp(int *buf)
{
 extern struct {
 struct spinlock lock;
 struct proc proc[NPROC];
 } ptable;
 //There needs to be room for a 16-character name, since that is the longest name allowed in a proc struct.
 cprintf("name              pid               state\n-------------------------------------------\n");
 acquire(&ptable.lock);
 for(struct proc *p = &ptable.proc[0];p < &ptable.proc[NPROC];p++){
	if(p->state == RUNNING || p->state == SLEEPING){
 		cprintf("%s",p->name);
		//Keep the second column alligned with the pid label
		for(int i = 0;i < 19-strlen(p->name); i++){
			cprintf(" ");
		}
		cprintf("%d",p->pid);
		//Keep the third column aligned with the state label
		for(int i = 0;i < 15-p->pid/10;i++){
			cprintf(" ");
		}
	}
	if(p->state == RUNNING){
		cprintf("%s\n","RUNNING");
	}
	else if(p->state == SLEEPING){
		cprintf("%s\n","SLEEPING");
	}
 }
 release(&ptable.lock);
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
