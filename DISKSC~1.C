#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include<float.h>
#define SECTORSPERTRACK 200
#define TRACKSPERCYLINDER 8
#define CYLINDERS 500000.0
#define RPM 10000.0
#define PHYSICALSECTORSIZE 512
#define LOGICALBLOCKSIZE 4096
#define TRACKTOTRACKSEEK 2.0
#define FULLSEEK 16.0
#define TRANSFERRATE (1024*1024*1024/8)
FILE* fin,*fout;
double rotational_offset, difference,rotationtime,transfertime,limit,
cursector_offset,temp,wait,service,time,candidate_index,x;
	int* processed;
	int curcylinder,no_processed,ptr,seek;
struct request
{
    double arrival;
    int lbn;
    int request_size;
};
struct allrequest
{
    double arrival;
    int lbn;
    int request_size;
    int psn;
    int cylinder;
    int surface;
    int offset;
};
double calculateSeekTime(int prevCylinder,int cylinder)
{
    if(prevCylinder==cylinder) return 0;
    else return 0.000028*abs(prevCylinder-cylinder)+2;
}
double calculateServiceTime(int prevcylinder,double prevoffset,int cylinder,int offset,int request_size)
{
    double seektime=0;
    if(prevcylinder==cylinder) seektime=0;
    else
    seektime=0.000028*(abs(prevcylinder-cylinder))/1000+(2.0/1000);
    rotational_offset=(RPM/60*SECTORSPERTRACK*seektime);
    prevoffset=prevoffset+rotational_offset;
    while(prevoffset>SECTORSPERTRACK)
    {
	prevoffset=prevoffset-SECTORSPERTRACK;
    }
     difference=offset-prevoffset;
    if(difference<0.0f){difference=difference+SECTORSPERTRACK;}
    rotationtime=60*(difference)/SECTORSPERTRACK/RPM;
    transfertime=request_size*1.0*PHYSICALSECTORSIZE/TRANSFERRATE;
    return (seektime+rotationtime+transfertime);
}
int main(int argc,char* argv[])
{
    fin=NULL;
    fout=NULL;
    if(argc<4 || argc>5)
    {
	fprintf(stdout,"Usage: ./disksched <inputfile> <outputfile> <algorithm> [limit]\n");
	fflush(stdout);
	return -1;
    }
    fin=fopen(argv[1],"r");
    if(!fin)
    {
	fprintf(stdout,"Could not open input file %s\n",argv[1]);
	fflush(stdout);
	return -1;
    }
    fout=fopen(argv[2],"w");
    if(!fout)
    {
	fprintf(stdout,"Could not open output file %s\n",argv[2]);
	fflush(stdout);
	fclose(fin);
	return -1;
    }
    limit=INT_MAX;
    if(argc==5)
    limit=atoi(argv[4]);
    if(strcmp(argv[3],"FCFS")==0 || strcmp(argv[3],"fcfs")==0)
    {
	struct request cur_line;
	double time=0;
	int curcylinder=0;
	double prev_arrival=0;
	double cursector_offset=200;
	int psn=0;
	int cylinder=0;
	int surface=0;
	int sector_offset=0;
	int temp=0;
	double wait=0;
	double service=0;
	int count=0;
	while(!feof(fin) && count<limit)
	{
	    count++;
	    fscanf(fin,"%lf %d %d",&cur_line.arrival,&cur_line.lbn,&cur_line.request_size);
	    if(prev_arrival==cur_line.arrival) continue;
	    else prev_arrival=cur_line.arrival;
	    psn=cur_line.lbn*(LOGICALBLOCKSIZE/PHYSICALSECTORSIZE)+cur_line.request_size;
	    cylinder=psn/SECTORSPERTRACK/TRACKSPERCYLINDER;
	    temp=psn-cylinder*SECTORSPERTRACK*TRACKSPERCYLINDER;
	    surface=temp/SECTORSPERTRACK;
	    sector_offset=temp%SECTORSPERTRACK;
	    service=calculateServiceTime(curcylinder,cursector_offset,cylinder,(SECTORSPERTRACK+sector_offset-cur_line.request_size)%SECTORSPERTRACK,cur_line.request_size);
	    curcylinder=cylinder;
	    cursector_offset=sector_offset;
	    if(time<cur_line.arrival) time=cur_line.arrival;
	    wait=time-cur_line.arrival;
	    time+=service;
	    fprintf(fout,"%lf %lf %lf %d %d %d %.6f\n",cur_line.arrival,(cur_line.arrival+wait+service),wait,psn,cylinder,surface,(float)sector_offset);
	}
    }
    else if(strcmp(argv[3],"SSTF")==0 || strcmp(argv[3],"sstf")==0)
    {
	int count=0;
	double prev_arrival=0;
	struct request cur_line;
	if(argc!=5)
	{
	    while(!feof(fin))//Count number of requests
	    {
		fscanf(fin,"%lf %d %d",&cur_line.arrival,&cur_line.lbn,&cur_line.request_size);
		if(prev_arrival==cur_line.arrival) continue;
		else prev_arrival=cur_line.arrival;
		count++;
	    }
	}
	fclose(fin);
	fin=fopen(argv[1],"r");
	if(!fin)
	{
	    fprintf(stdout,"Could not open input file %s\n",argv[1]);
	    fflush(stdout);
	    return -1;
	}
	prev_arrival=0;
	limit=(count>limit || limit==INT_MAX)?count:limit;
	count=0;

	struct allrequest* requests=malloc(sizeof(struct allrequest)*(limit));
	processed=malloc(sizeof(int)*(limit));
	while(count<limit)//Read in all requests at once
	{
	    fscanf(fin,"%lf %d %d",&requests[count].arrival,&requests[count].lbn,&requests[count].request_size);
	    if(prev_arrival==requests[count].arrival) {count++;continue;}
	    else prev_arrival=requests[count].arrival;
	    processed[count]=0;
	    requests[count].psn=requests[count].lbn*(LOGICALBLOCKSIZE/PHYSICALSECTORSIZE)+requests[count].request_size;
	    requests[count].cylinder=requests[count].psn/SECTORSPERTRACK/TRACKSPERCYLINDER;
	    temp=requests[count].psn-requests[count].cylinder*SECTORSPERTRACK*TRACKSPERCYLINDER;
	    requests[count].surface=temp/SECTORSPERTRACK;
	    x=temp%SECTORSPERTRACK;
	    requests[count].offset=x;
	    count++;
	}
	//Now process requests[] using SSTF
	curcylinder=0;
	cursector_offset=200;
	temp=0;
	wait=0;
	service=0;
	no_processed=0;
	ptr=-1;
	time=0;
	candidate_index=0;
	seek=DBL_MAX;
        while(no_processed<limit)//While all requests are not serviced
        {
            //First select a candidate request to be processed
	    if(time<requests[ptr+1].arrival)
                time=requests[ptr+1].arrival;
            if(no_processed==0)
            {//Process the first request there and there
                candidate_index=0;
            }
            else
            {
                int i;
                candidate_index=ptr;
                seek=DBL_MAX;
                for(i=ptr+1;requests[i].arrival<=time && i<limit;i++)
                {
                    if(processed[i]==1) continue;
                    temp=calculateSeekTime(curcylinder,requests[i].cylinder);
                    if(temp<seek)
                    {
                        seek=temp;
                        candidate_index=i;
                    }
                }
            }
            service=calculateServiceTime(curcylinder,cursector_offset,requests[candidate_index].cylinder,(SECTORSPERTRACK+requests[candidate_index].offset-requests[candidate_index].request_size)%SECTORSPERTRACK,requests[candidate_index].request_size);
            curcylinder=requests[candidate_index].cylinder;
            cursector_offset=requests[candidate_index].offset;
            wait=time-requests[candidate_index].arrival;
            time+=service;
            fprintf(fout,"%.6f %.6f %.6f %d %d %d %.6f\n",requests[candidate_index].arrival,(requests[candidate_index].arrival+wait+service),wait,requests[candidate_index].psn,requests[candidate_index].cylinder,requests[candidate_index].surface,(float)requests[candidate_index].offset);
            no_processed++;
            processed[candidate_index]=1;
            if(ptr+1==candidate_index)
            {
                while(processed[ptr+1]==1)
                {
                    ptr+=1;
                }
            }
        }
        free(requests);
        free(processed);
    }
    else
    {
        fprintf(stdout,"Algorithm must be FCFS or SSTF\n");
        fflush(stdout);
        fclose(fin);
        fclose(fout);
        return -1;
    }
    fclose(fin);
    fclose(fout);
    return 0;
}