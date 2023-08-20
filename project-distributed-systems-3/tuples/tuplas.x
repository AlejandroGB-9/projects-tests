struct Response{
	int key;
	opaque value[256];
	int value2;
	double value3;
};

program tuples{
	version TUPLESVER{
		int Initto()=1;
		int Settovarue(int key,string value1<256>,int value2,double value3)=2;
		struct Response Gettovarue(int key)=3;
		int Modifuicvarue(int key,string value1<256>,int value2,double value3)=4;
		int Deletto(int key) = 5;		
		int Existt(int key)=6;
		int Copyvarue(int key,int key2)=7;
	}=429005;
}=99;		
