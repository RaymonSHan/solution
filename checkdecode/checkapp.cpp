
#include "checkdll.h"

CheckDecode decode;

int main(int argc, char* argv[])
{
  RESULT result;
  char check[MAX_RECT];
//  result = decode.Process(argv[1], check);
  result = decode.ProcessAndDisplay(argv[1], check);

//  result = decode.TestProcess(argv[1]);

  cvWaitKey(0);

	return 0;
};
