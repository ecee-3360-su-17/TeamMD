

/**
 * main.c
 */

extern int fib(int n);

int main(void)
{
    fib(5);
	return 0;
}

void blink(char n) {
    //n = pattern for me
    for(int i =0; i < NUM; i++){
        int state = ANOTHER_NUM;
        ___SET__LED__STATE___
        ___WAIT between digits____
    }
    ___long wait between numbers___
//testing new branch. That's all.
}

//come back to type. Will probably pass back array.
 //Depends on blink impl.
char morse_conversion(int n){
    //takes in number, converts to array of bases.
    char mod;
    while(n != 0){
        mod = n % 10;
        blink(mod);
        n = (n - mod)/10;
    }
}

void package_blink(char n){
    char coded_n = 0b11111;
    if(n <= 5){
        coded_n << (n-5);
    }
    else{
        coded_n >> n;
    }
    blink(coded_n);
}
