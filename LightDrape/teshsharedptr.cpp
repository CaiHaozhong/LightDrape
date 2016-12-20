#include <memory>
#include <iostream>
#include <vector>
class A{
public:
	virtual char get(){
		return 'A';
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
	std::vector<A_> list;	
	list.push_back(smartNew(A));
	list.push_back(smartNew(B));
	A_ a = smartNew(B);
	A_ b = a;
	std::cout << a->get() << ' ' << b->get();
	for (int i = 0; i < list.size(); i++){
		std::cout << list.at(i)->get() << std::endl;
	}
	getchar();
}