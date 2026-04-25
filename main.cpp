#include <chrono>
#include "TaskManager.h"
using namespace std;

void task(int ID){
    cout<<"Worker thread "<< this_thread::get_id() <<" is starting its task." << ID<<endl;
    this_thread::sleep_for(chrono::seconds(1));
    cout<<"Task "<< ID<<" is done."<<endl;

}

int main(){
    ThreadPool pool(3);

    for(int i = 0; i<=6;i++){
        pool.Enqueue([i](){task(i);});
    }
    cout << "Main thread is hanging out while workers do the job..." << endl;
    this_thread::sleep_for(chrono::seconds(6));
    cout << "Main thread exiting. Pool destructor will now clean up." << endl;

    return 0;
}