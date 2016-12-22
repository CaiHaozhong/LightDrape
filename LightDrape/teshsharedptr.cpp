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
    std::vector<int> list;
    list.push_back(1);
    int n = 2;
    int& m = n;
    list[0] = m;
    m = 3;
    std::cout << list[0];
	
	getchar();
}