#include <curl\curl.h>
#include <iostream>
#include <stdio.h>
#include <string>



size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    int written = fwrite(ptr, size, nmemb, stream);
    return written;
}

int main(int argc, char** argv)
{
    CURL *curl;
    FILE *fp;
    CURLcode res;
    char *url = "http://ratedata.gaincapital.com/2003/01%20January/";
    char outfilename[FILENAME_MAX] = "C:\\bbb.txt";
    curl = curl_easy_init();
    if (curl) {
        fp = fopen(outfilename,"wb");
        curl_easy_setopt(curl, CURLOPT_URL, "http://ratedata.gaincapital.com/2003/01%20January/");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        res = curl_easy_perform(curl);
        /* always cleanup */
        curl_easy_cleanup(curl);
        fclose(fp);
    }
    return 0;
}


/*
using namespace std;


int main(int argc, char* argv[])
{
	cout<<"Running.."<<endl;
	char c;
	cin>>c;
}
*/