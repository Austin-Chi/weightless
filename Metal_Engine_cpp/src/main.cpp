//
//  main.cpp
//  Metal_Engine_cpp
//
//  Created by 錡亭勳 on 4/19/26.
//

#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION

#include "config.h"
#include "control/app.h"

int main(int argc, const char * argv[]) {
    srand( time(NULL) );
    NS::AutoreleasePool* autoreleasePool = NS::AutoreleasePool::alloc()->init();
       
    App* app = new App;
    app->run();
    delete app;
    
    autoreleasePool->release();
    
    return 0;
}
