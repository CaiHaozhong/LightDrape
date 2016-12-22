#include <memory>
#include <iostream>
#include <vector>
class A{
public:
	virtual char get(){
		return 'A';
	}
	operator int () {
		return 1;
	}
};
class B : public A{
public:
	char get(){
		return 'B';
	}
};
typedef std::shared_ptr<A> A_;
typedef std::shared_ptr<B> B_;
#define smartNew(Class) (std::make_shared<Class>())
int main(){

	int i = A();
	std::cout << i;
	getchar();
}