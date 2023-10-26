#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <time.h>
#include <chrono>
#include <atomic>
using namespace std;

//use to force failures
//std::this_thread::sleep_for(std::chrono::milliseconds(2));
mutex mcout;
mutex m;
bool depositfinished=false;
void out1(string s){
	lock_guard<mutex> lck(mcout);
	cout<<s<<endl;
}

atomic<int> bal(0);	//bank balance
void deposit(int id,int numbdeposits, int amt){
	while(numbdeposits>0){
		bal=bal+amt;
		numbdeposits--;
		out1("Thread "+to_string(id)+" deposited "+to_string(amt)+" bal="+to_string(bal));
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	depositfinished=true;
	out1("Thread "+to_string(id)+" IS DONE!,bal="+to_string(bal));
}
void withdraw(int id,int amt ){
	{
		while(!depositfinished || bal>0){	//this is a busy wait, fix later with condition variables
			{  //scope below lock
				unique_lock<mutex> lck(m);	//just like a lock_guard but also unlockable
				if(bal>0){
					bal=bal-amt;
					lck.unlock();//unlock so dont hold lck while next statement executes, thisincreases concurrency
					out1(" Thread "+to_string(id)+" withdrew "+to_string(amt)+" bal="+to_string(bal));
					continue;	//go back to while
				}
			}
			out1(" Thread "+to_string(id)+" WAITING");	//add delay to each deposit to show how often this loop just waits
		}
	}
	out1("Thread "+to_string(id)+" IS DONE!,bal="+to_string(bal));
}
int main() {
	//make a bunch of deposits, and withdrawels
	//without overdrafting, but keep withdrawing until
	//bal is 0
	thread t1(deposit,1,100,1);
	thread t2(withdraw,2,1);
	thread t3(withdraw,3,1);
	thread t4(withdraw,4,1);
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	cout<<"DONE! bal="<<bal<<endl;
}

