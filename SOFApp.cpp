// SOFApp.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "Assignment.h"

int main()
{
	Assignment *app = new Assignment();
	SOF::Window::CreateWindow(1024, 768, "Week9", app);
	SOF::Window::Run();

    return 0;
}

