#include <iostream>
#include "Application.h"

int main(int argc, char** argv)
{
	try
	{
		Application app;
		app.FindPath("maps/map2.bmp");
	}
	catch (const std::exception& e)
	{
		std::cout << "Error: " << e.what() << std::endl;
		return (EXIT_FAILURE);
	}
	catch (...)
	{
		std::cout << "Unknown error" << std::endl;
		return (EXIT_FAILURE);
	}

	return (EXIT_SUCCESS);
}