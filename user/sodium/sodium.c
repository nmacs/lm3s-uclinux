#include <sodium.h>

int main(int argc, char *argv[])
{
	int res = crypto_sign_open(argv[1], argv[2], argv[3], argv[4], argv[5]);
	return res;
}