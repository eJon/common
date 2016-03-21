#include "Bucket.h"
#include "ADutil.h"

using namespace std;

CBucket::CBucket()
{
	m_configId = 0;
	m_bucketId = 0;
	m_pathId = 0;
}

CBucket::~CBucket()
{
}

string CBucket::toBucketString()const
{
	string bucketStr;
	char temp[100];
	sprintf(temp, "%d;", m_bucketId);
	
	return temp;
}
