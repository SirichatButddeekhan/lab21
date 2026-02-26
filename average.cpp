#include<iostream>
using namespace std;


int main(int argc,char * argv[]){
    double sum = 0.0;
    double avg;
    int count;
    count = argc-1;
    if (argc == 1) {
        cout << "Please input numbers to find average." << endl;
        return 0;
    }
    for(int i= 1;i < argc;i++){
        sum += stod(argv[i]);
    }
    avg = sum/count;
    cout << "---------------------------------" << endl;
    cout << "Average of " << count << " numbers = " << avg << endl;
    cout << "---------------------------------" << endl;

}
