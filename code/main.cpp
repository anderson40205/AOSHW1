#include<iostream>
#include<time.h>
#include<queue> //For FIFO
#include<random> //uniform_int_distribution
#include<vector> //For OPT ESC
#include<string>
#include <cstdio>
#define REFERENCE_STRING 600
#define NUMBER_OF_MEMORY_REFERENCES 180000
#define ENABLE_DEBUGMODE 1
using namespace std;
int outputArray[360];
int outputIndex = 0;
struct FIFO
{
	uint8_t isInMemory[REFERENCE_STRING+1];//index 0不使用
	uint8_t dirtyBits[REFERENCE_STRING+1];
	int pageFaultCount;
	int interruptCount;
	int diskWrite;
	int noPageFaultCount;//驗證
	queue<uint16_t> q;
};
struct FIFO fifo;
struct OPT
{
	uint8_t isInMemory[REFERENCE_STRING+1];
	uint8_t dirtyBits[REFERENCE_STRING+1];
	int pageFaultCount;
	int interruptCount;
	int diskWrite;
	int noPageFaultCount;//驗證
	vector<uint16_t> v;
};
struct OPT opt;
struct ESC
{
	uint8_t isInMemory[REFERENCE_STRING+1];	
	uint8_t referenceBits[REFERENCE_STRING+1];
	uint8_t dirtyBits[REFERENCE_STRING+1];
	int pageFaultCount;
	int interruptCount;
	int diskWrite;
	int noPageFaultCount;//驗證
	vector<uint16_t> v;
};
struct ESC esc;
struct MYALOG
{
	uint8_t isInMemory[REFERENCE_STRING+1];	
	int total[REFERENCE_STRING+1];
	uint8_t dirtyBits[REFERENCE_STRING+1];
	int pageFaultCount;
	int interruptCount;
	int diskWrite;
	int noPageFaultCount;//驗證
	vector<uint16_t> v;
};
struct MYALOG myAlgo;
void print_queue(queue<uint16_t> q)
{
	//test function
	while (!q.empty())
	{
		cout << q.front() << " ";
		q.pop();
	}
	cout << endl;
}
uint16_t *generateRandomReferenceString()
{
	uint16_t *inputString = new uint16_t[NUMBER_OF_MEMORY_REFERENCES];
	uint16_t test;
	for(int i=0; i<NUMBER_OF_MEMORY_REFERENCES; i++)
	{
		test = rand()%REFERENCE_STRING + 1;
		inputString[i] = test;
	}
	return inputString;
}
uint16_t *generateLocalReferenceString()
{
	uint16_t *inputString = new uint16_t[NUMBER_OF_MEMORY_REFERENCES];
	default_random_engine generator(time(NULL));
	int remainRefStr = 180000;
	int indexCount = 0;
	//開始湊滿subDst數量的資料，此時range和Head都不會變
	while(remainRefStr!=0)
	{
		//要貢獻多少次資料
		uniform_int_distribution<int> subDst(500, 1000);//貢獻資料的長度random500~1000
		int subSize = min(subDst(generator), remainRefStr);//快到最後一個的時候區間會很小，所以要比對一下
		//Head加randomRange就是取樣範圍
		uniform_int_distribution<int> randomRange(15, 20);//老師規定的reference string number範圍
		uniform_int_distribution<int> refHead(1, 580);//隨機挑起始點，我怕range挑到20
		int range = randomRange(generator);
		int Head = refHead(generator);
		uniform_int_distribution<int> localOne(Head, Head+range);
		for (int i = 0; i < subSize; i++) 
		{
			uint16_t ref = localOne(generator);//ref代表頁碼
			//cout<<"ref = "<<ref<<", indexCount = "<<indexCount<<"\t";
			inputString[indexCount++] = ref;
			//cout<<"inputString="<<inputString[indexCount-1]<<"\t";
		}
		remainRefStr -= subSize;
		//cout<<endl<<"range="<<range<<", Head="<<Head<<", subSize="<<subDst(generator)<<", remainRefStr="<<remainRefStr<<endl;
	}
	//cout<<endl;
	return inputString;
}
uint16_t *Find50SpecificNumber()
{
	uint16_t *inputString = new uint16_t[50];
	
    int i, j, num[50];
    
    for(i = 1 ; i <= 50 ; i++){
        do{
            num[i-1] = rand() % 600 + 1;  //隨機生成1~10之間(包含10)的整數亂數，但可能重複
            for(j = 1 ; j < i ; j++){    //使用num[j-1]作為之前已生成的陣列成員，並且與當前新生成的num[i-1]比較是否重複
                if(num[i-1] == num[j-1]) //在此判斷是否重複
                break;   //若在比對過程遇到重複，則提早break離開for迴圈，並重新生成新亂數
            }   //若num[j-1]比較到num[i-1]都沒有重複，代表這個亂數是新的可以使用，並且for結束時j=i，因此在下一行判斷跳出do/while迴圈
        }while(j != i);  
        //cout << num[i-1] <<",";
		inputString[i-1] = num[i-1];
    }
	//cout<<endl;
	return inputString;
}
uint16_t *generateMypickReferenceString()
{
	uint16_t *inputString = new uint16_t[NUMBER_OF_MEMORY_REFERENCES];
	uint16_t test;
	uint16_t *testString = Find50SpecificNumber();
	for(int i=0; i<NUMBER_OF_MEMORY_REFERENCES; i++)
	{
		test = rand()%REFERENCE_STRING + 1;
		inputString[i] = test;
	}
	int i = 0;
	uint8_t TCount = 0;
	while(true)
	{
		int interval = rand()%5 + 1;
		i+=interval;
		if(i>NUMBER_OF_MEMORY_REFERENCES)break;
		inputString[i] = testString[TCount++];
		if(TCount == 50)TCount = 0;
	}
	return inputString;
}
void reset(uint8_t isInMemory[REFERENCE_STRING+1], int &pageFaultCount, int &interruptCount, int &diskWrite, int &noPageFaultCount, uint8_t dirtyBits[REFERENCE_STRING+1])
{
	//fill(std::begin(isInMemory), std::end(isInMemory), 0);//isInMemory[REFERENCE_STRING+1] = {0};沒效果
	for(int i=0;i<REFERENCE_STRING+1;i++)isInMemory[i]=0;
	pageFaultCount = 0;
	interruptCount = 0;
	diskWrite = 0;
	noPageFaultCount = 0;
	for(int i=0;i<REFERENCE_STRING+1;i++)dirtyBits[i]=0;
}
void FIFO_algorithm(uint16_t * arr, uint8_t frameSize)
{
	reset(fifo.isInMemory, fifo.pageFaultCount, fifo.interruptCount, fifo.diskWrite, fifo.noPageFaultCount, fifo.dirtyBits);
	//fifo.q={};//can't use for macOS
    while(!fifo.q.empty())fifo.q.pop();
	//cout<<"frameSize="<<+frameSize<<endl;
	//start
	//arr[0]=1;arr[1]=2;arr[2]=3;arr[3]=4;arr[4]=5;arr[5]=6;arr[6]=7;arr[7]=8;arr[8]=9;arr[9]=10;arr[10]=8;arr[11]=15;arr[12]=14;arr[13]=7;arr[14]=5;arr[15]=15;arr[16]=14;
	for(int i=0;i<NUMBER_OF_MEMORY_REFERENCES;i++)
	{
		if(fifo.isInMemory[arr[i]] == 0)
		{
			//cout<<"Fifo.q.size() = "<<fifo.q.size()<<endl;
			if(fifo.q.size() == frameSize)//frame is full
			{
				//cout<<"1 Size full, fifo.q.size() = "<<fifo.q.size()<<endl;
				//cout<<"fifo.q.front(): "<<fifo.q.front()<<endl;
				uint16_t previous = fifo.q.front();
				//print_queue(fifo.q);
				fifo.q.pop();
				//print_queue(fifo.q);
				//cout<<"previous: "<<previous<<endl;
				fifo.isInMemory[previous] = 0;
				if(fifo.dirtyBits[previous] == 1)
				{
					fifo.diskWrite++;
					fifo.interruptCount++;
					fifo.dirtyBits[previous] = 0;
				}
				//cout<<"2 fifo.q.size() = "<<fifo.q.size()<<endl;
			}
			fifo.isInMemory[arr[i]] = 1;//在記憶體裡了, ref bit 一定要設1
			//cout<<"arr[i] = "<<arr[i]<<endl;
			fifo.q.push(arr[i]);
			fifo.pageFaultCount++;
			fifo.interruptCount++;
			fifo.dirtyBits[arr[i]] = ((rand()%4 + 1)==3)? 1 : 0;//四分之一機率
			//cout<<"fifo.dirtyBits[arr[i]] = "<<+fifo.dirtyBits[arr[i]]<<endl;
		}
		else fifo.noPageFaultCount++;
		//print_queue(fifo.q);
	}
	cout<<"fifo.pageFaultCount = "<<fifo.pageFaultCount<<", fifo.interruptCount = "<<fifo.interruptCount<<", fifo.diskWrite = "<<fifo.diskWrite<<endl;
	outputArray[outputIndex++] = fifo.pageFaultCount;
	outputArray[outputIndex++] = fifo.interruptCount;
	outputArray[outputIndex++] = fifo.diskWrite;
}
uint16_t predict(int current, uint8_t frameSize, uint16_t *arr)
{
	int currentIndex = current,furthest = 0;
	uint16_t replaceCandidateIndex = 0;//replaceCandidateIndex range:0~(framesize-1)
	for(uint16_t i=0;i<frameSize;i++)
	{
		//cout<<"i="<<i<<endl;
		int j;
		bool foundest = false;
		for(j=currentIndex;j<NUMBER_OF_MEMORY_REFERENCES;j++)
		{	//cout<<"arr["<<j<<"]="<<arr[j]<<endl;
			if(opt.v[i] == arr[j])
			{
				//cout<<"opt.v["<<i<<"]="<<opt.v[i]<<endl;
				//cout<<"arr["<<j<<"]="<<arr[j]<<endl;
				if(j>furthest)
				{
					furthest = j;
					replaceCandidateIndex = i;
					foundest = true;
				}
				break;//found it, no need to continue
			}
		}
		//未來沒有page是出現在page裡的，所以沒人可以替換，預設為index0(替換第一個)
		if(j == NUMBER_OF_MEMORY_REFERENCES && !foundest) return i;//未來都沒出現，直接替換你，要return i，因為上面break代表有找到，i會加1
	}
	return replaceCandidateIndex;
}
void OPT_algorithm(uint16_t *arr, uint8_t frameSize)
{
	//arr[0]=1;arr[1]=2;arr[2]=3;arr[3]=4;arr[4]=5;arr[5]=6;arr[6]=7;arr[7]=8;arr[8]=9;arr[9]=10;arr[10]=8;arr[11]=15;arr[12]=14;arr[13]=7;arr[14]=5;arr[15]=15;arr[16]=14;
	reset(opt.isInMemory, opt.pageFaultCount, opt.interruptCount, opt.diskWrite, opt.noPageFaultCount, opt.dirtyBits);
	opt.v.clear();
	//cout<<"opt.v.size() = "<<opt.v.size()<<" frame size = "<<+frameSize<<endl;
	//for(int i=0;i<600;i++)cout<<+opt.isInMemory[i]<<" ";
	//cout<<endl;
	for(int i=0;i<NUMBER_OF_MEMORY_REFERENCES;i++)
	{
		if(opt.isInMemory[arr[i]] == 0)
		{
			//cout<<arr[i]<<" is not in memory. ";
			if(opt.v.size() == frameSize)//frame is full
			{
				//Chose the victim page
				uint16_t victimIndex = predict(i+1,frameSize,arr);
				uint16_t previous = opt.v[victimIndex];
				opt.isInMemory[previous] = 0;
				opt.v[victimIndex] = arr[i];//replace
				//cout<<"victimIndex="<<victimIndex<<endl;
				if(opt.dirtyBits[previous] == 1)
				{
					opt.diskWrite++;
					opt.interruptCount++;
					opt.dirtyBits[previous] = 0;
				}
			}
			else
			{//跟FIFO不一樣，因為FIFO滿的話是移除，OPT的話是修改
				//cout<<"arr[i] = "<<arr[i]<<endl;
				opt.v.push_back(arr[i]);
			}
			opt.isInMemory[arr[i]] = 1;//在記憶體裡了, ref bit 一定要設1
			opt.pageFaultCount++;
			opt.interruptCount++;
			opt.dirtyBits[arr[i]] = ((rand()%4 + 1)==3)? 1 : 0;//四分之一機率
			//cout<<"opt.dirtyBits[arr[i]] = "<<+opt.dirtyBits[arr[i]]<<endl;
			// cout<<"vector:"<<endl;
			// for (auto i: opt.v) cout << i << ' ';
		}
		else opt.noPageFaultCount++;
	}
	cout<<"opt.pageFaultCount = "<<opt.pageFaultCount<<", opt.interruptCount = "<<opt.interruptCount<<", opt.diskWrite = "<<opt.diskWrite<<endl;
	outputArray[outputIndex++] = opt.pageFaultCount;
	outputArray[outputIndex++] = opt.interruptCount;
	outputArray[outputIndex++] = opt.diskWrite;
}
int8_t ESC_findVictim(uint8_t frameSize)
{
	// find (0, 0)
	for(int8_t i = 0; i < frameSize; i++){
		if(esc.referenceBits[esc.v[i]] == 0 && esc.dirtyBits[esc.v[i]] == 0)return i;
	}
	// find (0, 1)
	for(int8_t i = 0; i < frameSize; i++){
		if(esc.referenceBits[esc.v[i]] == 0 && esc.dirtyBits[esc.v[i]] == 1)return i;
	}
	return -1;
}
void ESC_checkReferenceBits(uint8_t frameSize)
{
	uint8_t i;
	for(i=0;i<frameSize;i++) if(esc.referenceBits[esc.v[i]] == 0)break;
	if(i == frameSize)
	{
		for(i=0;i<frameSize;i++) esc.referenceBits[esc.v[i]] = 0;
		esc.interruptCount++;
	}
}
void ESC_algorithm(uint16_t *arr, uint8_t frameSize)
{
	// arr[0]=1;arr[1]=2;arr[2]=3;arr[3]=4;arr[4]=5;arr[5]=6;arr[6]=7;arr[7]=8;arr[8]=9;arr[9]=10;arr[10]=8;arr[11]=15;arr[12]=14;arr[13]=7;arr[14]=5;arr[15]=15;arr[16]=14;
	
	reset(esc.isInMemory, esc.pageFaultCount, esc.interruptCount, esc.diskWrite, esc.noPageFaultCount, esc.dirtyBits);
	
	// esc.dirtyBits[0]=0;esc.dirtyBits[1]=0;esc.dirtyBits[2]=1;esc.dirtyBits[3]=0;esc.dirtyBits[4]=0;esc.dirtyBits[5]=1;esc.dirtyBits[6]=1;esc.dirtyBits[7]=0;
	// esc.dirtyBits[8]=1;esc.dirtyBits[9]=0;esc.dirtyBits[10]=1;esc.dirtyBits[11]=0;esc.dirtyBits[12]=1;esc.dirtyBits[13]=0;esc.dirtyBits[14]=0;
	// esc.dirtyBits[15]=1;esc.dirtyBits[16]=0;esc.dirtyBits[16]=1;
	esc.v.clear();
	for(int i=0;i<REFERENCE_STRING+1;i++)esc.referenceBits[i]=0;//addition reset
	//cout<<"esc.v.size() = "<<esc.v.size()<<" frame size = "<<+frameSize<<endl;
	//for(int i=0;i<600;i++)cout<<+esc.isInMemory[i]<<" ";
	//cout<<endl;
	for(int i=0;i<NUMBER_OF_MEMORY_REFERENCES;i++)
	{
		if(esc.isInMemory[arr[i]] == 0)
		{
			//cout<<arr[i]<<" is not in memory. "<<endl;
			if(esc.v.size() == frameSize)//frame is full
			{
CHECK:			uint16_t victimIndex = ESC_findVictim(frameSize);//victimIndex是frame，替換前先看Dirty先處理DW++
				if(victimIndex == -1){ESC_checkReferenceBits(frameSize);goto CHECK;}
				uint16_t previous = esc.v[victimIndex];//victim index->victim value
				esc.isInMemory[previous] = 0;
				esc.referenceBits[previous] = 0;//也可以不設，反正載近來也會在設1
				esc.v[victimIndex] = arr[i];//replace				
				
				if(esc.dirtyBits[previous] == 1)
				{
					esc.diskWrite++;
					esc.interruptCount++;
					esc.dirtyBits[previous] = 0;
				}
			}
			else
			{//frame is not full，only few times
				esc.v.push_back(arr[i]);
			}
			esc.referenceBits[arr[i]] = 1;
			esc.isInMemory[arr[i]] = 1;//在記憶體裡了, ref bit 一定要設1
			esc.pageFaultCount++;
			esc.interruptCount++;
			esc.dirtyBits[arr[i]] = ((rand()%4 + 1)==3)? 1 : 0;//四分之一機率			
		}
		else
		{
			//cout<<arr[i]<<" is in memory. "<<endl;
			esc.noPageFaultCount++;
			esc.referenceBits[arr[i]] = 1;
		}
		// cout<<"vector:"<<endl;
		// for (int j=0;j<10;j++)
		// {
			// cout << esc.v[j] << ',';
			// cout<<+esc.referenceBits[esc.v[j]]<<",";
			// cout<<+esc.dirtyBits[esc.v[j]]<<endl;
			// cout<<"PF = "<<esc.pageFaultCount<<", IR = "<<esc.interruptCount<<", DW = "<<esc.diskWrite<<endl;
		// }
		// getchar();
		// cout<<endl;
		if(esc.v.size() == frameSize) ESC_checkReferenceBits(frameSize);//檢查ref bit有沒有全1，有的話全部清0
		
	}
	cout<<"esc.pageFaultCount = "<<esc.pageFaultCount<<", esc.interruptCount = "<<esc.interruptCount<<", esc.diskWrite = "<<esc.diskWrite<<endl;
	outputArray[outputIndex++] = esc.pageFaultCount;
	outputArray[outputIndex++] = esc.interruptCount;
	outputArray[outputIndex++] = esc.diskWrite;
}
uint16_t myAlgo_findVictim(uint8_t frameSize)
{
	// find minimum index
	struct TEMP
	{
		uint16_t value;
		uint16_t originalIndex;
	};
	uint16_t minimum = NUMBER_OF_MEMORY_REFERENCES, minimumIndex = 0;
	// vector<struct TEMP> temp;
	// for(uint16_t i = 0; i < frameSize; i++)
	// {
		// struct TEMP t;
		// if(myAlgo.dirtyBits[myAlgo.v[i]]==0)
		// {
			// t.value = myAlgo.v[i];
			// t.originalIndex = i;
			// temp.push_back(t);
		// }
	// }
	// if(temp.size()!=0)
	// {
		// for(uint16_t i = 0; i < temp.size(); i++)
		// {
			// if(myAlgo.total[temp[i].value] < minimum )
			// {
				// minimum = myAlgo.total[temp[i].value];
				// minimumIndex = temp[i].originalIndex;
			// }
		// }
	// }
	// else
	// {
		for(uint16_t i = 0; i < frameSize; i++)
		{
			if(myAlgo.total[myAlgo.v[i]] < minimum )
			{
				minimum = myAlgo.total[myAlgo.v[i]];
				minimumIndex = i;
			}
		}
	// }
	// temp.clear();
	return minimumIndex;
}
void My_algorithm(uint16_t *arr, uint8_t frameSize)
{
	// arr[0]=1;arr[1]=2;arr[2]=3;arr[3]=4;arr[4]=5;arr[5]=6;arr[6]=7;arr[7]=8;arr[8]=9;arr[9]=10;arr[10]=8;arr[11]=15;arr[12]=14;arr[13]=7;arr[14]=5;arr[15]=15;arr[16]=14;
	
	reset(myAlgo.isInMemory, myAlgo.pageFaultCount, myAlgo.interruptCount, myAlgo.diskWrite, myAlgo.noPageFaultCount, myAlgo.dirtyBits);
	myAlgo.v.clear();
	for(int i=0;i<REFERENCE_STRING+1;i++)myAlgo.total[i]=0;
	for(int i=0;i<NUMBER_OF_MEMORY_REFERENCES;i++)
	{
		if(i%1000==0)for(int i=0;i<REFERENCE_STRING+1;i++)myAlgo.total[i]=0;
		myAlgo.total[arr[i]] = myAlgo.total[arr[i]] + 1;
		if(myAlgo.isInMemory[arr[i]] == 0)
		{
			//cout<<arr[i]<<" is not in memory. "<<endl;
			if(myAlgo.v.size() == frameSize)//frame is full
			{
				uint16_t victimIndex = myAlgo_findVictim(frameSize);//victimIndex是frame，替換前先看Dirty先處理DW++
				uint16_t previous = myAlgo.v[victimIndex];//victim index->victim value
				myAlgo.isInMemory[previous] = 0;
				myAlgo.v[victimIndex] = arr[i];//replace				
				
				if(myAlgo.dirtyBits[previous] == 1)
				{
					myAlgo.diskWrite++;
					myAlgo.interruptCount++;
					myAlgo.dirtyBits[previous] = 0;
				}
			}
			else
			{//
				//cout<<"frame is not full，only few times"<<endl;
				myAlgo.v.push_back(arr[i]);
			}
			myAlgo.isInMemory[arr[i]] = 1;//在記憶體裡了, ref bit 一定要設1
			myAlgo.pageFaultCount++;
			myAlgo.interruptCount++;
			myAlgo.dirtyBits[arr[i]] = ((rand()%4 + 1)==3)? 1 : 0;//四分之一機率			
		}
		else
		{
			//cout<<arr[i]<<" is in memory. "<<endl;
			myAlgo.noPageFaultCount++;
		}
		// cout<<"vector:"<<endl;
		// for (int j=0;j<10 && myAlgo.v.size() == 10;j++)
		// {
			// cout << myAlgo.v[j] << ',';
			// cout<<+myAlgo.total[myAlgo.v[j]]<<",";
			// cout<<+myAlgo.dirtyBits[myAlgo.v[j]]<<endl;
			// cout<<"PF = "<<myAlgo.pageFaultCount<<", IR = "<<myAlgo.interruptCount<<", DW = "<<myAlgo.diskWrite<<endl;
		// }
		// getchar();
		// cout<<endl;		
	}
	cout<<"myAlgo.pageFaultCount = "<<myAlgo.pageFaultCount<<", myAlgo.interruptCount = "<<myAlgo.interruptCount<<", myAlgo.diskWrite = "<<myAlgo.diskWrite<<endl;
	outputArray[outputIndex++] = myAlgo.pageFaultCount;
	outputArray[outputIndex++] = myAlgo.interruptCount;
	outputArray[outputIndex++] = myAlgo.diskWrite;
}
int main()
{
	srand((unsigned int)time(NULL));
	cout<<"Random test reference string:"<<endl;
	uint16_t *inputString = new uint16_t[NUMBER_OF_MEMORY_REFERENCES];
	inputString = generateRandomReferenceString();
	for(int i=10;i<=100;i+=10) FIFO_algorithm(inputString, i);cout<<endl;
	for(int i=10;i<=100;i+=10) OPT_algorithm(inputString, i);cout<<endl;
	for(int i=10;i<=100;i+=10) ESC_algorithm(inputString, i);cout<<endl;
	for(int i=10;i<=100;i+=10) My_algorithm(inputString, i);cout<<endl;
	cout<<endl<<"Local test reference string:"<<endl;
	inputString = generateLocalReferenceString();
	for(int i=10;i<=100;i+=10) FIFO_algorithm(inputString, i);cout<<endl;
	for(int i=10;i<=100;i+=10) OPT_algorithm(inputString, i);cout<<endl;
	for(int i=10;i<=100;i+=10) ESC_algorithm(inputString, i);cout<<endl;
	for(int i=10;i<=100;i+=10) My_algorithm(inputString, i);cout<<endl;
	cout<<endl<<"My pick reference string:"<<endl;
	inputString = generateMypickReferenceString();
	for(int i=10;i<=100;i+=10) FIFO_algorithm(inputString, i);cout<<endl;
	for(int i=10;i<=100;i+=10) OPT_algorithm(inputString, i);cout<<endl;
	for(int i=10;i<=100;i+=10) ESC_algorithm(inputString, i);cout<<endl;
	for(int i=10;i<=100;i+=10) My_algorithm(inputString, i);cout<<endl;
	delete inputString;
	string line = "./a.out ";
	for(int i=0;i<360;i++) line += to_string(outputArray[i])+" ";
	system(line.c_str());  
	
	return 0;
}
