
#include "checkdll.h"
#include "checkmore.h"

//CheckDecode decode;
CheckMore more;

int main(int argc, char* argv[])
{
  RESULT result;
  char check[MAX_RECT];

  result = more.Process(argv[1], check, true);

  cvWaitKey(0);

	return 0;
};
