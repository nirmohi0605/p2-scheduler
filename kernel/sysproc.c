#include "types.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "sysfunc.h"
#include "spinlock.h"
#include "pstat.h"


struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;

int
sys_fork(void)
{
  return fork();
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
  return proc->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = proc->sz;
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
    if(proc->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since boot.
int
sys_uptime(void)
{
  uint xticks;
  
  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}




//P1 a - system call that returns the number of processes in the system

int
sys_getprocs(void){

  struct proc *p;
  int countProcs = 0;

  acquire(&ptable.lock);
  
  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
  {
    if(p->state != UNUSED){
      countProcs++;
    }
  }
  release(&ptable.lock);
  return countProcs;

}

int
sys_setpri(void){
    int pri = NULL;
    argint(0, &pri); //get argument from user side
    
    if(!pri || (pri > 2) || (pri < 1)){
        return -1; //incorrect priority
    }
    
    proc->priority = pri;
    
    return 0;
}

int
sys_getpinfo(void){
  struct pstat *ps;

  if(argptr(0, (void*)&ps, sizeof(*ps)) < 0){
    return -1;
  } else{
    if(ps == NULL){
      return -1;
    } else {
      struct proc *p;

      acquire(&ptable.lock);
      int i = 0;
      for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
        if(p->state == UNUSED){
          ps->inuse[i] = 0;
        } else{
          ps->inuse[i] = 1;
        }
        ps->pid[i] = p->pid;
        ps->hticks[i] = p->hticks;
        ps->lticks[i] = p->lticks;
        i++;
      }
      release(&ptable.lock);
      return 0;
    }
  }
  return 0;
}