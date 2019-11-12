#include <rtdk.h>
#include <native/queue.h>
#include <native/heap.h>
#include <commu.h>
#include "motion.h"

const joinpos_t  home = {0, 0 , 0, 0, 0, 0};
const incpos_t  dabao = {-59471, 3373567, 3254210, 3424, 107103, 121782};
const double maxangle[6] =
{
	MAXANG1,
	MAXANG2,
	MAXANG3,
	MAXANG4,
	MAXANG5,
	MAXANG6
};

const double minangle[6] =
{
	MINANG1,
	MINANG2,
	MINANG3,
	MINANG4,
	MINANG5,
	MINANG6
};

const double maxvel[6] = 
{
	MAXVEL1,
	MAXVEL2,
	MAXVEL3,
	MAXVEL4,
	MAXVEL5,
	MAXVEL6
};

const double maxacc[6] = 
{
	MAXACC1,
	MAXACC2,
	MAXACC3,
	MAXACC4,
	MAXACC5,
	MAXACC6
};



int ptp(const joinpos_t & p0, const joinpos_t & pf, const char * queue_name, int a, int v, int f)
{
	rt_printf("caculating the ptp function ......\n");
	for(int i=0; i<6; i++)
	{
		if( (pf.joi[i]<minangle[i]) || (pf.joi[i]>maxangle[i]) )
		{
			rt_printf("err: the destination of joint %d is out of range\n",i);
			return -1;
		}
	}


	int ivmax = 0, iamax = 0;
	double gapmaxv = 0, gapmaxa = 0;
	for(int i =0; i<6; i++)
	{
		if(abs(pf.joi[i]-p0.joi[i])/maxvel[i] > gapmaxv)
		{
			gapmaxv = abs(pf.joi[i]-p0.joi[i])/maxvel[i];
			ivmax = i;	//find which axis moves faster ralatively
		}
        if(abs(pf.joi[i]-p0.joi[i])/maxacc[i] > gapmaxa)
		{
			gapmaxa = abs(pf.joi[i]-p0.joi[i])/maxacc[i];
			iamax = i;	//find which axis accerlerate faster ralatively
		}
	}
	vectord namda = julipt(p0.joi[ivmax], 
                           pf.joi[ivmax],
    maxacc[iamax]*abs((pf.joi[ivmax]-p0.joi[ivmax])/(pf.joi[iamax]-p0.joi[iamax]))*a/100.0, 
                           maxvel[ivmax]*v/100.0,
                           f);
	RT_QUEUE tarpos;					   
	int err = rt_queue_bind(&tarpos, queue_name, TM_INFINITE);
	if(err < 0){
		rt_fprintf(stderr, "target position queue bind failed in motion.cpp:ptp() : %d", err);
	}
    for(unsigned int i=0; i<namda.size(); i++)
    {	
		joinpos_t jp;
        for(int j=0; j<6; j++)
        {	
            jp.joi[j] = p0.joi[j]+(pf.joi[j]-p0.joi[j])*namda[i];
        }
        // vjoin.push_back(jp);
		incpos_t ip = jointangle2increment(jp);
		err = rt_queue_write(&tarpos, &ip, sizeof(incpos_t), Q_NORMAL);
		if(err < 0){
			rt_fprintf(stderr, "target position queue write failed in motion.cpp:ptp() : %d", err);
		}	
    }
	err = rt_queue_unbind(&tarpos);
	if(err < 0){
		rt_fprintf(stderr, "target position queue unbind failed in motion.cpp:ptp() : %d", err);
	}
    return 0;
}
/* 
int ptp(const joinpos_t & p0, const joinpos_t & pf, vectori & vinc, int a, int v, int f )
{
	cout << "caculating the ptp function ......\n";
	for(int i=0; i<6; i++)
	{
		if( (pf.joi[i]<minangle[i]) || (pf.joi[i]>maxangle[i]) )
		{
			cout << "err: the destination of joint " << i << " is out of range\n";
			return -1;
		}
	}
    
	int ivmax = 0, iamax = 0;
	double gapmaxv = 0, gapmaxa = 0;
	for(int i =0; i<6; i++)
	{
		if(abs(pf.joi[i]-p0.joi[i])/maxvel[i] > gapmaxv)
		{
			gapmaxv = abs(pf.joi[i]-p0.joi[i])/maxvel[i];
			ivmax = i;	//find which axis moves faster ralatively
		}
        if(abs(pf.joi[i]-p0.joi[i])/maxacc[i] > gapmaxa)
		{
			gapmaxa = abs(pf.joi[i]-p0.joi[i])/maxacc[i];
			iamax = i;	//find which axis accerlerate faster ralatively
		}
	}
	vectord namda = julipt(p0.joi[ivmax], 
                           pf.joi[ivmax],
    maxacc[iamax]*abs((pf.joi[ivmax]-p0.joi[ivmax])/(pf.joi[iamax]-p0.joi[iamax]))*a/100.0, 
                           maxvel[ivmax]*v/100.0,
                           f);

    for(unsigned int i=0; i<namda.size(); i++)
    {	
		joinpos_t jp;
        for(int j=0; j<6; j++)
        {	
            jp.joi[j] = p0.joi[j]+(pf.joi[j]-p0.joi[j])*namda[i];
        }
        vinc.push_back(jointangle2increment(jp));
    }
    return 0;
}


int ptp(const incpos_t & inc0, const incpos_t & incf, vectori & vinc, int a, int v, int f)
{
    cout << "caculating the ptp function ......\n";
    joinpos_t p0 = increment2jointangle(inc0);
    joinpos_t pf = increment2jointangle(incf);
	for(int i=0; i<6; i++)
	{
		if( (pf.joi[i]<minangle[i]) || (pf.joi[i]>maxangle[i]) )
		{
			cout << "err: the destination of joint " << i << " is out of range\n";
			return -1;
		}
	}
	int ivmax = 0, iamax = 0;
	double gapmaxv = 0, gapmaxa = 0;
	for(int i =0; i<6; i++)
	{
		if(abs(pf.joi[i]-p0.joi[i])/maxvel[i] > gapmaxv)
		{
			gapmaxv = abs(pf.joi[i]-p0.joi[i])/maxvel[i];
			ivmax = i;	//find which axis moves faster ralatively
		}
        if(abs(pf.joi[i]-p0.joi[i])/maxacc[i] > gapmaxa)
		{
			gapmaxa = abs(pf.joi[i]-p0.joi[i])/maxacc[i];
			iamax = i;	//find which axis accerlerate faster ralatively
		}
	}
	vectord namda = julipt( p0.joi[ivmax], 
                            pf.joi[ivmax], 
    maxacc[iamax]*abs((pf.joi[ivmax]-p0.joi[ivmax])/(pf.joi[iamax]-p0.joi[iamax]))*a/100.0, 
                            maxvel[ivmax]*v/100.0,
                            f);

    for(unsigned int i=0; i<namda.size(); i++)
    {	
		joinpos_t jp;
        for(int j=0; j<6; j++)
        {	
            jp.joi[j] = p0.joi[j]+(pf.joi[j]-p0.joi[j])*namda[i];
        }
        vinc.push_back(jointangle2increment(jp));
    }
    return 0;
}

int ptp( const joinpos_t & p0, const cartpos_t & cpf, vectori & vinc, int a, int v, int f )
{
    cout << "caculating the ptp function ......\n";
    joinpos_t pf;
    if(-1 == invkine(cpf, p0.joi, pf.joi))
	{
		cout << "inverse kinematics err\n";
		return -1;
	}

	for(int i=0; i<6; i++)
	{
		if( (pf.joi[i]<minangle[i]) || (pf.joi[i]>maxangle[i]) )
		{
			cout << "err: the destination of joint " << i << " is out of range\n";
			return -1;
		}
	}
	int ivmax = 0, iamax = 0;
	double gapmaxv = 0, gapmaxa = 0;
	for(int i =0; i<6; i++)
	{
		if(abs(pf.joi[i]-p0.joi[i])/maxvel[i] > gapmaxv)
		{
			gapmaxv = abs(pf.joi[i]-p0.joi[i])/maxvel[i];
			ivmax = i;	//find which axis moves faster ralatively
		}
        if(abs(pf.joi[i]-p0.joi[i])/maxacc[i] > gapmaxa)
		{
			gapmaxa = abs(pf.joi[i]-p0.joi[i])/maxacc[i];
			iamax = i;	//find which axis accerlerate faster ralatively
		}
	}
	vectord namda = julipt( p0.joi[ivmax], 
                            pf.joi[ivmax], 
    maxacc[iamax]*abs((pf.joi[ivmax]-p0.joi[ivmax])/(pf.joi[iamax]-p0.joi[iamax]))*a/100.0, 
                            maxvel[ivmax]*v/100.0,
                            f );

    for(unsigned int i=0; i<namda.size(); i++)
    {	
		joinpos_t jp;
        for(int j=0; j<6; j++)
        {	
            jp.joi[j] = p0.joi[j]+(pf.joi[j]-p0.joi[j])*namda[i];
        }
        vinc.push_back(jointangle2increment(jp));
    }
    return 0;
}

void PTP(void *cookie)
{

}
 */

void PTP(void *cookie)
{
	RT_HEAP heap_desc;
	static void * data_sharm;
	driverdata_t * data;
	rt_print_auto_init(1);
	rt_printf("Satrt PTP......\n");
	int err = rt_heap_bind(&heap_desc, DRIVE_DATA_HEAP_NAME, TM_INFINITE);
	if(err < 0){
		rt_printf("data heap bind fail in motion.cpp:PTP() : %d\n", err);
	}
	err = rt_heap_alloc(&heap_desc, 0, TM_INFINITE, &data_sharm);
    if(err < 0){
		rt_fprintf(stderr, "data heap alloc faile in motion.cpp:PTP() : %d", err);
    }
	data = (driverdata_t *) data_sharm;
	incpos_t ip0;
	for(int i=0; i<6; i++){
		ip0.inc[i] = data->ActualPosition[i];
	}
	err = rt_heap_free(&heap_desc, data_sharm);
	if(err < 0){
		rt_fprintf(stderr, "data heap free fail in motion.cpp:PTP() : %d", err);
	}
	if(ptp(increment2jointangle(ip0), home, TARPOS_QUEUE_NAME, 10, 20) < 0){
		rt_printf("PTP function failed\n");
	}
}