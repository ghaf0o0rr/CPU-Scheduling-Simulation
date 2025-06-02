#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_P      128
#define MAX_SLICE 2048

typedef struct {
    int pid, arrival, burst;
    int start, finish, remain, started;
} Process;

typedef struct {            /* one Round-Robin quantum slice */
    int pid, t0, t1;
} Slice;

/* utility --------------------------------------------------------------- */
static void die(const char *m){ fprintf(stderr,"%s\n",m); exit(EXIT_FAILURE); }
static void sort_arrival(Process *p,int n){
    for(int i=1;i<n;++i){ Process k=p[i]; int j=i-1;
        while(j>=0 && p[j].arrival>k.arrival){ p[j+1]=p[j]; --j; }
        p[j+1]=k;
    }
}
static void print_table(Process *p,int n){
    puts("\nSchedule (time units)");
    puts("PID Arr Burst Start Finish Turn Wait Resp");
    for(int i=0;i<n;++i){
        int turn=p[i].finish-p[i].arrival;
        int wait=turn-p[i].burst;
        int resp=p[i].start-p[i].arrival;
        printf("%3d %4d %5d %5d %6d %5d %5d %5d\n",
               p[i].pid,p[i].arrival,p[i].burst,p[i].start,
               p[i].finish,turn,wait,resp);
    }
}
static void print_avgs(Process *p,int n){
    double st=0,sw=0,sr=0;
    for(int i=0;i<n;++i){
        st+=p[i].finish-p[i].arrival;
        sw+=p[i].finish-p[i].arrival-p[i].burst;
        sr+=p[i].start-p[i].arrival;
    }
    printf("\nAverages → Turn: %.2f   Wait: %.2f   Resp: %.2f\n",
           st/n, sw/n, sr/n);
}
static void print_timeline(Slice *s,int m){
    puts("\nTimeline (RR quantum slices):");
    for(int i=0;i<m;++i){
        printf("[%d-%d] P%d",s[i].t0,s[i].t1,s[i].pid);
        if(i<m-1) printf(" | ");
    }
    putchar('\n');
}

/* FCFS – First-Come/First-Served (non-pre-emptive) ----------------------- */
static void schedule_fcfs(Process *p,int n){
    sort_arrival(p,n);
    int t=0;
    for(int i=0;i<n;++i){
        if(t<p[i].arrival) t=p[i].arrival;
        p[i].start=t;
        t+=p[i].burst;
        p[i].finish=t;
    }
}

/* SJF – Shortest-Job-First (non-pre-emptive) ----------------------------- */
static void schedule_sjf(Process *p,int n){
    int done=0,t=0,taken[MAX_P]={0};
    while(done<n){
        int best=-1,b=0;
        for(int i=0;i<n;++i)
            if(!taken[i] && p[i].arrival<=t &&
               (best==-1 || p[i].burst<b)){ best=i; b=p[i].burst; }
        if(best==-1){
            int soon=-1;
            for(int i=0;i<n;++i)
                if(!taken[i] && (soon==-1 || p[i].arrival<p[soon].arrival))
                    soon=i;
            t=p[soon].arrival; best=soon;
        }
        p[best].start=t;
        t+=p[best].burst;
        p[best].finish=t;
        taken[best]=1;
        ++done;
    }
}

/* RR – Round-Robin (pre-emptive) with timeline -------------------------- */
static void schedule_rr(Process *p,int n,int q,Slice *sl,int *m){
    int Q[MAX_P],head=0,tail=0;
    sort_arrival(p,n);
    for(int i=0;i<n;++i){ p[i].remain=p[i].burst; p[i].started=0; }
    int time=0,done=0,next=0; *m=0;
#define ENQ(IDX) do{ Q[tail%MAX_P]=(IDX); ++tail; }while(0)
    while(next<n && p[next].arrival<=time) ENQ(next++);
    while(done<n){
        if(head==tail){ time=p[next].arrival; ENQ(next++); continue; }
        int idx=Q[head%MAX_P]; ++head;
        Process *pr=&p[idx];
        if(!pr->started){ pr->start=time; pr->started=1; }
        int slice=pr->remain<q? pr->remain:q;
        int t0=time, t1=time+slice;
        pr->remain-=slice; time=t1;
        sl[*m]=(Slice){ pr->pid, t0, t1 }; ++(*m);
        while(next<n && p[next].arrival<=time) ENQ(next++);
        if(pr->remain==0){ pr->finish=time; ++done; }
        else ENQ(idx);
    }
#undef ENQ
}

/* main ------------------------------------------------------------------ */
int main(void){
    char algo[8];
    printf("Algorithm (fcfs/sjf/rr): ");
    if(scanf("%7s",algo)!=1) die("No algorithm");
    int q=4;
    if(strcmp(algo,"rr")==0){
        printf("Quantum: ");
        if(scanf("%d",&q)!=1 || q<=0) die("Bad quantum");
    }else if(strcmp(algo,"fcfs") && strcmp(algo,"sjf"))
        die("Invalid algorithm");
    int n;
    printf("Processes (N): ");
    if(scanf("%d",&n)!=1 || n<=0 || n>MAX_P) die("Bad N");
    Process p[MAX_P];
    printf("Enter pid arrival burst\n");
    for(int i=0;i<n;++i){
        printf("P%d: ",i+1);
        if(scanf("%d %d %d",&p[i].pid,&p[i].arrival,&p[i].burst)!=3)
            die("Bad input");
    }

    Slice slices[MAX_SLICE]; int m=0;
    if(strcmp(algo,"fcfs")==0)      schedule_fcfs(p,n);
    else if(strcmp(algo,"sjf")==0)  schedule_sjf(p,n);
    else                            schedule_rr(p,n,q,slices,&m);

    print_table(p,n);
    print_avgs(p,n);
    if(strcmp(algo,"rr")==0) print_timeline(slices,m);
    return 0;
}
