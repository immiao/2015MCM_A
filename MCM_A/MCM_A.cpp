#include <iostream>
#include <cstdio>
#include <vector>
#include <ctime>
#include <algorithm>
#include <cmath>

#define depot_num 3
#define N 43
#define MAXN 100000000
#define L 80 //distance limit for each vehicle
#define EARTH_RADIUS 6378.137
using namespace std;

double dis[N+1][N+1];
double c[N+1];//ÿ�������������ҩ����������ҩ�������ҩ����ӵ�е�ҩ������
long long S[N+1],I[N+1],R[N+1];//ÿ����������SIR

struct Weight
{
	int w;
	int nodeNum;
}W[N+1];//�ڵ��Ȩ�� Խ��Խ��

double lambda[N+1],mu[N+1];
long long maxI = 1000000000;

double total_fitness;
const int worst_dis=1000000;
int index;

double rad(double d)
{
	return d*M_PI/180.0;
}

double GetDistance(double lon1,double lat1,double lon2,double lat2)
{
      double radLat1 = rad(lat1);
      double radLat2 = rad(lat2);
      double a = radLat1 - radLat2;
      double b = rad(lon1) - rad(lon2);
      double s = 2 * asin(sqrt(pow(sin(a/2),2)+cos(radLat1)*cos(radLat2)*pow(sin(b/2),2)));
      s = s * EARTH_RADIUS;
      //s = Math.round(s * 10000) / 10000;
      return s;
}

bool cmpForBan(Weight a, Weight b)
{
	return a.w<b.w;
}

class chromosome
{
public:
	int path[N+1];
	int pre[N+1];
	int lastPre[N+1];//��¼ÿ��ҩ�������һ������
	int lastcounter;
	bool banList[N+1];
	int banNumber;//�ͷ�����
	double fitness;
	chromosome()
	{
		lastcounter=0;
		memset(banList,false,sizeof(banList));
		memset(lastPre,-1,sizeof(lastPre));
		banNumber=0;
	}
	void assign(int arr[])
	{
		for (int i=0;i<=N;i++)
			path[i]=arr[i];
	}
	double split(int s,int e)
	{
		//to do the ban
		//memset(banList,false,sizeof(banList));

		int counter=0;

		for (int i=s+1;i<=e;i++)
		{
			double threshold=mu[i]/lambda[i];
			W[counter].w=dis[path[s]][path[i]]+(maxI-I[path[i]]);//;
			W[counter].nodeNum=path[i];
			counter++;
		}
		sort(W,W+counter,cmpForBan);
		int total=0;
		int wIndex=0;
		while (total<=c[path[s]] && wIndex<counter)
		{
			total+=c[W[wIndex].nodeNum];
			wIndex++;
		}
		if (total>c[path[s]])
			for (int i=wIndex-1;i<counter;i++)
				banList[W[i].nodeNum]=true;

		int banPunish=0;
		int leftPunish=0;
		for (int i=s+1;i<=e;i++)
		{
			if (banList[path[i]]) banNumber++;
			else leftPunish+=c[path[i]];
		}
		leftPunish=c[path[s]]-leftPunish;

		double v[N+1];
		//for delete middle path
		int mostLeft[N+1];
		int maxJ=0;

		//mostLeft initial
		mostLeft[s]=s;
		for (int i=s+1;i<=e;i++)
			if (banList[path[i]])
				mostLeft[i]=mostLeft[i-1];
			else
				mostLeft[i]=i;

		v[path[s]]=0;
		pre[path[s]]=path[s];
		for (int i=s+1;i<=e;i++) v[path[i]]=MAXN;
		for (int i=s+1;i<=e;i++)
		{
			if (banList[path[i]]) continue;
			double cost=0;
			double load=0;
			int j=i;
			int PRE=-1;
			while (j<=e && load<=L)
			{
				if (banList[path[j]])
				{
					j++;
					continue;
				}
				load+=c[path[j]];
				// +(maxI-I[j]) ��������Խ�� ����Խ�� ���ȿ���
				if (PRE==-1) cost=dis[path[s]][path[j]]+dis[path[j]][path[s]];//+(maxI-I[j])/2;
				else cost=cost-dis[path[s]][path[PRE]]+dis[path[PRE]][path[j]]+dis[path[j]][path[s]];//+(maxI-I[j])/2;
				PRE=j;
				if (load<=L)
				{
					if (j>maxJ)
						maxJ=j;
					if (v[path[mostLeft[i-1]]]+cost<v[path[j]])
					{
						v[path[j]]=v[path[mostLeft[i-1]]]+cost;
						pre[path[j]]=path[mostLeft[i-1]];
					}
					j++;
				}
			}
		}
		lastPre[lastcounter++]=maxJ;
		return v[path[maxJ]]+leftPunish;
	}
	void cal_fitness()
	{
		int pre=0;
		fitness=0;

		for (int i=1;i<=N;i++)
		{
			if (path[i]<depot_num)
			{
				fitness+=split(pre,i-1);
				pre=i;
			}
		}
		fitness+=split(pre,N);
		fitness=worst_dis-fitness;
	}
};
vector<chromosome> population[2];
vector<chromosome> temp;

void random_init(int arr[])
{
	int A[N];
	for (int i=0;i<N;i++)
		A[i]=i+1;
	int length=N;
	arr[0]=0;
	for (int i=1;i<=N;i++)
	{
		int r=rand()%length;
		arr[i]=A[r];
		int temp=A[r];
		A[r]=A[length-1];
		A[length-1]=temp;
		length--;
	}
	arr[N]=A[0];
}

const int p_size=30;
void init_population()
{
	srand((unsigned)time(NULL));
	chromosome chr;
	int p[N+1];
	//int p[N+1]={0,11, 18, 6 ,15 ,8 ,12, 1 ,7 ,10, 3, 2, 9, 16 ,17, 19, 5 ,13 ,14, 4};
    //int p[N+1]={0,17,24,35,37,34,26,11,8,2,22,3,7,16,32,10,21,30,13,28,27,36,6,14,19,25,33,12,18,4,9,38,15,5,29,20,23,1,31};
	//int p[N+1]={0,6,15, 4 ,8 ,18, 1 ,3 ,19, 7, 5 ,12 ,9 ,16 ,13 ,14 ,2 ,10, 11, 17};
	//int p[N+1]={0,3,2,1};
	//int p[N+1]={0 ,2 ,11, 15, 17, 18, 13, 10, 7 ,3 ,19, 1 ,9 ,8 ,12, 6, 5 ,14 ,4 ,16};
	index=0;
	//population size is 4
	for (int i=0;i<p_size;i++)
	{
		random_init(p);
		printf("Initial Population:");
		for (int j=0;j<=N;j++) printf("%d ",p[j]);printf("\n");
		chr.assign(p);
		population[index].push_back(chr);
	}
}

void update_fitness()
{
	total_fitness=0;
	for (int i=0;i<(int)population[index].size();i++)
	{
		memset(population[index][i].banList,false,sizeof(population[index][i].banList));
		population[index][i].cal_fitness();
		total_fitness+=population[index][i].fitness;
		/*for (int j=1;j<=N;j++)
			printf("%d ",population[index][i].path[j]);
		printf("\n");*/
	}
	//printf("\n\n");
}

bool cmp(chromosome a,chromosome b)
{
	return a.fitness<b.fitness;
}

//generate a number in range from 0 to end
int random(int end)
{
    return rand() % (end + 1);
}

void convert(int &a,int &b)
{
	if (a>b)
	{
		int temp=a;
		a=b;
		b=temp;
	}
}

void crossover(chromosome &a,chromosome &b,int index1,int index2)
{
	int first_pos=rand()%(N-1)+1; //generate a number in range from 1 to N-1
	int second_pos;
	//generate a number which is different from first number
	while ((second_pos=rand()%(N-1)+1)==first_pos);
	convert(first_pos,second_pos);
	chromosome new1,new2;
	new1.path[0]=new2.path[0]=0;
	bool hash1[N+1]={false};
	bool hash2[N+1]={false};
	for (int i=first_pos+1;i<=second_pos;i++)
	{
		new1.path[i]=a.path[i];
		hash1[a.path[i]]=true;
		new2.path[i]=b.path[i];
		hash2[b.path[i]]=true;
	}
	int hashapos=1;
	int hashbpos=1;
	for (int i=1;i<=first_pos;i++)
	{
		while (hash1[b.path[hashbpos]])
			hashbpos++;
		new1.path[i]=b.path[hashbpos++];
		while (hash2[a.path[hashapos]])
			hashapos++;
		new2.path[i]=a.path[hashapos++];
	}
	for (int i=second_pos+1;i<=N;i++)
	{
		while (hash1[b.path[hashbpos]])
			hashbpos++;
		new1.path[i]=b.path[hashbpos++];
		while (hash2[a.path[hashapos]])
			hashapos++;
		new2.path[i]=a.path[hashapos++];
	}
	new1.cal_fitness();
	new2.cal_fitness();
	if (new1.fitness>temp[0].fitness)
		temp[index1]=new1;
	if (new2.fitness>temp[1].fitness)
		temp[index2]=new2;
	//temp.push_back(new1);
	//temp.push_back(new2);
}

void copulation()
{
    //int remainder = 4;
    //int r1,r2;
    int new_index = (index + 1) % 2;
    temp=population[index];
    //crossover(temp[p_size-1],temp[0],8,9);
    crossover(temp[p_size-1],temp[p_size-8],0,1);
    crossover(temp[p_size-2],temp[p_size-7],2,3);
    crossover(temp[p_size-3],temp[p_size-6],4,5);
    crossover(temp[p_size-4],temp[p_size-5],6,7);

    /*while (remainder)
    {
        r1 = random(remainder-1);
        temp1 = population[new_index][r1];
        population[new_index].erase(population[new_index].begin() + r1);
        remainder--;
        r2 = random(remainder-1);
        temp2 = population[new_index][r2];
        population[new_index].erase(population[new_index].begin() + r2);
        remainder--;
        crossover(temp1,temp2);
    }*/
    population[new_index] = temp;
    temp.clear();
}

//2-opt
void mutation()
{
	int new_index=(index+1)%2;
	for (int i=15;i<(int)population[new_index].size();i++)
	{
		int r;
		chromosome maxn,ttt,minn;
		int first_pos,second_pos;
		maxn.fitness=0;
		minn.fitness=MAXN;
		r=random(99)+1;

		//Local Search 1 (used as mutation) : 2-opt
		bool improved_flag = true;
		while (improved_flag)
		{
			improved_flag = false;
			for (first_pos=1;first_pos<=N-1;first_pos++)
				for (second_pos=first_pos+1;second_pos<=N-1;second_pos++)
				{
					ttt=population[new_index][i];
					for (int j=first_pos;j<=(first_pos+second_pos)/2;j++)
					{
						int t=ttt.path[j];
						ttt.path[j]=ttt.path[second_pos-(j-first_pos)];
						ttt.path[second_pos-(j-first_pos)]=t;
					}
					ttt.cal_fitness();
					if (ttt.fitness>maxn.fitness)
					{
						maxn=ttt;
						improved_flag = true;
					}
					if (ttt.fitness<minn.fitness)
						minn=ttt;
				}


			//Local Search 2 : swap(u,v)
			for (first_pos=1;first_pos<=N;first_pos++)
				for (second_pos=first_pos+1;second_pos<=N;second_pos++)
				{
					ttt=population[new_index][i];
					int t=ttt.path[first_pos];
					ttt.path[first_pos]=ttt.path[second_pos];
					ttt.path[second_pos]=t;
					ttt.cal_fitness();
					if (ttt.fitness>maxn.fitness)
					{
						maxn=ttt;
						improved_flag = true;
					}
					if (ttt.fitness<minn.fitness)
						minn=ttt;
				}

			//Local Search 3 : insert(u,v) after x
			for (first_pos=1;first_pos<=N;first_pos++)
				for (second_pos=first_pos+1;second_pos<=N;second_pos++)
				{
					int temp_arr[N+1];
					int arr_counter=0;
					for (int j=0;j<first_pos;j++)
						temp_arr[arr_counter++]=population[new_index][i].path[j];
					for (int j=second_pos+1;j<=N;j++)
						temp_arr[arr_counter++]=population[new_index][i].path[j];
					int insert_pos;
					insert_pos=rand()%arr_counter; // 0 to length (length+1==arr_counter)
					for (int j=0;j<=insert_pos;j++)
						ttt.path[j]=temp_arr[j];
					int ccc=0;
					while (ccc<=second_pos-first_pos)
					{
						ttt.path[insert_pos+1+ccc]=population[new_index][i].path[first_pos+ccc];
						ccc++;
					}
					for (int j=insert_pos+1;j<arr_counter;j++)
					{
						ttt.path[insert_pos+1+ccc]=temp_arr[j];
						ccc++;
					}
					ttt.cal_fitness();
					if (ttt.fitness>maxn.fitness)
					{
						maxn=ttt;
						improved_flag = true;
					}
					if (ttt.fitness<minn.fitness)
						minn=ttt;
				}
		}

		//Local Search 4 : insert u after x
		/*for (first_pos=1;first_pos<=N;first_pos++)
		{
			ttt=population[new_index][i];
			for (second_pos=first_pos-1;second_pos>=1;second_pos--)
			{
				int t=ttt.path[second_pos+1];
				ttt.path[second_pos+1]=ttt.path[second_pos];
				ttt.path[second_pos]=t;
				ttt.cal_fitness();
				if (ttt.fitness>maxn.fitness)
					maxn=ttt;
				if (ttt.fitness<minn.fitness)
					minn=ttt;
			}

			ttt=population[new_index][i];
			for (second_pos=first_pos+1;second_pos<=N;second_pos++)
			{
				int t=ttt.path[second_pos-1];
				ttt.path[second_pos-1]=ttt.path[second_pos];
				ttt.path[second_pos]=t;
				ttt.cal_fitness();
				if (ttt.fitness>maxn.fitness)
					maxn=ttt;
				if (ttt.fitness<minn.fitness)
					minn=ttt;
			}
		}*/
		if (maxn.fitness>population[new_index][i].fitness)
			population[new_index][i]=maxn;
		//mutation : accept bad solution at rate 2%
		else if (r<=20)
			population[new_index][i]=minn;
	}
}

double x[N+1],y[N+1];

double GetDistance1(double x1,double y1,double x2,double y2)
{
      return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}

void input()
{
	FILE *fp=fopen("actual_pos.txt","r");
	FILE *fpin=fopen("actual_cap.txt","r");
	for (int i=0;i<=N;i++)
		fscanf(fp,"%lf%lf",&x[i],&y[i]);
	for (int i=0;i<=N;i++)
		for (int j=0;j<=N;j++)
			if (i==j)
				dis[i][j]=0;
			else
				dis[i][j]=GetDistance1(x[i],y[i],x[j],y[j]);
	for (int i=0;i<=N;i++)
		fscanf(fpin,"%lf",&c[i]);
	fclose(fp);
	fclose(fpin);

	FILE *fpSIR=fopen("actual_sir.txt","r");
	for (int i=0;i<=N;i++)
		fscanf(fpSIR,"%I64d%I64d%I64d",&S[i],&I[i],&R[i]);
	fclose(fpSIR);

	for (int i=depot_num;i<=N;i++)
	{
		lambda[i]=0.0000004;//0.0000004;
		mu[i]=0.3;
	}
}

int findpos(int city,chromosome &best_chr)
{
	int pos;
	for (pos=0;pos<=N;pos++)
		if (best_chr.path[pos]==city)
			break;
	return pos;
}

int main()
{
	char outSIRname[100];
	strcpy(outSIRname,"SIRout1.txt");
	input();
	for (int day=1;day<=30;day++)
	{
		clock_t start_time=clock();
		int counter=1;
		double max_fitness=0;
		chromosome best_chr;
		init_population();
		FILE *fit=fopen("best_dis.txt","w");
		while (counter<=20)
		{
			update_fitness();
			sort(population[index].begin(),population[index].end(),cmp);
			//debug
			/*for (int i=0;i<(int)population[index].size();i++)
			{
				for (int j=0;j<=N;j++)
					printf("%d ",population[index][i].path[j]);
				printf("fitness:%lf\n",population[index][i].fitness);
			}
			printf("--------------------------------\n");*/
			if (population[index][population[index].size()-1].fitness>max_fitness)
			{
				max_fitness=population[index][population[index].size()-1].fitness;
				best_chr=population[index][population[index].size()-1];
			}
			fprintf(fit,"%lf\n",worst_dis-max_fitness);
			copulation();
			mutation();
			index=(index+1)%2;
			counter++;
		}
		for (int i=0;i<=N;i++)
			printf("%d ",best_chr.path[i]);
		printf("\n");
		printf("The best dis is : %f\n",worst_dis-best_chr.fitness);

		//newest
		int lastIndex=best_chr.lastcounter-1;
		int city=best_chr.path[best_chr.lastPre[lastIndex]];

		int depotcounter=0;
		char filename[100],pathname[100];
		strcpy(filename,"matlab_data11.txt");
		strcpy(pathname,"matlab_path11.txt");
		filename[12]=day+'0';
		pathname[12]=day+'0';
		cout << filename << endl;
		cout << pathname << endl;
		while (1)
		{
			depotcounter++;
			filename[11]=depotcounter+'0';
			pathname[11]=depotcounter+'0';
			FILE *fout=fopen(filename,"w");
			FILE *fstr=fopen(pathname,"w");

			int ans[N];//�����ÿһ������������һ���ͻ���
			int ansc=0;
			while (1)//here
			{
				ans[ansc++]=city;
				if (city==best_chr.pre[city])
					break;
				city=best_chr.pre[city];
			}
			printf("---The depot %d---\n",city);
			ansc--;
			int pos=findpos(city,best_chr);
			int record_pos=pos;//depot position
			pos++;
			int vehicle_counter=0;
			for (int i=ansc-1;i>=0;i--)
			{
				int v_load=0;
				printf("The route of vehicle %d : %d -> ",++vehicle_counter,best_chr.path[record_pos]);
				fprintf(fout,"%f %f\n",x[best_chr.path[record_pos]],y[best_chr.path[record_pos]]);
				fprintf(fstr,"%d\n",best_chr.path[record_pos]);
				while (best_chr.path[pos]!=ans[i])
				{
					if (best_chr.banList[best_chr.path[pos]])
					{
						pos++;
						continue;
					}
					v_load+=c[best_chr.path[pos]];
					fprintf(fout,"%f %f\n",x[best_chr.path[pos]],y[best_chr.path[pos]]);
					fprintf(fstr,"%d\n",best_chr.path[pos]);
					printf("%d -> ",best_chr.path[pos]);
					pos++;
				}
				v_load+=c[best_chr.path[pos]];
				fprintf(fout,"%f %f\n",x[best_chr.path[pos]],y[best_chr.path[pos]]);
				fprintf(fstr,"%d\n",best_chr.path[pos]);
				printf("%d -> %d.\n",best_chr.path[pos++],best_chr.path[record_pos]);
				fprintf(fout,"%f %f\n",x[best_chr.path[record_pos]],y[best_chr.path[record_pos]]);
				fprintf(fstr,"%d\n",best_chr.path[record_pos]);

				printf("The load of vehicle %d : %d.\n",vehicle_counter,v_load);
			}
			if (!city) break;
			//find the position of city (pos == 0 is impossible)
			pos=findpos(city,best_chr);
			lastIndex--;
			city=best_chr.path[best_chr.lastPre[lastIndex]];
			fclose(fout);
			fclose(fstr);
		}
		fclose(fit);
		clock_t end_time=clock();
		printf("Time :%lds\n",(end_time-start_time)/CLOCKS_PER_SEC);

		outSIRname[6]=day+'0';
		FILE *foutSIR=fopen(outSIRname,"w");
		for (int i=0;i<depot_num;i++)
			fprintf(foutSIR,"0\t0\t0\n");
		long long stotal,itotal,rtotal;
		stotal=itotal=rtotal=0;
		for (int i=depot_num;i<=N;i++)
		{

			printf("---start---\n");
			printf("S:%I64d I:%I64d R:%I64d\n",S[i],I[i],R[i]);
			printf("lambda:%.10lf mu:%.10lf\n",lambda[i],mu[i]);
			if (!best_chr.banList[i])
			{
				lambda[i]-=1.0/c[i]/10000000;///10000000;
				mu[i]+=1.0/c[i];
			}
			printf("lambda:%.10lf mu:%.10lf\n",lambda[i],mu[i]);
			long long m_S=(int)(lambda[i]*S[i]*I[i])<S[i]?(int)(lambda[i]*S[i]*I[i]):S[i];
			long long m_I=(int)(mu[i]*I[i])<I[i]+m_S?(int)(mu[i]*I[i]):I[i]+m_S;
			S[i]=S[i]-m_S;
			I[i]=I[i]+m_S-m_I;
			R[i]=R[i]+m_I;
			printf("S:%I64d I:%I64d R:%I64d\n",S[i],I[i],R[i]);
			fprintf(foutSIR,"%I64d\t%I64d\t%I64d\n",S[i],I[i],R[i]);
			stotal+=S[i];itotal+=I[i];rtotal+=R[i];
		}
		fprintf(foutSIR,"total:%I64d\t%I64d\t%I64d\n",stotal,itotal,rtotal);
		fclose(foutSIR);
		//system("pause");
	}
}







