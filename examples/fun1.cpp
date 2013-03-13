#include<capability.h>
#include<stdio.h>
#include<string.h>
#include "fun1.h"

extern "C" {
	CCapability *newFun() {
		exampleFun1 *item = new exampleFun1();
		return (CCapability*)item;
	}
	void run( CCapability* item, void *argv ) {
		(exampleFun1 *)item->run( argv );
	}
	void getKey( CCapability* item,long long int *k){
		(exampleFun1 *)item->getKey( k );
	}
	void destroy( CCapability* item ) {
		(exampleFun1 *)item->destroy();
	}
	long long int md2_5() {
		return 0x1318bb259d65ee19LL;
	}
};	


long long int exampleFun1::key[] = {md2_5(),0x7e95373c35421368LL};
int exampleFun1::getKey( long long int *k ) {
	k[0]=key[0];
	k[1]=key[1];
	return 1;
}

int exampleFun1::run( void *argv ) {
	const char *str = (const char *)argv;
	char buf[100];
	memcpy(buf,str,32);
	buf[32]='\0';
	printf("I am 1 %s\n",buf);
	return 1;
}

int exampleFun1::destroy( ) {
	delete this;
	return 1;
}

