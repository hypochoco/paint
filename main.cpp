
//
//  main.cpp
//  engine
//
//  Created by Daniel Cho on 9/30/25.
//

#include <iostream>

#include "paint/application.h"

int main() {

	try {
         Application app;
         app.init();
         app.run();
         app.cleanup();
     } catch (const std::exception& e) {
         std::cerr << e.what() << std::endl;
         return EXIT_FAILURE;
     }
     return EXIT_SUCCESS;

}
