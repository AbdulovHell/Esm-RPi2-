#include <chrono>
#include <iostream>
#include <fstream>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
	system("/esm/update.sh");
	system("systemctl status esminec.service");
	return 0;
}