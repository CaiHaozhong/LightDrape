#include <memory>
#define smartNew(Class) std::make_shared<Class>()

#define S_PTR(Class) typedef std::shared_ptr<Class> Class##_

#include <climits>

#include <iostream>
#ifdef _DEBUG_
#define PRINT(str) std::cout << str
#define PRINTLN(str) std::cout << str << std::endl
#else
#define PRINT(str)
#define PRINTLN(str)
#endif // _DEBUG_