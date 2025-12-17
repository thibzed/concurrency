#include <iostream>
int main(){
    long long int counter = 0;
    for (long long int i = 0 ; i < 100'000'000 ; i++){
        counter+= i;
    }
    std::cout << counter;
    return 0;
}