// http://curl.haxx.se/docs/copyright.html.
#pragma warning (disable : 4018)

//#include "setup.h"
#include <conio.h>
#include "parser.h"
#include "stack.h"
#include <curl/curl.h>
#include "utils.h"

typedef struct
{
	char* website;
	char* path;
	char* filename;
	FILE* fp; // unused for now.
	int currentID; // id of the file currently being downloaded;
	int currentLevel;
	int filetype;
} callbackInfoT;


void freelink(urlT* link)
{
//	free(link->filename);
	free(link->website);
	free(link->path);
	free(link);
}

//global variables are preceeded by "g_"
machDataT g_contentLenght, g_contentType;
typedef enum {UNKNOWN, HTML, ZIP} filetypeT;
filetypeT g_currentFileType;
stackT* g_s;
callbackInfoT g_cbi;

size_t write_body(char *ptr, size_t size, size_t nmemb, FILE *stream)
{
    int written;
	int i;
	urlT* link;

	switch(g_cbi.filetype)
	{
		case HTML:
			// parse html for more links
			for (i=0; i<size*nmemb; i++)
			{
				if (link = peek(ptr[i]))
				{
					if (link->website[0]=='.')
					{
						free(link->website);
						link->website = newString(g_cbi.website);
					}

					if ( strcmp(link->website, g_cbi.website)==0) // internal link
					{
						link->level = g_cbi.currentLevel + 1;
						link->parent = g_cbi.currentID;
						link->path = newString(g_cbi.path);
						stack_push(g_s, link);
					}
					// else ignore external links
				}
			}
		// no break here, save all file types
		case ZIP:
		default:
			written = fwrite(ptr, size, nmemb, stream);
			break;
	}

    return written;
}

size_t write_header(char *ptr, size_t size, size_t nmemb, FILE *stream)
{
    int i;
	int written = size*nmemb;
	// written = fwrite(ptr, size, nmemb, stream);

	for (i=0; i<written; i++)
	{
		if (pattern_match(&g_contentLenght, ptr[i])==2)
		{
			printf("Len = %s\n", g_contentLenght.value);
		}
		if (pattern_match(&g_contentType, ptr[i])==2)
		{
			printf("Type = %s\n", g_contentType.value);
			if (strstr(g_contentType.value, "html"))
				g_cbi.filetype = HTML;
			if (strstr(g_contentType.value, "zip"))
				g_cbi.filetype = ZIP;
		}
	} // for(..)

    return written;
}



void parse(char* website, char* path, int depth)
{
	// links depper than <depth> will not be examined
//  FILE *headerfile;
//	char headerfilename[MAXPATHLEN] = "C:\\header.txt";

	CURL* curl;
    CURLcode res;
	char* curlstring;
	char url[MAXPATHLEN], tmp[MAXPATHLEN];
	urlT* link;
	char * root="c:\\Cobra\\";
	int nr = 0;
	FILE* fp;

    char outfilename[MAXPATHLEN];

	FILE* indexFile;
	char aux[MAXPATHLEN];

	concatenatePath(aux, root, website, path, "index.txt");
	indexFile=openFile(aux, "w");

	pattern_init(&g_contentLenght, "Content-Length:");
	pattern_init(&g_contentType, "Content-Type:");

	g_s = stack_init(16);
		
	curl = curl_easy_init();
	if (!curl)
	{
		//report error
		return;
	}

//	headerfile = fopen(headerfilename,"w");

	// no progress meter
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);

	// send body data to this function
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_body);
    
	// send header data to this function
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, write_header);
	
	// we want the header function to use this file handle
	// curl_easy_setopt(curl, CURLOPT_WRITEHEADER, headerfile);

	link = (urlT*)malloc(sizeof(urlT));
	link->website = newString(website);
	link->path = newString(path);
	link->filename = newString("");
	link->level=0;
	link->parent = -1; // root has no parent
	stack_push(g_s, link);

	while (!stack_empty(g_s))
	{
		nr++;
		itoa(nr, aux, 10);
		link = stack_pop(g_s);
	/*	if (strstr(link->path, ".zip")!=NULL)
		{
			freelink(link);
			continue;
		} */

		if (link->level>depth)
		{
			freelink(link);
			continue;
		}
		
		strcpy(tmp, link->filename);
		if (strlen(tmp)>0)
			strcat(tmp, "\\");
		strcat(tmp, aux);
		concatenatePath(outfilename, root, link->website, link->path, tmp);
		fp = openFile(outfilename,"wb");
		concatenatePath(outfilename, root, link->website, link->path, link->filename);
		normalisePathSeparators(outfilename);
		fprintf(indexFile, "%d	%d	%s\n",nr, link->parent, outfilename);
		g_cbi.website = link->website;
		strcpy(tmp,link->path);
		strcat(tmp, link->filename);
		g_cbi.path = tmp;
		g_cbi.currentID = nr;
		g_cbi.currentLevel = link->level;
		g_cbi.filetype = UNKNOWN;

		//tmp = normaliseUrlString(link->root, strlen(link->root)+1);
		//printf("%s --> %s --> %s \n", tmp, link->website, link->path);
		// 	free(tmp);

		if (strlen(website) + strlen(path)>= MAXPATHLEN)
		{
			printf("Maximum URL length exceeded.");
			exit(0);
		}

		concatenateUrl(url, link->website, link->path, link->filename, "");
		
		//ugly hack!
		if (strstr(url,".zip")==NULL && url[strlen(url)-1]!='/')
			strcat(url,"/");

		curl_easy_setopt(curl, CURLOPT_URL, url);


		// we want the body function to use this file handle
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

		res = curl_easy_perform(curl);

		pattern_reset(&g_contentLenght);
		pattern_reset(&g_contentType);

		fclose(fp);
		//fclose(headerfile);
		freelink(link);
	} // while (!stack_empty(g_s))

	// always cleanup
	curl_easy_cleanup(curl);

	fclose(indexFile);
}


int main(int argc, char *argv[])
{

	char * tmp = "https://www.google.com/doodles/finder/2012/All%20doodles";
	char* aux = fileNameFromUrl(tmp, strlen(tmp));

	//char *url = "http://ratedata.gaincapital.com/2003/01%20January/USD_JPY.zip";

//	analyse(url, "C:\\body.txt");	
//	analyseHeader(url, "C:\\header.txt");		
	char c;

	parse("ratedata.gaincapital.com/", "2000/", 3);
	c = _getch();
}

