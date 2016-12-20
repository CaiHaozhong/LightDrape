#include <memory>
#define smartNew(Class) std::make_shared<Class>()

#define S_PTR(Class) typedef std::shared_ptr<Class> Class##_

#include <climits>